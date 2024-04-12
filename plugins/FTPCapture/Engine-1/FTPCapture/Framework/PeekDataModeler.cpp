// =============================================================================
//	PeekDataModeler.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "Peek.h"
#include "PeekDataModeler.h"
#include "imemorystream.h"

namespace ConfigTags {
	const CPeekString	kEnabled( L"Enabled" );
	const CPeekString	kTrue( L"True" );
	const CPeekString	kFalse( L"False" );
}


// =============================================================================
//		CPeekXmlTransfer	[static]
// =============================================================================

bool
CPeekXmlTransfer::TestBool(
	HeLib::CHeVariant&	inValue,
	bool&				outValue )
{
	HeLib::CHeVariant	varBstr( inValue );
	if ( HE_SUCCEEDED( varBstr.ChangeType( Helium::HE_VT_BSTR ) ) ) {
		CHeBSTR	bstrValue( varBstr.bstrVal );
		if ( (bstrValue[0] == L'T') || (bstrValue[0] == L't') ) {
			outValue = true;
			return true;
		}
		if ( (bstrValue[0] == L'F') || (bstrValue[0] == L'f') ) {
			outValue = false;
			return true;
		}
	}

	HeLib::CHeVariant	varInteger( inValue );
	if ( HE_SUCCEEDED( varInteger.ChangeType( Helium::HE_VT_I4 ) ) ) {
		outValue = (varInteger.ulVal != 0);
		return true;
	}

	return false;
}


// =============================================================================
//		CPeekXmlLoad
// =============================================================================

// -----------------------------------------------------------------------------
//		Attribute
// -----------------------------------------------------------------------------

bool
CPeekXmlLoad::Attribute(
	PCWSTR	inName,
	bool&	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue ) ) return false;
	return TestBool( varValue, outAttribute );
}

bool
CPeekXmlLoad::Attribute(
	PCWSTR			inName,
	CPeekString&	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue, Helium::HE_VT_BSTR ) ) return false;
	outAttribute = CPeekString( varValue.bstrVal, HeLib::HeStringLen( varValue.bstrVal ) );
	return true;
}

bool
CPeekXmlLoad::Attribute(
	PCWSTR		inName,
	UInt32		inLength,
	wchar_t*	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue, Helium::HE_VT_BSTR ) ) return false;

	CHeBSTR	bstrValue( varValue.bstrVal );
	if ( bstrValue.Length() >= inLength ) return false;
	lstrcpyn( outAttribute, bstrValue, inLength );
	return true;
}

#ifdef PEEK_UI
bool
CPeekXmlLoad::Attribute(
	PCWSTR	inName,
	BSTR&	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue, Helium::HE_VT_BSTR ) ) return false;
	outAttribute = varValue.bstrVal;
	return true;
}
#endif

bool
CPeekXmlLoad::Attribute(
	PCWSTR	inName,
	GUID&	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue, Helium::HE_VT_BSTR ) ) return false;
	HeID* idAttribute( reinterpret_cast<HeID*>( &outAttribute) );
	if ( !idAttribute->Parse( varValue.bstrVal ) ) Peek::Throw( HE_E_FAIL );
	return true;
}

bool
CPeekXmlLoad::Attribute(
	PCWSTR	inName,
	double&	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue, Helium::HE_VT_R8 ) ) return false;
	outAttribute = varValue.dblVal;
	return true;
}

bool
CPeekXmlLoad::Attribute(
	PCWSTR	inName,
	UInt64&	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue, Helium::HE_VT_UI8 ) ) return false;
	outAttribute = varValue.ullVal;
	return true;
}

bool
CPeekXmlLoad::Attribute(
	PCWSTR	inName,
	SInt64&	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue, Helium::HE_VT_I8 ) ) return false;
	outAttribute = varValue.llVal;
	return true;
}

bool
CPeekXmlLoad::Attribute(
	PCWSTR	inName,
	UInt32&	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue, Helium::HE_VT_UI4 ) ) return false;
	outAttribute = varValue.ulVal;
	return true;
}

bool
CPeekXmlLoad::Attribute(
	PCWSTR	inName,
	SInt32&	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue, Helium::HE_VT_I4 ) ) return false;
	outAttribute = varValue.lVal;
	return true;
}

bool
CPeekXmlLoad::Attribute(
	PCWSTR			inName,
	unsigned int&	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue, Helium::HE_VT_UINT ) ) return false;
	outAttribute = varValue.uintVal;
	return true;
}

bool
CPeekXmlLoad::Attribute(
	PCWSTR	inName,
	int&	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue, Helium::HE_VT_INT ) ) return false;
	outAttribute = varValue.intVal;
	return true;
}

