// ============================================================================
//	IntegrityFileWriter.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2005. All rights reserved.

// OPT_INTEGRITY - this file is part of the Integrity option

#include "stdafx.h"
#include "IntegrityFileWriter.h"
#include "XmlUtil.h"
#include "CatchHR.h"
#include "XFile.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

#pragma warning(disable:4200)	// Suppress warning: Nonstandard extension used : zero-sized array...


// ============================================================================
//		CIntegrityFileWriter
// ============================================================================
//
//CIntegrityFileWriter::CIntegrityFileWriter(
//	 tIntegrityOptions*	inOptions,		/* = NULL */
//	 LPCTSTR			lpszFile )		/* = NULL */
//	:	CCaptureFileWriter( lpszFile )
//	,	m_nErrorId( 0 )
//	,	m_nEnabled( kIntegrity_Disabled )
//	,	m_nHashAlgorithm( kIntegrity_Algorithm_MD5 )
//{
//	if ( inOptions != NULL ) {
//		m_nEnabled = inOptions->nEnabled;
//		m_nHashAlgorithm = inOptions->nHashAlgorithm;
//		m_strKeyContainer = inOptions->strKeyContainer;
//
//		InitIntegrity();
//		CreateXmlDoc();
//	}
//}


// ----------------------------------------------------------------------------
//		HashOpenedFile
// ----------------------------------------------------------------------------

void
CIntegrityFileWriter::HashOpenedFile(
	XFile&		theFile,
	CCryptHash*	pHash )
{
	if ( pHash == NULL ) return;

	HRESULT			hResult;
	BOOL			bResult;
	const UInt64	nBlockSize = 64*1024;
	CByteArray		baData;
	UInt64			nBytesRemaining = theFile.GetSize();

	baData.SetSize( nBlockSize );

	while ( nBytesRemaining > nBlockSize ) {
		bResult = theFile.Read( baData.GetData(), (UInt32)nBlockSize );
		if ( bResult ) {
			hResult = pHash->AddData( baData.GetData(), (UInt32)nBlockSize );
			if ( FAILED( hResult ) ) {
				throw;
			}
			nBytesRemaining -= nBlockSize;
		}
		else {
			throw;
		}
	}
	if ( nBytesRemaining ) {
		bResult = theFile.Read( baData.GetData(), (UInt32)nBytesRemaining );
		if ( bResult ) {
			hResult = pHash->AddData( baData.GetData(), (UInt32)nBytesRemaining );
			if ( FAILED( hResult ) ) {
				throw;
			}
		}
	}
}


// ----------------------------------------------------------------------------
//		CreateXmlDoc
// ----------------------------------------------------------------------------

