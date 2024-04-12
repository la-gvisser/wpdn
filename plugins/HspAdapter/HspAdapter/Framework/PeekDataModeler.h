// =============================================================================
//	PeekDataModeler.h
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "ixmldom.h"
#include "Peek.h"
#include "PeekStrings.h"
#include "PeekXml.h"
#include "PeekStream.h"
#include <memory>
#include <string>

class CPeekXmlTransfer;
typedef std::shared_ptr<CPeekXmlTransfer>	CPeekXmlTransferPtr;


// =============================================================================
//		Data Structures and Constants
// =============================================================================

typedef enum {
	kModeler_Store,
	kModeler_Load
} tModeler_Type;

namespace ConfigTags {
	extern const CPeekString	kEnabled;
}


// =============================================================================
//		CPeekXmlTransfer
// =============================================================================

class CPeekXmlTransfer
	:	public	CPeekXml
{
protected:
	static bool	TestBool( HeLib::CHeVariant& inValue, bool& outValue );

public:
	;		CPeekXmlTransfer() {}
	;		CPeekXmlTransfer( IHeUnknown* inXmlDoc, IHeUnknown* inXmlNode )
		:	CPeekXml( inXmlDoc, inXmlNode )
	{
	}
	;		CPeekXmlTransfer( const CPeekXml& inOther )
		:	CPeekXml( inOther )
	{
	}
	;		~CPeekXmlTransfer() { Close(); }

	virtual void	Attribute( const wchar_t* inName, HeLib::CHeVariant& ioAttribute ) = 0;
	virtual bool	Attribute( const wchar_t* inName, bool& outAttribute ) = 0;
	virtual bool	Attribute( const wchar_t* inName, CPeekString& outAttribute ) = 0;
	virtual bool	Attribute( const wchar_t* inName, std::wstring& outAttribute ) = 0;
	virtual bool	Attribute( const wchar_t* inName, UInt32 inLength, wchar_t* outAttribute ) = 0;
#ifdef PEEK_UI
	virtual bool	Attribute( const wchar_t* inName, BSTR& outAttribute ) = 0;
#endif // PEEK_UI
	virtual bool	Attribute( const wchar_t* inName, GUID& outAttribute ) = 0;
	virtual bool	Attribute( const wchar_t* inName, double& outAttribute ) = 0;
	virtual bool	Attribute( const wchar_t* inName, UInt64& outAttribute ) = 0;
	virtual bool	Attribute( const wchar_t* inName, SInt64& outAttribute ) = 0;
	virtual bool	Attribute( const wchar_t* inName, UInt32& outAttribute ) = 0;
	virtual bool	Attribute( const wchar_t* inName, SInt32& outAttribute ) = 0;
#ifdef _WIN32
	virtual bool	Attribute( const wchar_t* inName, unsigned int& outAttribute ) = 0;
	virtual bool	Attribute( const wchar_t* inName, int& outAttribute ) = 0;
#endif // _WIN32
	virtual bool	Attribute( const wchar_t* inName, UInt16& outAttribute ) = 0;
	virtual bool	Attribute( const wchar_t* inName, SInt16& outAttribute ) = 0;
	virtual bool	Attribute( const wchar_t* inName, UInt8& outAttribute ) = 0;
	virtual bool	Attribute( const wchar_t* inName, SInt8& outAttribute ) = 0;
	virtual bool	Attribute( const wchar_t* inName, COmniModeledData& ioValue ) = 0;
	virtual bool	Attribute( const wchar_t* inName, size_t& outAttribute, const CArrayString& inValues ) = 0;

	virtual CPeekXmlTransferPtr	Child( const wchar_t* inName, size_t inIndex = 0 ) = 0;
	virtual void				Close() {
		CPeekXml::Close();
	}

	virtual bool	IsLoading() const = 0;
	virtual bool	IsStoring() const = 0;

	virtual void	Value( HeLib::CHeVariant& ioValue ) = 0;
	virtual bool	Value( bool& ioValue ) = 0;
	virtual bool	Value( std::wstring& ioValue ) = 0;
	virtual bool	Value( CPeekString& ioValue ) = 0;
	virtual bool	Value( UInt32 inLength, wchar_t* ioValue ) = 0;
#ifdef PEEK_UI
	virtual bool	Value( BSTR& ioValue ) = 0;
#endif // PEEK_UI
	virtual bool	Value( GUID& ioValue ) = 0;
	virtual bool	Value( double& ioValue ) = 0;
	virtual bool	Value( UInt64& ioValue ) = 0;
	virtual bool	Value( SInt64& ioValue ) = 0;
	virtual bool	Value( UInt32& ioValue ) = 0;
	virtual bool	Value( SInt32& ioValue ) = 0;
#ifdef _WIN32
	virtual bool	Value( unsigned int& ioValue ) = 0;
	virtual bool	Value( int& ioValue ) = 0;
#endif // _WIN32
	virtual bool	Value( UInt16& ioValue ) = 0;
	virtual bool	Value( SInt16& ioValue ) = 0;
	virtual bool	Value( UInt8& ioValue ) = 0;
	virtual bool	Value( SInt8& ioValue ) = 0;
	virtual bool	Value( COmniModeledData& ioValue ) = 0;
};


