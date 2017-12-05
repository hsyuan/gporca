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
	CExpression *pexprPattern
	)
	:
	CXformSimplifySubquery(pexprPattern)
{}

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

			CExpression *pexprGbChild = (*pexprGbAgg)[0];
			pexprGbChild->AddRef();

			CExpression *pexprScalarIdent = (*pexprScalar)[1];
			pexprScalarIdent->AddRef();

			*ppexprNewScalar = GPOS_NEW(pmp) CExpression(pmp, pop, pexprGbChild, pexprScalarIdent);
			return true;
		}
	}

	return false;
}

//BOOL
//CXformRemoveSubqDistinct::FSimplifyExistential
//	(
//	IMemoryPool *,//pmp,
//	CExpression *,//pexprScalar,
//	CExpression **//ppexprNewScalar
//	)
//{
//	return false;
//}

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
		{FTransformQuantified, CUtils::FQuantifiedSubquery}
	};
	*mapping = rgssm;
	size = GPOS_ARRAY_SIZE(rgssm);
}

CXform::EXformPromise
CXformRemoveSubqDistinct::Exfp
	(
	CExpressionHandle &exprhdl
	)
	const
{
	if (exprhdl.Pdpscalar(1)->FHasSubquery())
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
	GPOS_ASSERT(FCheckPattern(pexpr));

//	IMemoryPool *pmp = pxfctxt->Pmp();

	if (!CUtils::FQuantifiedSubquery(pexpr->Pop()))
	{
		return;
	}
}

// EOF