void
CIntegrityFileWriter::CreateXmlDoc()
{
	if ( m_spXMLDoc != NULL ) return;

	HRESULT	hr = S_OK;

	try {
		CPeekString	strHashAlgorithm;
		GetHashAlgorithm( strHashAlgorithm );

		// create the doc
		hr = XmlUtil::CreateXmlDocument( &m_spXMLDoc );
		if ( FAILED(hr) ) AtlThrow( hr );

		// root element
		CComPtr<IXMLDOMElement>	spRoot;
		XmlUtil::AddChildElement( m_spXMLDoc, m_spXMLDoc,
			kIntegrityFile_Root, &spRoot );

		if ( m_nEnabled == kIntegrity_Hash ) {
			// Hash
			CComPtr<IXMLDOMElement>	spHash;
			XmlUtil::AddChildElement( m_spXMLDoc, spRoot,
				kIntegrityFile_Hash, &spHash );

			// Hash algorithm
			XmlUtil::AddChildElement( m_spXMLDoc, spHash,
				kIntegrityFile_Algorithm, strHashAlgorithm );

			// Hash value
			XmlUtil::AddChildElement( m_spXMLDoc, spHash,
				kIntegrityFile_Value, &m_spHashValue );
		}
		else if ( m_nEnabled == kIntegrity_Sign ) {
			// Signature
			CComPtr<IXMLDOMElement>	spSignature;
			XmlUtil::AddChildElement( m_spXMLDoc, spRoot,
				kIntegrityFile_Signature, &spSignature );

			// PublicKey
			CComPtr<IXMLDOMElement>	spPublicKey;
			XmlUtil::AddChildElement( m_spXMLDoc, spSignature,
				kIntegrityFile_PublicKey, &spPublicKey );

			// PublicKey Algorithm
			XmlUtil::AddChildElement( m_spXMLDoc, spPublicKey,
				kIntegrityFile_Algorithm, kIntegrityFile_RSA );

			// Crypto API Key Container label.
			XmlUtil::AddChildElement( m_spXMLDoc, spPublicKey,
				kIntegrityFile_Label, m_strKeyContainer );

			// The PublicKey
			CComPtr<IXMLDOMElement>	spPKValue;
			XmlUtil::AddChildElement( m_spXMLDoc, spPublicKey,
				kIntegrityFile_Value, &spPKValue );
			XmlUtil::AddChildElement( m_spXMLDoc, spPKValue,
				kIntegrityFile_Exponent, m_strPublicExponent );
			XmlUtil::AddChildElement( m_spXMLDoc, spPKValue,
				kIntegrityFile_Modulus, m_strPublicModulus );

			// Hash algorithm
			XmlUtil::AddChildElement( m_spXMLDoc, spSignature,
				kIntegrityFile_Algorithm, strHashAlgorithm );

			// Signature value
			XmlUtil::AddChildElement( m_spXMLDoc, spSignature,
				kIntegrityFile_Value, &m_spSigValue );
		}

		// File Name
		XmlUtil::AddChildElement( m_spXMLDoc, spRoot,
			kIntegrityFile_FileName, &m_spFileName );
	}
	CATCH_HR(hr)
}


// ----------------------------------------------------------------------------
//		Delete
// ----------------------------------------------------------------------------

bool
CIntegrityFileWriter::Delete()
{
	return CCaptureFileWriter::Delete();
}


// ----------------------------------------------------------------------------
//		DeleteIntegrityFile
// ----------------------------------------------------------------------------

BOOL
CIntegrityFileWriter::DeleteIntegrityFile(
	const CPeekString&	strFilePath )
{
	if ( m_nEnabled ) {
		BOOL	bResult;
		CPeekString	strIntegrityFilePath( strFilePath + _T(".integrity") );
		bResult = ::DeleteFile( strIntegrityFilePath );

		return bResult;
	}
	return false;
}


// ----------------------------------------------------------------------------
//		GetErrorId
// ----------------------------------------------------------------------------

UINT
CIntegrityFileWriter::GetErrorId(
	bool	bClear )
{
	UINT	nErrorId = m_nErrorId;
	if ( bClear ) {
		m_nErrorId = 0;
	}
	return nErrorId;
}


// ----------------------------------------------------------------------------
//		GetHashAlgorithm
// ----------------------------------------------------------------------------

bool
CIntegrityFileWriter::GetHashAlgorithm(
	CPeekString& strHashAlgorithm )
{
	switch ( m_nHashAlgorithm ) {
	case kIntegrity_Algorithm_MD5:
		strHashAlgorithm = kIntegrityFile_MD5;
		break;

	case kIntegrity_Algorithm_SHA:
		strHashAlgorithm = kIntegrityFile_SHA;
		break;

	default:
		return false;
	}
	return true;
}


// ----------------------------------------------------------------------------
//		GetHashValue
// ----------------------------------------------------------------------------

CPeekString
CIntegrityFileWriter::GetHashValue(
	CCryptHash* pHash )
{
	CPeekString		strHashValue;
	if ( pHash == NULL ) return strHashValue;

	HRESULT		hr;
	DWORD		dwError;
	DWORD		dwHashSize = 0;
	CByteArray	bHashValue;

	hr = pHash->GetSize( &dwHashSize );
	if ( FAILED( hr ) ) {
		dwError = ::GetLastError();
		return strHashValue;
	}

	bHashValue.SetSize( dwHashSize );
	hr = pHash->GetValue( bHashValue.GetData(), &dwHashSize );
	if ( FAILED( hr ) ) {
		dwError = ::GetLastError();
		return strHashValue;
	}

	ByteArrayToString( bHashValue, strHashValue );
	return strHashValue;
}


