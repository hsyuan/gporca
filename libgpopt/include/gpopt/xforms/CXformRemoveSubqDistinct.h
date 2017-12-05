//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal Software Inc.
//
//	@filename:
//		CXformRemoveSubqDistinct.h
//
//	@doc:
//		Transform that removes distinct clause from subquery
//---------------------------------------------------------------------------
#ifndef GPOPT_CXformRemoveSubqDistinct_H
#define GPOPT_CXformRemoveSubqDistinct_H

#include "gpos/base.h"
#include "gpopt/xforms/CXformSimplifySubquery.h"

namespace gpopt
{
	using namespace gpos;

	class CXformRemoveSubqDistinct : public CXformSimplifySubquery
	{

		private:

			static BOOL
			FTransformQuantified(IMemoryPool *pmp, CExpression *pexprScalar, CExpression **ppexprNewScalar);

			static BOOL
			FTransformExistential(IMemoryPool *pmp, CExpression *pexprScalar, CExpression **ppexprNewScalar);

			// private copy ctor
			CXformRemoveSubqDistinct(const CXformRemoveSubqDistinct &);

		public:

			// ctor
			explicit
			CXformRemoveSubqDistinct(IMemoryPool *pmp);

			// dtor
			virtual
			~CXformRemoveSubqDistinct()
			{}

			// ident accessors
			virtual
			EXformId Exfid() const
			{
				return ExfRemoveSubqDistinct;
			}

			// return a string for xform name
			virtual
			const CHAR *SzId() const
			{
				return "CXformRemoveSubqDistinct";
			}

			// compute xform promise for a given expression handle
			virtual
			EXformPromise Exfp (CExpressionHandle &exprhdl) const;

			virtual
			void ssmaping(const SSimplifySubqueryMapping **mapping, ULONG &size) const;

	}; // class CXformRemoveSubqDistinct

}

#endif // !GPOPT_CXformRemoveSubqDistinct_H

// EOF
