//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal Software Inc.
//
//	@filename:
//		CXformRemoveSubqDistinct.cpp
//
//	@doc:
//		Implementation of the transform that removes distinct clause from subquery
//---------------------------------------------------------------------------

#include "gpos/base.h"

#include "gpopt/operators/ops.h"
#include "gpopt/operators/COperator.h"
#include "gpopt/xforms/CXformUtils.h"
#include "gpopt/xforms/CXformRemoveSubqDistinct.h"

using namespace gpopt;

CXformRemoveSubqDistinct::CXformRemoveSubqDistinct
	(
	IMemoryPool *pmp
	)
	:
	// pattern
	CXformExploration
	(
	GPOS_NEW(pmp) CExpression
			(
			pmp,
			GPOS_NEW(pmp) CLogicalSelect(pmp),
			GPOS_NEW(pmp) CExpression(pmp, GPOS_NEW(pmp) CPatternLeaf(pmp)), // relational child
			GPOS_NEW(pmp) CExpression(pmp, GPOS_NEW(pmp) CPatternTree(pmp))	// predicate tree
			)
	)
{}

CXform::EXformPromise
CXformRemoveSubqDistinct::Exfp
	(
	CExpressionHandle &exprhdl
	)
	const
{
	// consider this transformation only if subqueries exist
	if (!exprhdl.Pdpscalar(1)->FHasSubquery())
	{
		return CXform::ExfpNone;
	}

	CExpression *pexprScalar = exprhdl.PexprScalarChild(1);
	COperator *pop = pexprScalar->Pop();
	if (CUtils::FQuantifiedSubquery(pop) || CUtils::FExistentialSubquery(pop))
	{
		return CXform::ExfpHigh;
	}

	return CXform::ExfpNone;
}

void
CXformRemoveSubqDistinct::Transform
	(
	CXformContext *pxfctxt,
	CXformResult *pxfres,
	CExpression *pexpr
	)
	const
{
	GPOS_ASSERT(NULL != pxfctxt);
	GPOS_ASSERT(NULL != pxfres);
	GPOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	GPOS_ASSERT(FCheckPattern(pexpr));

	IMemoryPool *pmp = pxfctxt->Pmp();
	CExpression *pexprScalar = (*pexpr)[1]; // scalar subquery expression
	CExpression *pexprGbAgg = (*pexprScalar)[0];

	if (COperator::EopLogicalGbAgg == pexprGbAgg->Pop()->Eopid())
	{
		CExpression *pexprGbAggProjectList = (*pexprGbAgg)[1];
		if (0 == pexprGbAggProjectList->UlArity())
		{
			CExpression *pexprNewScalar = NULL;
			CExpression *pexprRelChild = (*pexprGbAgg)[0];
			pexprRelChild->AddRef();

			COperator *pop = pexprScalar->Pop();
			pop->AddRef();
			if (CUtils::FExistentialSubquery(pop))
			{
				// EXIST/NOT EXIST scalar subquery
				pexprNewScalar = GPOS_NEW(pmp) CExpression(pmp, pop, pexprRelChild);
			}
			else
			{
				// IN/NOT IN scalar subquery
				CExpression *pexprScalarIdent = (*pexprScalar)[1];
				pexprScalarIdent->AddRef();
				pexprNewScalar = GPOS_NEW(pmp) CExpression(pmp, pop, pexprRelChild, pexprScalarIdent);
			}

			pexpr->Pop()->AddRef(); // logical select operator
			(*pexpr)[0]->AddRef(); // relational child of logical select

			// new logical select expression
			CExpression *ppexprNew = GPOS_NEW(pmp) CExpression(pmp, pexpr->Pop(), (*pexpr)[0], pexprNewScalar);
			pxfres->Add(ppexprNew);
		}
	}
}

// EOF