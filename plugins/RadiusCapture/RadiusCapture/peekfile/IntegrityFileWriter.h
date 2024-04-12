// ============================================================================
//	IntegrityFileWriter.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2005. All rights reserved.

// OPT_INTEGRITY - this file is part of the Integrity option

#pragma once

#include "PeekTaggedFile.h"
#include <atlcrypt.h>

class XFile;

#define kIntegrityFile_Root			_T( "Integrity" )
#define kIntegrityFile_Hash			_T( "Hash" )
#define kIntegrityFile_Signature	_T( "Signature" )
#define kIntegrityFile_FileName		_T( "FileName" )
#define kIntegrityFile_Algorithm	_T( "Algorithm" )
#define kIntegrityFile_PublicKey	_T( "PublicKey" )
#define kIntegrityFile_MD5			_T( "MD5" )
#define kIntegrityFile_SHA			_T( "SHA" )
#define kIntegrityFile_RSA			_T( "RSA" )
#define kIntegrityFile_Label		_T( "Label" )
#define kIntegrityFile_Value		_T( "Value" )
#define kIntegrityFile_Exponent		_T( "Exponent" )
#define kIntegrityFile_Modulus		_T( "Modulus" )

// Integrity constants - OPT_INTEGRITY
#define kIntegrity_Disabled				0
#define kIntegrity_Hash					1
#define kIntegrity_Sign					2
#define kIntegrity_Algorithm_MD5		1
#define kIntegrity_Algorithm_SHA		2
#define kIntegrity_Algorithm_RSA		3
#define kIntegrity_Undefined			-1

typedef struct {
	int			nEnabled;
	int			nHashAlgorithm;
	CPeekString	strKeyContainer;
} tIntegrityOptions;

class CIntegrityFileWriter
	:	public CCaptureFileWriter
{
protected:
	UINT				m_nErrorId;
	int					m_nEnabled;
	CPeekString			m_strKeyContainer;
	CCryptProv			m_Prov;
	CCryptUserSigKey	m_SignatureKey;
	CPeekString			m_strPublicExponent;
	CPeekString			m_strPublicModulus;
	int					m_nHashAlgorithm;

	CComPtr<IXMLDOMDocument>	m_spXMLDoc;
	CComPtr<IXMLDOMElement>		m_spSigValue;
	CComPtr<IXMLDOMElement>		m_spHashValue;
	CComPtr<IXMLDOMElement>		m_spFileName;

	void			ByteArrayToString( const CByteArray& cbData, CPeekString& strValue );
	void			CreateXmlDoc();
	CCryptHash*		HashFile( CPeekString strFilePath );
	void			HashOpenedFile( XFile& theFile, CCryptHash* pHash );
	CCryptHash*		InitHash();
	void			InitIntegrity();
	bool			GetHashAlgorithm( CPeekString& strHashAlgorithm );
	CPeekString			GetHashValue( CCryptHash* pHash );
	bool			GetPublicKey( CCryptUserSigKey SignatureKey );
	CPeekString			GetSignature( CCryptHash* pHash );
	void			PrepareToWrite( CPeekString& strFileName, CStreamOnCByteStream& sbsXMLStream, CCryptHash* pHash );
	bool			ReadPrefs();

public:
//	;				CIntegrityFileWriter( tIntegrityOptions* inOptions = NULL, LPCTSTR lpszFile = NULL );
	;				CIntegrityFileWriter() {}

	virtual bool	Close() { return CCaptureFileWriter::Close(); }
	virtual bool	Delete();
	BOOL			DeleteIntegrityFile( const CPeekString& strFilePath );
	UINT			GetErrorId( bool bClear );
	static bool		IsEnabled();
	virtual bool	Open( LPCTSTR lpszFile, bool bCompressed = false );
	virtual bool	WriteBytes( const void* pData, UInt32 nLength );
	BOOL			WriteIntegrityFile( CPeekString strFilePath );
};