// =============================================================================
//		CPeekXmlLoad
// =============================================================================

class CPeekXmlLoad
	:	public	CPeekXmlTransfer
{
protected:

public:
	;		CPeekXmlLoad( const wchar_t* inName ) { Create( inName ); }
	;		CPeekXmlLoad( const wchar_t* inFileName, const wchar_t* inRootName ) { Load( inFileName, inRootName ); }
	;		CPeekXmlLoad( IHeUnknown* inXmlDoc, IHeUnknown* inXmlNode )
		:	CPeekXmlTransfer( inXmlDoc, inXmlNode )
	{
	}
	;		CPeekXmlLoad( size_t inLength, const UInt8* inData ) {
		Load( inLength, inData );
	}
	;		CPeekXmlLoad( const CPeekXml& inOther )
		:	CPeekXmlTransfer( inOther )
	{
	}

	virtual void	Attribute( const wchar_t* inName, HeLib::CHeVariant& outAttribute ) {
		GetAttribute( inName, outAttribute );
	}
	virtual bool	Attribute( const wchar_t* inName, bool& outAttribute );
	virtual bool	Attribute( const wchar_t* inName, CPeekString& outAttribute );
	virtual bool	Attribute( const wchar_t* inName, std::wstring& outAttribute );
	virtual bool	Attribute( const wchar_t* inName, UInt32 inLength, wchar_t* outAttribute );
#ifdef PEEK_UI
	virtual bool	Attribute( const wchar_t* inName, BSTR& outAttribute );
#endif // PEEK_UI
	virtual bool	Attribute( const wchar_t* inName, GUID& outAttribute );
	virtual bool	Attribute( const wchar_t* inName, double& outAttribute );
	virtual bool	Attribute( const wchar_t* inName, UInt64& outAttribute );
	virtual bool	Attribute( const wchar_t* inName, SInt64& outAttribute );
	virtual bool	Attribute( const wchar_t* inName, UInt32& outAttribute );
	virtual bool	Attribute( const wchar_t* inName, SInt32& outAttribute );
#ifdef _WIN32
	virtual bool	Attribute( const wchar_t* inName, unsigned int&	outAttribute );
	virtual bool	Attribute( const wchar_t* inName, int& outAttribute );
#endif // _WIN32
	virtual bool	Attribute( const wchar_t* inName, UInt16& outAttribute );
	virtual bool	Attribute( const wchar_t* inName, SInt16& outAttribute );
	virtual bool	Attribute( const wchar_t* inName, UInt8& outAttribute );
	virtual bool	Attribute( const wchar_t* inName, SInt8& outAttribute );
	virtual bool	Attribute( const wchar_t* inName, COmniModeledData& outAttribute );
	virtual bool	Attribute( const wchar_t* inName, size_t& outAttribute, const CArrayString& inValues );

	virtual CPeekXmlTransferPtr	Child( const wchar_t* inName, size_t inIndex = 0 ) {
		CPeekXmlTransferPtr	sp( new CPeekXmlLoad( GetChild( inName, inIndex ) ) );
		return sp;
	}
	virtual void	Close() {
		CPeekXmlTransfer::Close();
	}

	virtual bool	IsLoading() const { return true; }
	virtual bool	IsStoring() const { return false; }

	virtual void	Value( HeLib::CHeVariant& outValue ) {
		GetValue( outValue );
	}
	virtual bool	Value( bool& outValue );
	virtual bool	Value( CPeekString& outValue );
	virtual bool	Value( std::wstring& ioValue );
	virtual bool	Value( UInt32 inLength, wchar_t* outValue );
#ifdef PEEK_UI
	virtual bool	Value( BSTR& outValue );
#endif // PEEK_UI
	virtual bool	Value( GUID& outValue );
	virtual bool	Value( double& outValue );
	virtual bool	Value( UInt64& outValue );
	virtual bool	Value( SInt64& outValue );
	virtual bool	Value( UInt32& outValue );
	virtual bool	Value( SInt32& outValue );
#ifdef _WIN32
	virtual bool	Value( unsigned int& outValue );
	virtual bool	Value( int& outValue );
#endif // _WIN32
	virtual bool	Value( UInt16& outValue );
	virtual bool	Value( SInt16& outValue );
	virtual bool	Value( UInt8& outValue );
	virtual bool	Value( SInt8& outValue );
	virtual bool	Value( COmniModeledData& outValue );
};


