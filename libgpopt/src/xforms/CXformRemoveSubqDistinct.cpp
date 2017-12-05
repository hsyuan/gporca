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
	CXformSimplifySubquery
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

BOOL
CXformRemoveSubqDistinct::FTransformQuantified
	(
	IMemoryPool *pmp,
	CExpression *pexprScalar,
	CExpression **ppexprNewScalar
	)
{
	CExpression *pexprGbAgg = (*pexprScalar)[0];
	if (COperator::EopLogicalGbAgg == pexprGbAgg->Pop()->Eopid())
	{
		CExpression *pexprGbAggProjectList = (*pexprGbAgg)[1];
		if (0 == pexprGbAggProjectList->UlArity())
		{
			COperator *pop = pexprScalar->Pop();
			pop->AddRef();

			CExpression *pexprRelChild = (*pexprGbAgg)[0];
			pexprRelChild->AddRef();

			CExpression *pexprScalarIdent = (*pexprScalar)[1];
			pexprScalarIdent->AddRef();

			*ppexprNewScalar = GPOS_NEW(pmp) CExpression(pmp, pop, pexprRelChild, pexprScalarIdent);
			return true;
		}
	}

	return false;
}

BOOL
CXformRemoveSubqDistinct::FTransformExistential
	(
	IMemoryPool *pmp,
	CExpression *pexprScalar,
	CExpression **ppexprNewScalar
	)
{
	CExpression *pexprGbAgg = (*pexprScalar)[0];
	if (COperator::EopLogicalGbAgg == pexprGbAgg->Pop()->Eopid())
	{
		CExpression *pexprGbAggProjectList = (*pexprGbAgg)[1];
		if (0 == pexprGbAggProjectList->UlArity())
		{
			COperator *pop = pexprScalar->Pop();
			pop->AddRef();

			CExpression *pexprRelChild = (*pexprGbAgg)[0];
			pexprRelChild->AddRef();

			*ppexprNewScalar = GPOS_NEW(pmp) CExpression(pmp, pop, pexprRelChild);
			return true;
		}
	}

	return false;
}

void
CXformRemoveSubqDistinct::ssmaping
	(
	const SSimplifySubqueryMapping **mapping,
	ULONG &size
	)
	const
{
	// initialization of simplify function mappings
	const SSimplifySubqueryMapping rgssm[] =
	{
		{FTransformQuantified, CUtils::FQuantifiedSubquery},
		{FTransformExistential, CUtils::FExistentialSubquery}
	};
	*mapping = rgssm;
	size = GPOS_ARRAY_SIZE(rgssm);
}

// EOF
