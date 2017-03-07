//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal, Inc.
//
//	Implementation of DXL Part List Values expression
//---------------------------------------------------------------------------

#include "naucrates/dxl/operators/CDXLScalarPartListValues.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

#include "gpopt/mdcache/CMDAccessor.h"

using namespace gpopt;
using namespace gpmd;
using namespace gpos;
using namespace gpdxl;

// Ctor
CDXLScalarPartListValues::CDXLScalarPartListValues
	(
	IMemoryPool *pmp,
	ULONG ulLevel,
	IMDId *pmdidType
	)
	:
	CDXLScalar(pmp),
	m_ulLevel(ulLevel),
	m_pmdidType(pmdidType)
{
	GPOS_ASSERT(pmdidType->FValid());
}


// Dtor
CDXLScalarPartListValues::~CDXLScalarPartListValues()
{
	m_pmdidType->Release();
}

// Operator type
Edxlopid
CDXLScalarPartListValues::Edxlop() const
{
	return EdxlopScalarPartListValues;
}

// Operator name
const CWStringConst *
CDXLScalarPartListValues::PstrOpName() const
{
	return CDXLTokens::PstrToken(EdxltokenScalarPartListValues);
}

// partitioning level
ULONG
CDXLScalarPartListValues::UlLevel() const
{
	return m_ulLevel;
}

// element type
IMDId *
CDXLScalarPartListValues::PmdidType() const
{
	return m_pmdidType;
}

// does the operator return a boolean result
BOOL
CDXLScalarPartListValues::FBoolean
	(
	CMDAccessor * //pmda
	)
	const
{
	return false;
}

// Serialize operator in DXL format
void
CDXLScalarPartListValues::SerializeToDXL
	(
	CXMLSerializer *pxmlser,
	const CDXLNode * // pdxln
	)
	const
{
	const CWStringConst *pstrElemName = PstrOpName();

	pxmlser->OpenElement(CDXLTokens::PstrToken(EdxltokenNamespacePrefix), pstrElemName);
	pxmlser->AddAttribute(CDXLTokens::PstrToken(EdxltokenPartLevel), m_ulLevel);
	m_pmdidType->Serialize(pxmlser, CDXLTokens::PstrToken(EdxltokenMDType));
	pxmlser->CloseElement(CDXLTokens::PstrToken(EdxltokenNamespacePrefix), pstrElemName);
}

#ifdef GPOS_DEBUG
// Checks whether operator node is well-structured
void
CDXLScalarPartListValues::AssertValid
	(
	const CDXLNode *pdxln,
	BOOL // fValidateChildren
	)
	const
{
	GPOS_ASSERT(0 == pdxln->UlArity());
}
#endif // GPOS_DEBUG

// conversion function
CDXLScalarPartListValues *
CDXLScalarPartListValues::PdxlopConvert
	(
	CDXLOperator *pdxlop
	)
{
	GPOS_ASSERT(NULL != pdxlop);
	GPOS_ASSERT(EdxlopScalarPartListValues == pdxlop->Edxlop());

	return dynamic_cast<CDXLScalarPartListValues*>(pdxlop);
}

// EOF
