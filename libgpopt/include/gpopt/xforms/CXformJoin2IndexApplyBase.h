//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal Software Inc.
//
//	Base class for transforming Join to Index Apply
//---------------------------------------------------------------------------
#ifndef GPOPT_CXformJoin2IndexApplyBase_H
#define GPOPT_CXformJoin2IndexApplyBase_H

#include "gpos/base.h"
#include "gpopt/operators/ops.h"
#include "gpopt/xforms/CXformJoin2IndexApply.h"

namespace gpopt
{
	using namespace gpos;

	template<class TJoin, class TApply, class TGet, BOOL fWithSelect, BOOL fPartial, IMDIndex::EmdindexType eidxtype>
	class CXformJoin2IndexApplyBase : public CXformJoin2IndexApply
	{
		private:

			// private copy ctor
			CXformJoin2IndexApplyBase(const CXformJoin2IndexApplyBase &);
			
			// Can transform left outer join to left outer index apply?
			// For hash distributed table, we can do outer index apply only
			// when the inner columns used in the join condition contains
			// the inner distribution key set. Master only table is ok to
			// transform to outer index apply, but random table is not.
			BOOL
			FCanLeftOuterIndexApply
				(
				IMemoryPool *pmp,
				CExpression *pexprInner,
				CExpression *pexprScalar
				) const
			{
				TGet *popGet = TGet::PopConvert(pexprInner->Pop());
				IMDRelation::Ereldistrpolicy ereldist = popGet->Ptabdesc()->Ereldistribution();

				if (ereldist == IMDRelation::EreldistrRandom)
					return false;
				else if (ereldist == IMDRelation::EreldistrMasterOnly)
					return true;

				// now consider hash distributed table
				CColRefSet *pcrsInnerOutput = CDrvdPropRelational::Pdprel(pexprInner->PdpDerive())->PcrsOutput();
				CColRefSet *pcrsScalarExpr = CDrvdPropScalar::Pdpscalar(pexprScalar->PdpDerive())->PcrsUsed();
				CColRefSet *pcrsInnerRefs = GPOS_NEW(pmp) CColRefSet(pmp, *pcrsScalarExpr);
				pcrsInnerRefs->Intersection(pcrsInnerOutput);

				// Distribution key set of inner GET must be subset of inner columns used in
				// the left outer join condition, but doesn't need to be equal. Given
				// R (a, b, c) distributed by (a)
				// S (a, b, c) distributed by (a), with index1 on S(a), index2 on S(b)
				// R LOJ S on R.a=S.a and R.c=S.c, we are good to do outer index NL join.
				// R LOJ S on R.a=S.a and R.b=S.b, we need to take care that in non-distributed
				// scenario, it is fine to choose either condition as the index condition,
				// and the other one as the index join filter. But in distributed scenario,
				// we have to choose R.a=S.a as the index condition, R.b=S.b as the index
				// filter, because S is hash distributed by a, indicating S.b is randomly
				// distributed in every segment, nothing we can do on table R to match
				// the distribution spec of S if we want to utilize index on S(b). This
				// should be taken care of right before adding the outer index apply
				// alternative by guarding check.
				BOOL FCanOuterIndexApply = pcrsInnerRefs->FSubset(popGet->PcrsDist());
				pcrsInnerRefs->Release();
				return FCanOuterIndexApply;
			}

		protected:

			virtual
			CLogicalJoin *PopLogicalJoin(IMemoryPool *pmp) const
			{
				return GPOS_NEW(pmp) TJoin(pmp);
			}

			virtual
			CLogicalApply *PopLogicalApply
				(
				IMemoryPool *pmp,
				DrgPcr *pdrgpcr
				) const
			{
				return GPOS_NEW(pmp) TApply(pmp, pdrgpcr);
			}

		public:

			// ctor
			explicit
			CXformJoin2IndexApplyBase<TJoin, TApply, TGet, fWithSelect, fPartial, eidxtype>(IMemoryPool *pmp)
			:
			// pattern
			CXformJoin2IndexApply
			(
				GPOS_NEW(pmp) CExpression
				(
				pmp,
				GPOS_NEW(pmp) TJoin(pmp),
				GPOS_NEW(pmp) CExpression(pmp, GPOS_NEW(pmp) CPatternLeaf(pmp)), // outer child
					fWithSelect
					?
					GPOS_NEW(pmp) CExpression  // inner child with Select operator
						(
						pmp,
						GPOS_NEW(pmp) CLogicalSelect(pmp),
						GPOS_NEW(pmp) CExpression(pmp, GPOS_NEW(pmp) TGet(pmp)), // Get below Select
						GPOS_NEW(pmp) CExpression(pmp, GPOS_NEW(pmp) CPatternTree(pmp))  // predicate
						)
					:
					GPOS_NEW(pmp) CExpression(pmp, GPOS_NEW(pmp) TGet(pmp)), // inner child with Get operator,
				GPOS_NEW(pmp) CExpression(pmp, GPOS_NEW(pmp) CPatternTree(pmp))  // predicate tree
				)
			)
			{}

			// dtor
			virtual
			~CXformJoin2IndexApplyBase<TJoin, TApply, TGet, fWithSelect, fPartial, eidxtype>()
			{}

			// actual transform
			virtual
			void Transform(CXformContext *pxfctxt, CXformResult *pxfres, CExpression *pexpr) const
			{
				GPOS_ASSERT(NULL != pxfctxt);
				GPOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
				GPOS_ASSERT(FCheckPattern(pexpr));

				IMemoryPool *pmp = pxfctxt->Pmp();

				// extract components
				CExpression *pexprOuter = (*pexpr)[0];
				CExpression *pexprInner = (*pexpr)[1];
				CExpression *pexprScalar = (*pexpr)[2];

				CExpression *pexprGet = pexprInner;
				CExpression *pexprAllPredicates = pexprScalar;

				if (fWithSelect)
				{
					pexprGet = (*pexprInner)[0];
					pexprAllPredicates = CPredicateUtils::PexprConjunction(pmp, pexprScalar, (*pexprInner)[1]);
				}
				else
				{
					pexprScalar->AddRef();
				}

				if (pexpr->Pop()->Eopid() == COperator::EopLogicalLeftOuterJoin &&
						!FCanLeftOuterIndexApply(pmp, pexprGet, pexprScalar))
				{
					// It is a left outer join, but we can't do outer index apply,
					// stop transforming and return immediately.
					CRefCount::SafeRelease(pexprAllPredicates);
					return;
				}

				CLogicalDynamicGet *popDynamicGet = NULL;
				if (COperator::EopLogicalDynamicGet == pexprGet->Pop()->Eopid())
				{
					popDynamicGet = CLogicalDynamicGet::PopConvert(pexprGet->Pop());
				}

				CTableDescriptor *ptabdescInner = TGet::PopConvert(pexprGet->Pop())->Ptabdesc();
				if (fPartial)
				{
					CreatePartialIndexApplyAlternatives
						(
						pmp,
						pexpr->Pop()->UlOpId(),
						pexprOuter,
						pexprInner,
						pexprAllPredicates,
						ptabdescInner,
						popDynamicGet,
						pxfres
						);
				}
				else
				{
					CreateHomogeneousIndexApplyAlternatives
						(
						pmp,
						pexpr->Pop()->UlOpId(),
						pexprOuter,
						pexprGet,
						pexprAllPredicates,
						ptabdescInner,
						popDynamicGet,
						pxfres,
						eidxtype
						);
				}
				CRefCount::SafeRelease(pexprAllPredicates);
			}

	}; // class CXformJoin2IndexApplyBase

}

#endif // !GPOPT_CXformJoin2IndexApplyBase_H

// EOF