// ----------------------------------------------------------------------------
//		GetPublicKey
// ----------------------------------------------------------------------------

bool
CIntegrityFileWriter::GetPublicKey(
	CCryptUserSigKey	SignatureKey )
{
	HRESULT		hr;
	DWORD		dwError;
	CCryptKey	keyExport;
	CByteArray	bPublicKey;
	DWORD		dwPublicKeySize;

	hr = SignatureKey.ExportPublicKeyBlob( keyExport, 0, NULL, &dwPublicKeySize );
	if ( FAILED( hr ) ) {
		dwError = ::GetLastError();
		return false;
	}
	bPublicKey.SetSize( dwPublicKeySize );
	hr = SignatureKey.ExportPublicKeyBlob( keyExport, 0, bPublicKey.GetData(), &dwPublicKeySize );
	if ( FAILED( hr ) ) {
		dwError = ::GetLastError();
		return false;
	}

	struct RSAPublicKey {
		PUBLICKEYSTRUC	Header;
		RSAPUBKEY		PublicKey;
		BYTE			Modulus[];
	} *pPublicKey;
	size_t	cbModulus;

	pPublicKey = (RSAPublicKey*) bPublicKey.GetData();

	// Perform some sanity checks
	if ( pPublicKey->Header.bType != PUBLICKEYBLOB ) return false;
	if ( pPublicKey->Header.bVersion != CUR_BLOB_VERSION ) return false;
	if ( pPublicKey->Header.aiKeyAlg != CALG_RSA_SIGN ) return false;
	if ( pPublicKey->PublicKey.magic != '1ASR' ) return false;	// RSA1

	wchar_t wszTemp[20];

	wsprintf( wszTemp, L"%0X", pPublicKey->PublicKey.pubexp );
	m_strPublicExponent = (CPeekString)wszTemp;

	cbModulus = pPublicKey->PublicKey.bitlen / 8;
	for ( size_t i = 0; i < cbModulus; i++ ) {
		wsprintf( wszTemp, L"%02X", pPublicKey->Modulus[i] );
		CPeekString strPublicModulus = m_strPublicModulus + wszTemp;
	}

	return true;
}


// ----------------------------------------------------------------------------
//		GetSignature
// ----------------------------------------------------------------------------

CPeekString
CIntegrityFileWriter::GetSignature(
	CCryptHash*	pHash )
{
	CPeekString		strSignature;
	if ( pHash == NULL ) return strSignature;

	HRESULT		hr;
	DWORD		dwError;
	DWORD		dwSignatureSize = 0;
	CByteArray	bSignature;

	hr = pHash->Sign( NULL, &dwSignatureSize );
	if ( FAILED( hr ) ) {
		dwError = ::GetLastError();
		return strSignature;
	}

	bSignature.SetSize( dwSignatureSize );
	hr = pHash->Sign( bSignature.GetData(), &dwSignatureSize );
	if ( FAILED( hr ) ) {
		dwError = ::GetLastError();
		return strSignature;
	}

	ByteArrayToString( bSignature, strSignature );
	return strSignature;
}


// ----------------------------------------------------------------------------
//		InitHash
// ----------------------------------------------------------------------------

