//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal, Inc.
//
//	@filename:
//		CDXLScalarPartListValues.cpp
//
//	@doc:
//		Implementation of DXL Part List Values expression
//---------------------------------------------------------------------------

#include "naucrates/dxl/operators/CDXLScalarPartListValues.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

#include "gpopt/mdcache/CMDAccessor.h"

using namespace gpopt;
using namespace gpmd;
using namespace gpos;
using namespace gpdxl;

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartListValues::CDXLScalarPartListValues
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
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


//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartListValues::~CDXLScalarPartListValues
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CDXLScalarPartListValues::~CDXLScalarPartListValues()
{
	m_pmdidType->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartListValues::Edxlop
//
//	@doc:
//		Operator type
//
//---------------------------------------------------------------------------
Edxlopid
CDXLScalarPartListValues::Edxlop() const
{
	return EdxlopScalarPartListValues;
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartListValues::PstrOpName
//
//	@doc:
//		Operator name
//
//---------------------------------------------------------------------------
const CWStringConst *
CDXLScalarPartListValues::PstrOpName() const
{
	return CDXLTokens::PstrToken(EdxltokenScalarPartListValues);
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartListValues::SerializeToDXL
//
//	@doc:
//		Serialize operator in DXL format
//
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartListValues::AssertValid
//
//	@doc:
//		Checks whether operator node is well-structured
//
//---------------------------------------------------------------------------
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

// EOF