bool
CPeekXmlLoad::Attribute(
	PCWSTR	inName,
	UInt16&	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue, Helium::HE_VT_UI2 ) ) return false;
	outAttribute = varValue.uiVal;
	return true;
}

bool
CPeekXmlLoad::Attribute(
	PCWSTR	inName,
	SInt16&	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue, Helium::HE_VT_I2 ) ) return false;
	outAttribute = varValue.iVal;
	return true;
}

bool
CPeekXmlLoad::Attribute(
	PCWSTR	inName,
	UInt8&	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue, Helium::HE_VT_UI1 ) ) return false;
	outAttribute = varValue.bVal;
	return true;
}

bool
CPeekXmlLoad::Attribute(
	PCWSTR	inName,
	SInt8&	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue, Helium::HE_VT_I1 ) ) return false;
	outAttribute = varValue.cVal;
	return true;
}

bool
CPeekXmlLoad::Attribute(
	PCWSTR				inName,
	COmniModeledData&	outAttribute )
{
	HeLib::CHeVariant	varValue;
	if ( !GetAttribute( inName, varValue ) ) return false;
	outAttribute.ModeledDataSet( CPeekString( varValue.bstrVal, HeLib::HeStringLen( varValue.bstrVal ) ) );
	return true;
}

bool
CPeekXmlLoad::Attribute(
	PCWSTR				inName,
	size_t&				outAttribute,
	const CArrayString& inValues )
{
	CPeekString	strValue;
	if ( !GetAttribute( inName, strValue ) ) return false;

	return inValues.Find( strValue, outAttribute );
}


// -----------------------------------------------------------------------------
//		Value
// -----------------------------------------------------------------------------

bool
CPeekXmlLoad::Value(
	bool&	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue, Helium::HE_VT_BSTR ) ) return false;
	return TestBool( varValue, outValue );
}

bool
CPeekXmlLoad::Value(
	CPeekString&	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue, Helium::HE_VT_BSTR ) ) return false;
	outValue = CPeekString( varValue.bstrVal, static_cast<int>(HeLib::HeStringLen( varValue.bstrVal )) );
	return true;
}

bool
CPeekXmlLoad::Value(
	UInt32		inLength,
	wchar_t*	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue, Helium::HE_VT_BSTR ) ) return false;

	CHeBSTR	bstrValue( varValue.bstrVal );
	if ( bstrValue.Length() >= inLength ) return false;
	lstrcpyn( outValue, bstrValue, inLength );
	return true;
}

#ifdef PEEK_UI
bool
CPeekXmlLoad::Value(
	BSTR&	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue, Helium::HE_VT_BSTR ) ) return false;
	outValue = varValue.bstrVal;
	return true;
}
#endif

bool
CPeekXmlLoad::Value(
	GUID&	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue, Helium::HE_VT_BSTR ) ) return false;
	HeID* idAttribute( reinterpret_cast<HeID*>( &outValue) );
	if ( !idAttribute->Parse( varValue.bstrVal ) ) Peek::Throw( HE_E_FAIL );
	return true;
}

bool
CPeekXmlLoad::Value(
	double&	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue, Helium::HE_VT_R8 ) ) return false;
	outValue = varValue.dblVal;
	return true;
}

bool
CPeekXmlLoad::Value(
	UInt64&	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue, Helium::HE_VT_UI8 ) ) return false;
	outValue = varValue.ullVal;
	return true;
}

bool
CPeekXmlLoad::Value(
	SInt64&	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue, Helium::HE_VT_I8 ) ) return false;
	outValue = varValue.llVal;
	return true;
}

bool
CPeekXmlLoad::Value(
	UInt32&	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue, Helium::HE_VT_UI4 ) ) return false;
	outValue = varValue.ulVal;
	return true;
}

bool
CPeekXmlLoad::Value(
	SInt32&	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue, Helium::HE_VT_I4 ) ) return false;
	outValue = varValue.lVal;
	return true;
}

bool
CPeekXmlLoad::Value(
	unsigned int&	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue, Helium::HE_VT_UINT ) ) return false;
	outValue = varValue.uintVal;
	return true;
}

bool
CPeekXmlLoad::Value(
	int&	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue, Helium::HE_VT_INT ) ) return false;
	outValue = varValue.intVal;
	return true;
}

bool
CPeekXmlLoad::Value(
	UInt16&	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue, Helium::HE_VT_UI2 ) ) return false;
	outValue = varValue.uiVal;
	return true;
}