// =============================================================================
//		CPeekXmlStore
// =============================================================================

class CPeekXmlStore
	:	public	CPeekXmlTransfer
{
protected:

public:
	;		CPeekXmlStore( const wchar_t* inName, bool inAsync = false, bool inProcInstr = false ) { Create( inName, inAsync, inProcInstr ); }
	;		CPeekXmlStore( IHeUnknown* inXmlDoc, IHeUnknown* inXmlNode )
		:	CPeekXmlTransfer( inXmlDoc, inXmlNode )
	{
	}
	;		CPeekXmlStore( const CPeekXml& inOther )
		:	CPeekXmlTransfer( inOther )
	{
	}

	virtual void	Attribute( const wchar_t* inName, HeLib::CHeVariant& inAttribute ) {
		SetAttribute( inName, inAttribute );
	}
	virtual bool	Attribute( const wchar_t* inName, bool& inAttribute );
	virtual bool	Attribute( const wchar_t* inName, CPeekString& inAttribute );
	virtual bool	Attribute( const wchar_t* inName, std::wstring& inAttribute );
	virtual bool	Attribute( const wchar_t* inName, UInt32 inLength, wchar_t* inAttribute );
#ifdef PEEK_UI
	virtual bool	Attribute( const wchar_t* inName, BSTR& inAttribute );
#endif // PEEK_UI
	virtual bool	Attribute( const wchar_t* inName, GUID& inAttribute );
	virtual bool	Attribute( const wchar_t* inName, double& inAttribute );
	virtual bool	Attribute( const wchar_t* inName, UInt64& inAttribute );
	virtual bool	Attribute( const wchar_t* inName, SInt64& inAttribute );
	virtual bool	Attribute( const wchar_t* inName, UInt32& inAttribute );
	virtual bool	Attribute( const wchar_t* inName, SInt32& inAttribute );
#ifdef _WIN32
	virtual bool	Attribute( const wchar_t* inName, unsigned int&	inAttribute );
	virtual bool	Attribute( const wchar_t* inName, int& inAttribute );
#endif // _WIN32
	virtual bool	Attribute( const wchar_t* inName, UInt16& inAttribute );
	virtual bool	Attribute( const wchar_t* inName, SInt16& inAttribute );
	virtual bool	Attribute( const wchar_t* inName, UInt8& inAttribute );
	virtual bool	Attribute( const wchar_t* inName, SInt8& inAttribute );
	virtual bool	Attribute( const wchar_t* inName, COmniModeledData& inAttribute );
	virtual bool	Attribute( const wchar_t* inName, size_t& outAttribute, const CArrayString& inValues );

	virtual CPeekXmlTransferPtr	Child( const wchar_t* inName, size_t /*inIndex = 0*/ ) {
		CPeekXmlTransferPtr	sp( new CPeekXmlStore( CreateChild( inName ) ) );
		return sp;
	}
	virtual void	Close() {
		CPeekXmlTransfer::Close();
	}

	virtual bool	IsLoading() const { return false; }
	virtual bool	IsStoring() const { return true; }

	virtual void	Value( HeLib::CHeVariant& inValue ) {
		SetValue( inValue );
	}
	virtual bool	Value( bool& inValue );
	virtual bool	Value( CPeekString& inValue );
	virtual bool	Value( std::wstring& ioValue );
	virtual bool	Value( UInt32 inLength, wchar_t* inValue );
#ifdef PEEK_UI
	virtual bool	Value( BSTR& inValue );
#endif // PEEK_UI
	virtual bool	Value( GUID& inValue );
	virtual bool	Value( double& inValue );
	virtual bool	Value( UInt64& inValue );
	virtual bool	Value( SInt64& inValue );
	virtual bool	Value( UInt32& inValue );
	virtual bool	Value( SInt32& inValue );
#ifdef _WIN32
	virtual bool	Value( unsigned int& inValue );
	virtual bool	Value( int& inValue );
#endif // _WIN32
	virtual bool	Value( UInt16& inValue );
	virtual bool	Value( SInt16& inValue );
	virtual bool	Value( UInt8& inValue );
	virtual bool	Value( SInt8& inValue );
	virtual bool	Value( COmniModeledData& inValue );
};


