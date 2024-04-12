// =============================================================================
//	hlruntime.h
// =============================================================================
//	Copyright (c) 2005-2011 WildPackets, Inc. All rights reserved.

#ifndef HLRUNTIME_H
#define HLRUNTIME_H

#include "hecom.h"
#include "heid.h"
#include "hestr.h"
#include "hevariant.h"
#include "hlexcept.h"
#include <cstddef>

namespace HeLib
{

enum HeLoadFlags
{
	heLoadDynamic,
	heLoadImmediate
};

// HeStringCmp() and HeVarCmp() results. These are returned as a success HeResult.
// Subtracting one gives the usual values of -1 for Less Than, 0 for Equal To,
// +1 for Greater Than.
#define HE_VARCMP_LT			0
#define HE_VARCMP_EQ			1
#define HE_VARCMP_GT			2
#define HE_VARCMP_NULL			3

// HeStringCmp() and HeVarCmp() flags.
#define HE_VARCMP_IGNORECASE	0x00000001

extern "C" HE_API_DECL HeResult	HeInitialize();
extern "C" HE_API_DECL HeResult	HeUninitialize();

extern "C" HE_API_DECL HeResult	HeLoadManifestFile( const char* pszManifestFile, const char* pszLibPath, HeLoadFlags flags );
extern "C" HE_API_DECL HeResult	HeUnloadManifestFile( const char* pszManifestFile  );

extern "C" HE_API_DECL HeResult	HeFreeUnusedLibraries();

extern "C" HE_API_DECL HeResult	HeComponentInfo( const Helium::HeID* pCat, std::size_t nCat, char** ppszComponentInfo );

extern "C" HE_API_DECL HeResult	HeCIDFromProgID( const char* pszProgID, Helium::HeCID* pCID );
extern "C" HE_API_DECL HeResult	HeProgIDFromCID( const Helium::HeCID& cid, char** ppszProgID );
extern "C" HE_API_DECL HeResult HeEnumClassesOfCategory( const Helium::HeID& catid, Helium::HeCID** ppCIDs, std::size_t* pnCID );

extern "C" HE_API_DECL HeResult	HeGetClassObject( const Helium::HeCID& cid, const Helium::HeIID& iid, void** ppv );

extern "C" HE_API_DECL HeResult	HeCreateInstance( const Helium::HeCID& cid, const Helium::HeIID& iid, void** ppv );

extern "C" HE_API_DECL HeResult	HeCreateID( Helium::HeID* pID );

extern "C" HE_API_DECL void*	HeTaskMemAlloc( std::size_t cb );
extern "C" HE_API_DECL void		HeTaskMemFree( void* pv );
extern "C" HE_API_DECL void*	HeTaskMemRealloc( void* pv, std::size_t cb );

extern "C" HE_API_DECL Helium::HEBSTR	HeAllocString( const wchar_t* psz );
extern "C" HE_API_DECL int				HeReAllocString( Helium::HEBSTR* pbstr,const wchar_t* psz );
extern "C" HE_API_DECL Helium::HEBSTR	HeAllocStringLen( const wchar_t* pch, std::size_t cch );
extern "C" HE_API_DECL int				HeReAllocStringLen( Helium::HEBSTR* pbstr, const wchar_t* pch, std::size_t cch );
extern "C" HE_API_DECL void				HeFreeString( Helium::HEBSTR bstr );
extern "C" HE_API_DECL std::size_t		HeStringLen( Helium::HEBSTR bstr );
extern "C" HE_API_DECL std::size_t		HeStringByteLen( Helium::HEBSTR bstr );
extern "C" HE_API_DECL Helium::HEBSTR	HeAllocStringByteLen( const char* pch, std::size_t len );
extern "C" HE_API_DECL HeResult			HeStringCmp( Helium::HEBSTR bstrLeft,
											Helium::HEBSTR bstrRight, UInt32 flags );

extern "C" HE_API_DECL HeResult	HeVarInit( Helium::HEVARIANT* pVar );
extern "C" HE_API_DECL HeResult	HeVarClear( Helium::HEVARIANT* pVar );
extern "C" HE_API_DECL HeResult	HeVarCopy( Helium::HEVARIANT* pVarDest, const Helium::HEVARIANT* pVarSrc );
extern "C" HE_API_DECL HeResult	HeVarCopyInd( Helium::HEVARIANT* pVarDest, const Helium::HEVARIANT* pVarSrc );

extern "C" HE_API_DECL HeResult	HeVarChangeType( Helium::HEVARIANT* pVarDest,
						const Helium::HEVARIANT* pVarSrc, UInt16 flags, Helium::HEVARTYPE vt );

extern "C" HE_API_DECL HeResult	HeVarCmp( const Helium::HEVARIANT* pVarLeft,
						const Helium::HEVARIANT* pVarRight, UInt32 flags );

class CHeAutoInitialize
{
public:
	CHeAutoInitialize()
	{
		HeThrowIfFailed( HeInitialize() );
	}

	~CHeAutoInitialize() throw()
	{
		HeUninitialize();
	}

private:
	CHeAutoInitialize( const CHeAutoInitialize& );
	CHeAutoInitialize& operator=( const CHeAutoInitialize& );
};

} /* namespace HeLib */

#endif /* HLRUNTIME_H */