bool
CPeekXmlLoad::Value(
	SInt16&	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue, Helium::HE_VT_I2 ) ) return false;
	outValue = varValue.iVal;
	return true;
}

bool
CPeekXmlLoad::Value(
	UInt8&	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue, Helium::HE_VT_UI1 ) ) return false;
	outValue = varValue.bVal;
	return true;
}

bool
CPeekXmlLoad::Value(
	SInt8&	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue, Helium::HE_VT_I1 ) ) return false;
	outValue = varValue.cVal;
	return true;
}

bool
CPeekXmlLoad::Value(
	COmniModeledData&	outValue )
{
	HeLib::CHeVariant	varValue;
	if ( !GetValue( varValue ) ) return false;
	outValue.ModeledDataSet( CPeekString( varValue.bstrVal ) );
	return true;
}


// =============================================================================
//		CPeekXmlStore
// =============================================================================

// -----------------------------------------------------------------------------
//		Attribute
// -----------------------------------------------------------------------------

bool
CPeekXmlStore::Attribute(
	PCWSTR	inName,
	bool&	inAttribute )
{
	SetAttribute( inName, ((inAttribute) ? L"True" : L"False") );
	return true;
}

bool
CPeekXmlStore::Attribute(
	PCWSTR			inName,
	CPeekString&	inAttribute )
{
	SetAttribute( inName, CHeBSTR( inAttribute.GetLength(), inAttribute ) );
	return true;
}

bool
CPeekXmlStore::Attribute(
	PCWSTR		inName,
	UInt32		inLength,
	wchar_t*	inAttribute )
{
	SetAttribute( inName, CHeBSTR( inLength, inAttribute ) );
	return true;
}

#ifdef PEEK_UI
bool
CPeekXmlStore::Attribute(
	PCWSTR	inName,
	BSTR&	inAttribute )
{
	SetAttribute( inName, inAttribute );
	return true;
}
#endif

bool
CPeekXmlStore::Attribute(
	PCWSTR	inName,
	GUID&	inAttribute )
{
	HeID* HeGuidVal( reinterpret_cast<HeID*>( &inAttribute) );
	SetAttribute( inName, CHeBSTR( *HeGuidVal ) );
	return true;
}

bool
CPeekXmlStore::Attribute(
	PCWSTR	inName,
	double&	inAttribute )
{
	SetAttribute( inName, HeLib::CHeVariant( inAttribute ) );
	return true;
}

bool
CPeekXmlStore::Attribute(
	PCWSTR	inName,
	UInt64&	inAttribute )
{
	SetAttribute( inName, HeLib::CHeVariant( inAttribute ) );
	return true;
}

bool
CPeekXmlStore::Attribute(
	PCWSTR	inName,
	SInt64&	inAttribute )
{
	SetAttribute( inName, HeLib::CHeVariant( inAttribute ) );
	return true;
}

bool
CPeekXmlStore::Attribute(
	PCWSTR	inName,
	UInt32&	inAttribute )
{
	SetAttribute( inName, HeLib::CHeVariant( inAttribute ) );
	return true;
}

bool
CPeekXmlStore::Attribute(
	PCWSTR	inName,
	SInt32&	inAttribute )
{
	SetAttribute( inName, HeLib::CHeVariant( inAttribute ) );
	return true;
}

bool
CPeekXmlStore::Attribute(
	PCWSTR			inName,
	unsigned int&	inAttribute )
{
	SetAttribute( inName, HeLib::CHeVariant( inAttribute ) );
	return true;
}

bool
CPeekXmlStore::Attribute(
	PCWSTR	inName,
	int&	inAttribute )
{
	SetAttribute( inName, HeLib::CHeVariant( inAttribute ) );
	return true;
}

bool
CPeekXmlStore::Attribute(
	PCWSTR	inName,
	UInt16&	inAttribute )
{
	SetAttribute( inName, HeLib::CHeVariant( inAttribute ) );
	return true;
}

bool
CPeekXmlStore::Attribute(
	PCWSTR	inName,
	SInt16&	inAttribute )
{
	SetAttribute( inName, HeLib::CHeVariant( inAttribute ) );
	return true;
}

bool
CPeekXmlStore::Attribute(
	PCWSTR	inName,
	UInt8&	inAttribute )
{
	SetAttribute( inName, HeLib::CHeVariant( inAttribute ) );
	return true;
}

bool
CPeekXmlStore::Attribute(
	PCWSTR	inName,
	SInt8&	inAttribute )
{
	SetAttribute( inName, HeLib::CHeVariant( inAttribute ) );
	return true;
}

