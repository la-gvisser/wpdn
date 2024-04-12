// =============================================================================
//	peeknames.h
// =============================================================================
//	Copyright (c) 2003-2020 LiveAction Inc. All rights reserved.

#ifndef PEEKNAMES_H
#define PEEKNAMES_H

#include "heunk.h"
#include "MediaTypes.h"

/// \defgroup Name Table interfaces and classes implemented by Omni.
/// \brief Name Table interfaces and classes.

/// \enum PeekNameEntryType
/// \ingroup Name Table
/// \brief Name Table entry types.
/// \see PeekNameEntry
enum PeekNameEntryType
{
	peekNameEntryTypeUnknown,
	peekNameEntryTypeWorkstation,
	peekNameEntryTypeServer,
	peekNameEntryTypeRouter,
	peekNameEntryTypeSwitch,
	peekNameEntryTypeRepeater,
	peekNameEntryTypePrinter,
	peekNameEntryTypeAccessPoint
};

/// \enum PeekNameEntrySource
/// \ingroup Name Table
/// \brief Name Table entry types.
/// \see PeekNameEntry
enum PeekNameEntrySource
{
	peekNameEntrySourceUnknown,		///< Name source unknown.
	peekNameEntrySourceActive,		///< Name came from active resolver.
	peekNameEntrySourcePassive,		///< Name came from passive resolver.
	peekNameEntrySourceUser,		///< Name came from user.
	peekNameEntrySourcePlugin,		///< Name came from a plug-in.
	peekNameEntrySourceWildcard		///< Name came from wildcard match.
};

/// \enum PeekNameTrust
/// \ingroup Name Table
/// \brief Name Table trust types.
/// \see PeekNameEntry
enum PeekNameEntryTrust
{
	peekNameEntryTrustUnknown,		///< Trust is unknown.
	peekNameEntryTrustKnown,		///< Address is known.
	peekNameEntryTrustTrusted,		///< Address is trusted.
	peekNameEntryTrustUntrusted		///< Address is untrusted.
};

/// \enum PeekNameAddOptions
/// \ingroup Name Table
/// \brief Name Table trust types.
/// \see INameTable
enum PeekNameAddOptions
{
	peekNameAddOptionNoMatchAdd			= 0x0001,
	peekNameAddOptionNoMatchSkip		= 0x0002,
	peekNameAddOptionNameMatchAdd		= 0x0010,
	peekNameAddOptionNameMatchReplace	= 0x0020,
	peekNameAddOptionNameMatchSkip		= 0x0040,
	peekNameAddOptionAddrMatchAdd		= 0x0100,
	peekNameAddOptionAddrMatchReplace	= 0x0200,
	peekNameAddOptionAddrMatchSkip		= 0x0400,
	peekNameAddOptionDelete				= 0x8000,
	peekNameAddOptionsDefault			= (peekNameAddOptionNoMatchAdd |
											peekNameAddOptionNameMatchReplace |
											peekNameAddOptionAddrMatchSkip),
	peekNameAddOptionsManual			= (peekNameAddOptionNoMatchAdd |
											peekNameAddOptionNameMatchAdd |
											peekNameAddOptionAddrMatchReplace)
};

/// \enum PeekNameLookupResult
/// \ingroup Name Table
/// \brief Result codes for a lookup by name.
enum PeekNameLookupResult
{
	peekNameLookupResultNoMatch,
	peekNameLookupResultTypeMatched,
	peekNameLookupResultNameMatched
};

/// \struct PeekNameEntry
/// \ingroup Name Table
/// \brief An entry in a name table.
/// \see INameTable
struct PeekNameEntry
{
	TMediaSpec				Spec;
	wchar_t					Name[64];
	SInt32					Group;
	SInt32					Modified;
	SInt32					LastUsed;
	UInt32					Color;
	PeekNameEntryType		Type;
	PeekNameEntrySource		Source;
	PeekNameEntryTrust		Trust;
};

/// \interface INameTable
/// \ingroup Name Table
/// \brief A name table.
/// \see PeekNameEntry
#define INameTable_IID \
{ 0x8069F53F, 0xBF1B, 0x4F4C, {0x8E, 0x72, 0xB3, 0x1B, 0x08, 0x50, 0xE1, 0x38} }

class HE_NO_VTABLE INameTable : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(INameTable_IID)

	/// Get an entry by index.
	HE_IMETHOD GetItem(/*in*/ TMediaSpecClass msc, /*in*/ SInt32 nIndex,
		/*out*/ PeekNameEntry* pEntry) = 0;
	/// Get the count of name entries in the table.
	HE_IMETHOD GetCount(/*in*/ TMediaSpecClass msc, /*out*/ SInt32* pnCount) = 0;
	/// Arbitrary counter that increments when you add/remove/edit.
	HE_IMETHOD GetChangeCount(/*out*/ SInt32* pnChangeCount) = 0;
	/// Add a name entry. Options are PeekNameAddOptions.
	HE_IMETHOD Add(/*in*/ const PeekNameEntry* pEntry, /*in*/ UInt32 opt) = 0;
	/// Remove entry with this media spec.
	HE_IMETHOD Remove(/*in*/ const TMediaSpec* pSpec) = 0;
	/// Remove all entries.
	HE_IMETHOD RemoveAll() = 0;
	/// Purge old entries.
	HE_IMETHOD Purge() = 0;
	/// Merge with another name table.
	HE_IMETHOD Merge(/*in*/ INameTable* pNameTable, /*in*/ UInt32 opt) = 0;
	/// Get the group name for an entry.
	HE_IMETHOD GetGroup(/*in*/ PeekNameEntry* pEntry,
		/*out*/ Helium::HEBSTR* pbstrGroup) = 0;
	// Get a media spec from entry and entry type strings.
	HE_IMETHOD MediaSpecFromEntryString(/*in*/ Helium::HEBSTR bstrEntry,
		/*in*/ Helium::HEBSTR bstrEntryType, /*out*/ TMediaSpec* pSpec) = 0;
	/// Add a group.
	HE_IMETHOD AddGroup(/*in*/ const wchar_t* pszGroupName, /*in*/ const UInt8 nClass, /*out*/SInt32* pIndex) = 0;

};