CCryptHash*
CIntegrityFileWriter::InitHash()
{
	HRESULT	hr;
	DWORD	dwError;

	switch ( m_nHashAlgorithm ) {
	case kIntegrity_Algorithm_MD5:
		{
			CCryptMD5Hash*	pMD5Hash = new CCryptMD5Hash();
			if ( pMD5Hash != NULL ) {
				hr = pMD5Hash->Initialize( m_Prov );
				if ( SUCCEEDED( hr ) ) {
					return pMD5Hash;
				}
				else {
					dwError = ::GetLastError();
				}
			}
			else {
				dwError = ::GetLastError();
			}
		}
		break;

	case kIntegrity_Algorithm_SHA:
		{
			CCryptSHAHash*	pShaHash = new CCryptSHAHash();
			if ( pShaHash ) {
				hr = pShaHash->Initialize( m_Prov );
				if ( SUCCEEDED( hr ) ) {
					return pShaHash;
				}
				else {
					dwError = ::GetLastError();
				}
			}
			else {
				dwError = ::GetLastError();
			}
		}
		break;
	}
	return NULL;
}


// ----------------------------------------------------------------------------
//		InitIntegrity
// ----------------------------------------------------------------------------

void
CIntegrityFileWriter::InitIntegrity()
{
	DWORD	dwError = 0;
	HRESULT	hr;

	if ( m_nEnabled == kIntegrity_Hash ) {
		hr = m_Prov.Initialize( PROV_RSA_FULL, 0, MS_DEF_PROV, CRYPT_VERIFYCONTEXT );
		if ( FAILED( hr ) ) {
			dwError = ::GetLastError();
			m_nEnabled = kIntegrity_Disabled;
		}
		return;
	}

	if ( m_nEnabled == kIntegrity_Sign ) {
		hr = m_Prov.Initialize( PROV_RSA_FULL, m_strKeyContainer, MS_DEF_PROV, CRYPT_SILENT );
		if ( FAILED( hr ) ) {
			dwError = ::GetLastError();
			if ( dwError != NTE_BAD_KEYSET ) {
				// Container exists but is not usable.
				m_nEnabled = kIntegrity_Disabled;
				return;
			}

			// Create the container. Will still need to generate the keys.
			hr = m_Prov.Initialize(
				PROV_RSA_FULL, m_strKeyContainer, MS_DEF_PROV, CRYPT_NEWKEYSET );
			if ( FAILED( hr ) ) {
				// Can not create the Container, disable Integrity processing.
				dwError = ::GetLastError();
				m_nEnabled = kIntegrity_Disabled;
				return;
			}
		}

		// Verify that there is a signature key.
		CCryptUserSigKey	SignatureKey;
		hr = SignatureKey.Initialize( m_Prov );
		if ( FAILED( hr ) ) {
			dwError = ::GetLastError();
			if ( dwError != NTE_NO_KEY ) {
				// An error besides No Key, release and disable.
				m_Prov.Uninitialize();
				m_nEnabled = kIntegrity_Disabled;
				return;
			}
			
			// TBD: Set the key size.

			// Need to generate the key.
			hr = SignatureKey.Create( m_Prov );
			if ( FAILED( hr ) ) {
				// Failed to generate the key, release and disable.
				dwError = ::GetLastError();
				m_Prov.Uninitialize();
				m_nEnabled = kIntegrity_Disabled;
				return;
			}
		}
		bool bResult = GetPublicKey( SignatureKey );
		if ( !bResult ) {
			m_Prov.Uninitialize();
			m_nEnabled = kIntegrity_Disabled;
			return;
		}
	}
}


// ----------------------------------------------------------------------------
//		ByteArrayToString
// ----------------------------------------------------------------------------

void
CIntegrityFileWriter::ByteArrayToString(
	const CByteArray&	cbData,
	CPeekString&		strValue )
{
	INT_PTR	nSize = cbData.GetSize();

	for ( INT_PTR i = 0; i < nSize; i++ ) {
		wchar_t wszTemp[20];
		UInt32	nSize( sizeof(cbData[i]) );

		wsprintf( wszTemp, L"%02X", cbData[i], nSize );
		strValue = strValue + wszTemp;
	}
}


// ----------------------------------------------------------------------------
//		HashFile
// ----------------------------------------------------------------------------

