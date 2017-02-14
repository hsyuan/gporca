//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal, Inc.
//
//	@filename:
//		CDXLScalarPartListValues.h
//
//	@doc:
//		Class for representing DXL Part List Values expressions
//		These expressions indicate the constant values for the list partition
//---------------------------------------------------------------------------

#ifndef GPDXL_CDXLScalarPartListValues_H
#define GPDXL_CDXLScalarPartListValues_H

#include "gpos/base.h"
#include "naucrates/dxl/operators/CDXLScalar.h"

namespace gpdxl
{

	//---------------------------------------------------------------------------
	//	@class:
	//		CDXLScalarPartListValues
	//
	//	@doc:
	//		Class for representing DXL Part List Values expressions
	//
	//---------------------------------------------------------------------------
	class CDXLScalarPartListValues : public CDXLScalar
	{
		private:

			// partitioning level
			ULONG m_ulLevel;

			// element type
			IMDId *m_pmdidType;

			// private copy ctor
			CDXLScalarPartListValues(const CDXLScalarPartListValues&);

		public:
			// ctor
			CDXLScalarPartListValues(IMemoryPool *pmp, ULONG ulLevel, IMDId *pmdidType);

			// dtor
			virtual
			~CDXLScalarPartListValues();

			// operator type
			virtual
			Edxlopid Edxlop() const;

			// operator name
			virtual
			const CWStringConst *PstrOpName() const;

			// partitioning level
			ULONG UlLevel() const
			{
				return m_ulLevel;
			}

			// element type
			IMDId *PmdidType() const
			{
				return m_pmdidType;
			}

			// serialize operator in DXL format
			virtual
			void SerializeToDXL(CXMLSerializer *pxmlser, const CDXLNode *pdxln) const;

			// does the operator return a boolean result
			virtual
			BOOL FBoolean
					(
					CMDAccessor * //pmda
					)
					const
			{
				return false;
			}

#ifdef GPOS_DEBUG
			// checks whether the operator has valid structure, i.e. number and
			// types of child nodes
			virtual
			void AssertValid(const CDXLNode *pdxln, BOOL fValidateChildren) const;
#endif // GPOS_DEBUG

			// conversion function
			static
			CDXLScalarPartListValues *PdxlopConvert
				(
				CDXLOperator *pdxlop
				)
			{
				GPOS_ASSERT(NULL != pdxlop);
				GPOS_ASSERT(EdxlopScalarPartListValues == pdxlop->Edxlop());

				return dynamic_cast<CDXLScalarPartListValues*>(pdxlop);
			}
	};
}

#endif // !GPDXL_CDXLScalarPartListValues_H

// EOF
