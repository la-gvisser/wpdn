// =============================================================================
//	SAXContentHandlerImpl.h
// =============================================================================
//	Copyright (c) 2002-2007 WildPackets, Inc. All rights reserved.

#pragma once

#include <msxml2.h>

class CSAXContentHandlerImpl :
	public ISAXContentHandler
{
public:
				CSAXContentHandlerImpl() {}
	virtual		~CSAXContentHandlerImpl() {}

	// IUnknown
public:
	STDMETHOD(QueryInterface)(REFIID riid, void** ppv)
	{
		if ( ppv ==	NULL ) return E_POINTER;
		if ( InlineIsEqualGUID(	riid, __uuidof(ISAXContentHandler) ) )
		{
			*ppv = static_cast<ISAXContentHandler*>(this);
			this->AddRef();
			return S_OK;
		}
		if ( InlineIsEqualGUID(	riid, __uuidof(IUnknown) ) )
		{
			*ppv = static_cast<IUnknown*>(this);
			this->AddRef();
			return S_OK;
		}
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	STDMETHOD_(ULONG,AddRef)()
	{
		return 1;
	}
	STDMETHOD_(ULONG,Release)()
	{
		return 1;
	}

	// ISAXContentHandler
public:
	STDMETHOD(putDocumentLocator)(
		/*[in]*/ ISAXLocator*		/*pLocator*/)
	{
		return S_OK;
	}

	STDMETHOD(startDocument)()
	{
		return S_OK;
	}

	STDMETHOD(endDocument)()
	{
		return S_OK;
	}

	STDMETHOD(startPrefixMapping)(
		/*[in]*/ const wchar_t*		/*pwchPrefix*/,
		/*[in]*/ int				/*cchPrefix*/,
		/*[in]*/ const wchar_t*		/*pwchUri*/,
		/*[in]*/ int				/*cchUri*/)
	{
		return S_OK;
	}

	STDMETHOD(endPrefixMapping)(
		/*[in]*/ const wchar_t*		/*pwchPrefix*/,
		/*[in]*/ int				/*cchPrefix*/)
	{
		return S_OK;
	}

	STDMETHOD(startElement)( 
		/*[in]*/ const wchar_t*		/*pwchNamespaceUri*/,
		/*[in]*/ int				/*cchNamespaceUri*/,
		/*[in]*/ const wchar_t*		/*pwchLocalName*/,
		/*[in]*/ int				/*cchLocalName*/,
		/*[in]*/ const wchar_t*		/*pwchRawName*/,
		/*[in]*/ int				/*cchRawName*/,
		/*[in]*/ ISAXAttributes*	/*pAttributes*/)
	{
		return S_OK;
	}

	STDMETHOD(endElement)( 
		/*[in]*/ const wchar_t*		/*pwchNamespaceUri*/,
		/*[in]*/ int				/*cchNamespaceUri*/,
		/*[in]*/ const wchar_t*		/*pwchLocalName*/,
		/*[in]*/ int				/*cchLocalName*/,
		/*[in]*/ const wchar_t*		/*pwchRawName*/,
		/*[in]*/ int				/*cchRawName*/)
	{
		return S_OK;
	}

	STDMETHOD(characters)(
		/*[in]*/ const wchar_t*		/*pwchChars*/,
		/*[in]*/ int				/*cchChars*/)
	{
		return S_OK;
	}

	STDMETHOD(ignorableWhitespace)(
		/*[in]*/ const wchar_t*		/*pwchChars*/,
		/*[in]*/ int				/*cchChars*/)
	{
		return S_OK;
	}

	STDMETHOD(processingInstruction)(
		/*[in]*/ const wchar_t*		/*pwchTarget*/,
		/*[in]*/ int				/*cchTarget*/,
		/*[in]*/ const wchar_t*		/*pwchData*/,
		/*[in]*/ int				/*cchData*/)
	{
		return S_OK;
	}

	STDMETHOD(skippedEntity)(
		/*[in]*/ const wchar_t*		/*pwchName*/,
		/*[in]*/ int				/*cchName*/)
	{
		return S_OK;
	}
};