CCryptHash*
CIntegrityFileWriter::HashFile(
	CPeekString	strFilePath )
{
	XFile	theFile( strFilePath );

	bool	bResult = theFile.Open( kReadPerm, kFile_OpenExisting );
	if ( !bResult ) return NULL;
	if ( !theFile.IsOpen() ) return NULL;

	CCryptHash*	pHash = NULL;
	try {
		pHash = InitHash();
		if ( pHash != NULL ) {
			HashOpenedFile( theFile, pHash );
		}
	}
	catch ( ... ) {
		pHash = NULL;
	}
	theFile.Close();
	return pHash;
}


// ----------------------------------------------------------------------------
//		Open
// ----------------------------------------------------------------------------

bool
CIntegrityFileWriter::Open(
	LPCTSTR lpszFile,
	bool bCompressed )
{
	return CCaptureFileWriter::Open( lpszFile, bCompressed );
}


// ----------------------------------------------------------------------------
//		PrepareToWrite
// ----------------------------------------------------------------------------

void
CIntegrityFileWriter::PrepareToWrite(
	CPeekString&				strFileName,
	CStreamOnCByteStream&	sbsXMLStream,
	CCryptHash*				pHash )
{
	HRESULT	hr = S_OK;

	try {
		if ( m_spXMLDoc == NULL ) {
			CreateXmlDoc();
		}

		CPeekString	strHashAlgorithm;
		GetHashAlgorithm( strHashAlgorithm );

		CPeekString	strSignature;
		CPeekString	strHashValue;
		if ( (m_nEnabled == kIntegrity_Hash) && (m_spHashValue != NULL) ) {
			strHashValue = GetHashValue( pHash );

			hr = m_spHashValue->put_text( CComBSTR( strHashValue ) );
			if ( FAILED(hr) ) AtlThrow( hr );
		}
		else if ( (m_nEnabled == kIntegrity_Sign) && (m_spSigValue != NULL) ) {
#ifdef _DEBUG
			strHashValue = GetHashValue( pHash );
#endif
			strSignature = GetSignature( pHash );

			hr = m_spSigValue->put_text( CComBSTR( strSignature ) );
			if ( FAILED(hr) ) AtlThrow( hr );
		}

		hr = m_spFileName->put_text( CComBSTR( strFileName ) );
		if ( FAILED(hr) ) AtlThrow( hr );

		CComPtr<IPersistStream>	spPersist;
		hr = m_spXMLDoc.QueryInterface( &spPersist );
		if ( FAILED(hr) ) AtlThrow( hr );
		hr = spPersist->Save( &sbsXMLStream, TRUE );
		if ( FAILED(hr) ) AtlThrow( hr );
	}
	CATCH_HR(hr)
}


// ----------------------------------------------------------------------------
//		WriteBytes
// ----------------------------------------------------------------------------

bool
CIntegrityFileWriter::WriteBytes(
	const void*	pData,
	UInt32		nLength )
{
	return CCaptureFileWriter::WriteBytes( pData, nLength );
}


// ----------------------------------------------------------------------------
//		WriteIntegrityFile
// ----------------------------------------------------------------------------

BOOL
CIntegrityFileWriter::WriteIntegrityFile(
	CPeekString	strFilePath )
{
	if ( m_nEnabled ) {
		CCryptHash*	pHash= HashFile( strFilePath );
		if ( pHash == NULL ) return FALSE;

		CStreamOnCByteStream	sbsXMLStream;

		CPeekString strFileName( strFilePath );
		PrepareToWrite( strFileName, sbsXMLStream, pHash );

		UInt32 nLength = sbsXMLStream.GetLength();
		if ( nLength == 0 ) return TRUE;

		CPeekString	strIntegrityFilePath( strFilePath + _T(".integrity") );
		XFile	theFile( strIntegrityFilePath );
		BOOL bResult = theFile.Open( kWritePerm, kFile_CreateNew );
		if ( bResult ) {
			bResult = theFile.Write( sbsXMLStream.GetRawDataPtr(), nLength );
		}
		theFile.Close();

		return bResult;
	}
	return TRUE;
}