/// \interface ILookupNameEntry
/// \ingroup Name Table
/// \brief Lookup an entry in a name table.
#define ILookupNameEntry_IID \
{ 0xCECF4167, 0x6324, 0x423A, {0xB1, 0xAA, 0x84, 0xF2, 0x4A, 0xDC, 0x83, 0x8D} }

class HE_NO_VTABLE ILookupNameEntry : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ILookupNameEntry_IID)

	/// Lookup an entry in a name table.
	HE_IMETHOD LookupNameEntry(/*in*/ const TMediaSpec* pSpec,
		/*out*/ PeekNameEntry* pEntry, /*out*/ BOOL* pbResult) = 0;
};

/// \interface ILookupNameEntry2
/// \ingroup Name Table
/// \brief Lookup an entry in a name table.
#define ILookupNameEntry2_IID \
{ 0xB27EBA29, 0x8076, 0x4A9E, {0x9B, 0xD5, 0x0A, 0xB6, 0xD0, 0x1C, 0x3F, 0x63} }

class HE_NO_VTABLE ILookupNameEntry2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ILookupNameEntry2_IID)

	/// Lookup an entry in a name table.
	HE_IMETHOD LookupNameEntry(/*in*/ Helium::HEBSTR bstrName, /*in*/ TMediaSpecType mst,
		/*out*/ PeekNameEntry* pEntry, /*out*/ BOOL* pbResult) = 0;
};

/// \interface ILookupCountryEntry
/// \ingroup Name Table
/// \brief Lookup an entry in a country list.
#define ILookupCountryEntry_IID \
{ 0x58EBD84E, 0xE5CE, 0x4DB6, {0xA1, 0x7F, 0x59, 0x51, 0x24, 0x88, 0x71, 0x94} }

class HE_NO_VTABLE ILookupCountryEntry : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ILookupCountryEntry_IID)

	/// Is a valid country code.
	HE_IMETHOD IsValidCountryCode(/*in*/ const char code[2],
		/*out*/ BOOL* pbValid) = 0;
	/// Lookup a country name.
	HE_IMETHOD LookupCountryName(/*in*/ const char code[2],
		/*out*/ Helium::HEBSTR* pbstrCountryName) = 0;
	/// Lookup an entry in a country list.
	HE_IMETHOD LookupCountryCode(/*in*/ Helium::HEBSTR bstrCountryName,
		/*out*/ char code[2]) = 0;
};

/// \interface IPassiveNameResolver
/// \ingroup Name Table
/// \brief Configure a passive name resolver.
#define IPassiveNameResolver_IID \
{ 0xFC7387D6, 0xC3D9, 0x4956, {0xB4, 0x75, 0x66, 0xFD, 0x0F, 0xF1, 0x24, 0x55} }

class HE_NO_VTABLE IPassiveNameResolver : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPassiveNameResolver_IID)

	/// Set the name table to send results.
	HE_IMETHOD SetNameTable(/*in*/ IHeUnknown* pNameTable) = 0;
};

/// \interface IActiveNameResolver
/// \ingroup Name Table
/// \brief Resolve IPv4 or IPv6 names and addresses
#define IActiveNameResolver_IID \
{ 0xFC7387D6, 0xC3D9, 0x4956, {0xB4, 0x75, 0x66, 0xFD, 0x0F, 0xF1, 0x24, 0x55} }

class HE_NO_VTABLE IActiveNameResolver : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IActiveNameResolver_IID)

	HE_IMETHOD ResolveAddressSync(/*in*/ const TMediaSpec* pSpec,
		/*out*/ Helium::HEBSTR* pbstrName) = 0;
	HE_IMETHOD ResolveNameSync(/*in*/ const wchar_t* pszName,
		/*in*/ TMediaSpecType type, /*out*/ TMediaSpec* pSpec) = 0;
};


/// \brief Service identifier for name table services.
#define NameTable_SID \
{ 0x5574E41E, 0x2D6C, 0x48F4, {0xB5, 0xA8, 0xA7, 0x85, 0x6F, 0xC6, 0xA2, 0x90} }

#define NameTable_CID \
{ 0xED7AA218, 0xD7F4, 0x4815, {0x9E, 0xC8, 0x7A, 0x73, 0x7C, 0x06, 0x3B, 0x3F} }

#define PassiveNameResolver_CID \
{ 0x54C7E732, 0x53DF, 0x43A3, {0x9A, 0x3E, 0x13, 0xDD, 0xFB, 0x93, 0xDE, 0xAB} }

// {611B336F-5C12-4A0E-9D2E-9A7997872BA0}
#define ActiveNameResolver_CID \
{ 0x611B336F, 0x5C12, 0x4A0E, {0x9D, 0x2E, 0x9A, 0x79, 0x97, 0x87, 0x2B, 0xA0} }

#endif /* PEEKNAMES_H */