// =============================================================================
//	CPeekDataModeler
//
//	Class for modeling data that will be sent from console to engine.
//	A model is constructed within a single function that can be called on both
//	the sending and receiving sides of a transaction, thus the marshaling and
//	un-marshaling of data is expedited.
// =============================================================================

class CPeekDataModeler
{
	friend class CPeekDataElement;

protected:
	CPeekXmlTransferPtr	m_spXml;

public:
	;		CPeekDataModeler( const wchar_t* inName, bool inAsync = false, bool inProcInstr = false  )
	{
		CPeekXmlTransferPtr	sp( new CPeekXmlStore( inName, inAsync, inProcInstr ) );
		m_spXml = sp;
	}
	;		CPeekDataModeler( const wchar_t* inName, const wchar_t* inRootName )
 	{
		CPeekXmlTransferPtr	sp( new CPeekXmlLoad( inName, inRootName ) );
		m_spXml = sp;
	}
	;		CPeekDataModeler( IHeUnknown* inXmlDoc, IHeUnknown* inXmlNode, tModeler_Type inType ) {
		if ( inType == kModeler_Load ) {
			CPeekXmlTransferPtr	sp( new CPeekXmlLoad( inXmlDoc, inXmlNode ) );
			m_spXml = sp;
		}
		else {
			CPeekXmlTransferPtr	sp( new CPeekXmlStore( inXmlDoc, inXmlNode ) );
			m_spXml = sp;
		}
	}
	;		CPeekDataModeler( size_t inLength, const UInt8* inData )
	{
		CPeekXmlTransferPtr	sp( new CPeekXmlLoad( inLength, inData ) );
		m_spXml = sp;
	}

	virtual	~CPeekDataModeler() {}

	operator bool() const { return IsValid(); }

	CPeekXmlTransferPtr	Child( const wchar_t* inName ) {
		CPeekXmlTransferPtr	sp;
		if ( m_spXml ) sp = m_spXml->Child( inName );
		return sp;
	}
	void		End() { if ( m_spXml ) m_spXml->Close(); }

	UInt32			GetChildCount( const wchar_t* inName = nullptr ) const {
		return (m_spXml) ? m_spXml->GetChildCount( inName ) : 0;
	}
	CPeekXmlTransferPtr	GetRoot() {
		CPeekXmlTransferPtr	sp;
		if ( m_spXml ) sp = m_spXml->Child( GetRootName() );
		return sp;
	}
	CPeekString		GetRootName() const {
		CPeekString	strRoot;
		if ( m_spXml ) {
			strRoot = m_spXml->GetName();
		}
		return strRoot;
	}
	tModeler_Type	GetType() const { return (IsStoring()) ? kModeler_Load : kModeler_Store; }
	CPeekXml		GetXml() const { return *m_spXml; }

	void	Load( UInt32 inLength, const UInt8* inData ) {
		CPeekXmlTransferPtr	sp( new CPeekXmlLoad( inLength, inData ) );
		m_spXml = sp;
	}

	bool	IsOpen() const { return (m_spXml) ? m_spXml->IsOpen() : false; }
	bool	IsLoading() const { return (m_spXml) ? m_spXml->IsLoading() : false; }
	bool	IsNotValid() const { return !IsOpen(); }
	bool	IsStoring() const { return (m_spXml) ? m_spXml->IsStoring() : false; }
	bool	IsValid() const { return IsOpen(); }

