//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal, Inc.
//
//	@filename:
//		CDXLScalarPartListNullTest.cpp
//
//	@doc:
//		Implementation of DXL Part list null test expression
//---------------------------------------------------------------------------

#include "naucrates/dxl/operators/CDXLScalarPartListNullTest.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"

#include "gpopt/mdcache/CMDAccessor.h"

using namespace gpopt;
using namespace gpmd;
using namespace gpos;
using namespace gpdxl;

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartListNullTest::CDXLScalarPartListNullTest
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDXLScalarPartListNullTest::CDXLScalarPartListNullTest
	(
	IMemoryPool *pmp,
	ULONG ulLevel,
	BOOL fIsNull
	)
	:
	CDXLScalar(pmp),
	m_ulLevel(ulLevel),
	m_fIsNull(fIsNull)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartListNullTest::Edxlop
//
//	@doc:
//		Operator type
//
//---------------------------------------------------------------------------
Edxlopid
CDXLScalarPartListNullTest::Edxlop() const
{
	return EdxlopScalarPartListNullTest;
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartListNullTest::PstrOpName
//
//	@doc:
//		Operator name
//
//---------------------------------------------------------------------------
const CWStringConst *
CDXLScalarPartListNullTest::PstrOpName() const
{
	return CDXLTokens::PstrToken(EdxltokenScalarPartListNullTest);
}

//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartListNullTest::SerializeToDXL
//
//	@doc:
//		Serialize operator in DXL format
//
//---------------------------------------------------------------------------
void
CDXLScalarPartListNullTest::SerializeToDXL
	(
	CXMLSerializer *pxmlser,
	const CDXLNode * // pdxln
	)
	const
{
	const CWStringConst *pstrElemName = PstrOpName();

	pxmlser->OpenElement(CDXLTokens::PstrToken(EdxltokenNamespacePrefix), pstrElemName);
	pxmlser->AddAttribute(CDXLTokens::PstrToken(EdxltokenPartLevel), m_ulLevel);
	pxmlser->AddAttribute(CDXLTokens::PstrToken(EdxltokenScalarIsNull), m_fIsNull);
	pxmlser->CloseElement(CDXLTokens::PstrToken(EdxltokenNamespacePrefix), pstrElemName);
}

#ifdef GPOS_DEBUG
//---------------------------------------------------------------------------
//	@function:
//		CDXLScalarPartListNullTest::AssertValid
//
//	@doc:
//		Checks whether operator node is well-structured
//
//---------------------------------------------------------------------------
void
CDXLScalarPartListNullTest::AssertValid
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