bool
CPeekXmlStore::Attribute(
	PCWSTR				inName,
	COmniModeledData&	inAttribute )
{
	CHeBSTR	bstrValue( inAttribute.ModeledDataGet() );
	SetAttribute( inName, bstrValue );
	return true;
}

bool
CPeekXmlStore::Attribute(
	PCWSTR				inName,
	size_t&				inAttribute,
	const CArrayString& inValues )
{
	CHeBSTR	bstrValue( inValues[inAttribute] );
	SetAttribute( inName, bstrValue );
	return true;
}


// -----------------------------------------------------------------------------
//		Value
// -----------------------------------------------------------------------------

bool
CPeekXmlStore::Value(
	bool&	inValue )
{
	CHeBSTR		bstrValue = (inValue != 0) ? ConfigTags::kTrue : ConfigTags::kFalse;
	HeLib::CHeVariant	varValue( bstrValue );
	SetValue( varValue );
	return true;
}

bool
CPeekXmlStore::Value(
	CPeekString&	inValue )
{
	SetValue( HeLib::CHeVariant( inValue ) );
	return true;
}

bool
CPeekXmlStore::Value(
	UInt32		inLength,
	wchar_t*	inValue )
{
	CHeBSTR		bstrValue( inLength, inValue );
	SetValue( HeLib::CHeVariant( bstrValue ) );
	return true;
}

#ifdef PEEK_UI
bool
CPeekXmlStore::Value(
	BSTR&	inValue )
{
	SetValue( HeLib::CHeVariant( inValue ) );
	return true;
}
#endif

bool
CPeekXmlStore::Value(
	GUID&	inValue )
{
	HeID* idAttribute( reinterpret_cast<HeID*>( &inValue) );
	wchar_t	szId[64];	// need 38.
	idAttribute->ToString( szId, 64 );
	SetValue( HeLib::CHeVariant( szId ) );
	return true;
}

bool
CPeekXmlStore::Value(
	double&	inValue )
{
	SetValue( HeLib::CHeVariant( inValue ) );
	return true;
}

bool
CPeekXmlStore::Value(
	UInt64&	inValue )
{
	SetValue( HeLib::CHeVariant( inValue ) );
	return true;
}

bool
CPeekXmlStore::Value(
	SInt64&	inValue )
{
	SetValue( HeLib::CHeVariant( inValue ) );
	return true;
}

bool
CPeekXmlStore::Value(
	UInt32&	inValue )
{
	SetValue( HeLib::CHeVariant( inValue ) );
	return true;
}

bool
CPeekXmlStore::Value(
	SInt32&	inValue )
{
	SetValue( HeLib::CHeVariant( inValue ) );
	return true;
}

bool
CPeekXmlStore::Value(
	unsigned int&	inValue )
{
	SetValue( HeLib::CHeVariant( inValue ) );
	return true;
}

bool
CPeekXmlStore::Value(
	int&	inValue )
{
	SetValue( HeLib::CHeVariant( inValue ) );
	return true;
}

bool
CPeekXmlStore::Value(
	UInt16&	inValue )
{
	SetValue( HeLib::CHeVariant( inValue ) );
	return true;
}

bool
CPeekXmlStore::Value(
	SInt16&	inValue )
{
	SetValue( HeLib::CHeVariant( inValue ) );
	return true;
}

bool
CPeekXmlStore::Value(
	UInt8&	inValue )
{
	SetValue( HeLib::CHeVariant( inValue ) );
	return true;
}

bool
CPeekXmlStore::Value(
	SInt8&	inValue )
{
	SetValue( HeLib::CHeVariant( inValue ) );
	return true;
}

bool
CPeekXmlStore::Value(
	COmniModeledData&	inValue )
{
	CPeekString	strValue = inValue.ModeledDataGet();
	SetValue( HeLib::CHeVariant( strValue ) );
	return true;
}


// =============================================================================
//		CPeekDataModeler
// =============================================================================

// =============================================================================
//		CPeekDataElement
// =============================================================================

CPeekDataElement::CPeekDataElement(
	PCWSTR				inName,
	CPeekDataModeler&	inModeler )
	:	m_Modeler( inModeler )
{
	CPeekXmlTransferPtr	sp( m_Modeler.Child( inName ) );
	m_spXml = sp;
}

CPeekDataElement::CPeekDataElement(
	PCWSTR				inName,
	CPeekDataElement&	inParent,
	size_t				inIndex /*= 0*/ )
	:	m_Modeler( inParent.m_Modeler )
{
	CPeekXmlTransferPtr	sp( inParent.Child( inName, inIndex ) );
	m_spXml = sp;
}