	// Attribute
	bool	Attribute( const wchar_t* inName, bool& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, CPeekString& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, std::wstring& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, UInt32 inLength, wchar_t* ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, inLength, ioAttribute ) : false;
	}
#ifdef PEEK_UI
	bool	Attribute( const wchar_t* inName, BSTR& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
#endif // PEEK_UI
	bool	Attribute( const wchar_t* inName, GUID& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, double& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, UInt64& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, SInt64& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, UInt32& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, SInt32& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
#ifdef _WIN32
	bool	Attribute( const wchar_t* inName, unsigned int& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, int& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
#endif // _WIN32
	bool	Attribute( const wchar_t* inName, UInt16& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, SInt16& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, UInt8& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, SInt8& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, COmniModeledData& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, size_t& ioAttribute, const CArrayString& inValues ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute, inValues ) : false;
	}

	bool	AttributeBOOL( const wchar_t* inName, BOOL& ioValue ) {
		bool	bEnabled = (ioValue != FALSE);
		if ( !Attribute( inName, bEnabled ) ) return false;
		ioValue = bEnabled;
		return true;
	}

	// Enable
	bool	Enabled( bool& ioEnabled ) {
		return ( Attribute( ConfigTags::kEnabled, ioEnabled ) );
	}
	bool	Enabled( BOOL& ioEnabled ) {
		bool	bEnabled = (ioEnabled != FALSE);
		if ( !Enabled( bEnabled ) ) return false;
		ioEnabled = bEnabled;
		return true;
	}

	void	Store( CPeekStream& outData ) {
		if ( m_spXml ) m_spXml->Save( outData.GetIStreamPtr() );
	}
};

typedef std::shared_ptr<CPeekDataModeler>	CPeekDataModelerPtr;


// =============================================================================
//		CPeekDataElement
// =============================================================================

class CPeekDataElement
{
protected:
	CPeekDataModeler&			m_Modeler;
	CPeekXmlTransferPtr			m_spXml;

public:
	;		CPeekDataElement( const wchar_t* inName, CPeekDataModeler& inModeler );
	;		CPeekDataElement( const wchar_t* inName, CPeekDataElement& inParent, size_t inIndex = 0 );
	;		CPeekDataElement( CPeekDataModeler& inModeler )
		:	m_Modeler( inModeler )
		,	m_spXml( inModeler.m_spXml )
	{}
	virtual	~CPeekDataElement() {
		End();
	}

	CPeekDataElement&	operator=( const CPeekDataElement& inOther ) {
		if ( inOther != *this ) {
			m_Modeler = inOther.m_Modeler;
			m_spXml = inOther.m_spXml;
		}
		return *this;
	}
	;					operator bool() const { return IsValid(); }

	CPeekXmlTransferPtr	Child( const wchar_t* inName, size_t inIndex = 0 ) {
		CPeekXmlTransferPtr	sp;
		if ( m_spXml ) sp = m_spXml->Child( inName, inIndex );
		return sp;
	}
	void	End() { m_spXml->Close(); }

	UInt32		GetChildCount( const wchar_t* inName = nullptr ) const {
		return (m_spXml) ? m_spXml->GetChildCount( inName ) : 0;
	}
	CPeekDataModeler	GetModeler() const { return m_Modeler; }
	CPeekXml			GetXml() const { return *m_spXml; }

	bool	IsLoading() const { return m_Modeler.IsLoading(); }
	bool	IsNotValid() const { return !m_spXml->IsOpen(); }
	bool	IsStoring() const { return m_Modeler.IsStoring(); }
	bool	IsOpen() const { return m_spXml->IsOpen(); }
	bool	IsValid() const { return m_spXml->IsOpen(); }

	// Attribute
	bool	Attribute( const wchar_t* inName, bool& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, CPeekString& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, std::wstring& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, UInt32 inLength, wchar_t* ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, inLength, ioAttribute ) : false;
	}
#ifdef PEEK_UI
	bool	Attribute( const wchar_t* inName, BSTR& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
#endif // PEEK_UI
	bool	Attribute( const wchar_t* inName, GUID& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, double& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, UInt64& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, SInt64& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, UInt32& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, SInt32& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
#ifdef _WIN32
	bool	Attribute( const wchar_t* inName, unsigned int& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, int& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
#endif // _WIN32
	bool	Attribute( const wchar_t* inName, UInt16& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, SInt16& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, UInt8& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, SInt8& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, COmniModeledData& ioAttribute ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute ) : false;
	}
	bool	Attribute( const wchar_t* inName, size_t& ioAttribute, const CArrayString& inValues ) {
		return (IsOpen()) ? m_spXml->Attribute( inName, ioAttribute, inValues ) : false;
	}

	bool	AttributeBOOL( const wchar_t* inName, BOOL& ioValue ) {
		bool	bEnabled = (ioValue != FALSE);
		if ( !Attribute( inName, bEnabled ) ) return false;
		ioValue = bEnabled;
		return true;
	}

	// Enable
	bool	Enabled( bool& ioEnabled ) {
		return ( Attribute( ConfigTags::kEnabled, ioEnabled ) );
	}
	bool	Enabled( BOOL& ioEnabled ) {
		bool	bEnabled = (ioEnabled != FALSE);
		if ( !Enabled( bEnabled ) ) return false;
		ioEnabled = bEnabled;
		return true;
	}

	// Child Value
	bool	ChildValue( const wchar_t* inName, bool& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}
	bool	ChildValue( const wchar_t* inName, CPeekString& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}
	bool	ChildValue( const wchar_t* inName, std::wstring& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}
	bool	ChildValue( const wchar_t* inName, UInt32 inLength, wchar_t* ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( inLength, ioValue );
			}
		}
		return false;
	}
#ifdef PEEK_UI
	bool	ChildValue( const wchar_t* inName, BSTR& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}
#endif // PEEK_UI
	bool	ChildValue( const wchar_t* inName, GUID& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}
	bool	ChildValue( const wchar_t* inName, double& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}
	bool	ChildValue( const wchar_t* inName, UInt64& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}
	bool	ChildValue( const wchar_t* inName, SInt64& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}
	bool	ChildValue( const wchar_t* inName, UInt32& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}
	bool	ChildValue( const wchar_t* inName, SInt32& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}
#ifdef _WIN32
	bool	ChildValue( const wchar_t* inName, unsigned int& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}
	bool	ChildValue( const wchar_t* inName, int& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}
#endif // _WIN32
	bool	ChildValue( const wchar_t* inName, UInt16& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}
	bool	ChildValue( const wchar_t* inName, SInt16& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}
	bool	ChildValue( const wchar_t* inName, UInt8& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}
	bool	ChildValue( const wchar_t* inName, SInt8& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}
	bool	ChildValue( const wchar_t* inName, COmniModeledData& ioValue, size_t inIndex = 0 ) {
		if ( IsOpen() ) {
			CPeekXmlTransferPtr	elemChild = m_spXml->Child( inName, inIndex );
			if ( elemChild ) {
				return elemChild->Value( ioValue );
			}
		}
		return false;
	}

	void	Store( CPeekStream& outData ) {
		if ( m_spXml ) m_spXml->Save( outData.GetIStreamPtr() );
	}

	// Value
	bool	Value( bool& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}
	bool	Value( CPeekString& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}
	bool	Value( std::wstring& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}
	bool	Value( UInt32 inLength, wchar_t* ioValue ) {
		return (IsOpen()) ? m_spXml->Value( inLength, ioValue ) : false;
	}
#ifdef PEEK_UI
	bool	Value( BSTR& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}
#endif // PEEK_UI
	bool	Value( GUID& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}
	bool	Value( double& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}
	bool	Value( UInt64& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}
	bool	Value( SInt64& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}
	bool	Value( UInt32& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}
	bool	Value( SInt32& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}
#ifdef _WIN32
	bool	Value( unsigned int& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}
	bool	Value( int& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}
#endif // _WIN32
	bool	Value( UInt16& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}
	bool	Value( SInt16& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}
	bool	Value( UInt8& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}
	bool	Value( SInt8& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}
	bool	Value( COmniModeledData& ioValue ) {
		return (IsOpen()) ? m_spXml->Value( ioValue ) : false;
	}

	bool	ValueBOOL( BOOL& ioValue ) {
		bool	bEnabled = (ioValue != FALSE);
		if ( !Value( bEnabled ) ) return false;
		ioValue = bEnabled;
		return true;
	}
};
