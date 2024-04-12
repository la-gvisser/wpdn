// =============================================================================
//	peeknames.h
// =============================================================================
//	Copyright (c) 2003-2017 Savvius, Inc. All rights reserved.

#ifndef PEEKNAMES_H
#define PEEKNAMES_H

#include "heunk.h"
#include "MediaTypes.h"

/// \internal
/// \defgroup Name Table interfaces and classes implemented by Omni.
/// \brief Name Table interfaces and classes.

/// \internal
/// \enum PeekNameEntryType
/// \ingroup Name Table
/// \brief Name Table entry types.
/// \see PeekNameEntry
enum PeekNameEntryType
{
	peekNameEntryTypeUnknown,		///< TODO
	peekNameEntryTypeWorkstation,	///< TODO
	peekNameEntryTypeServer,		///< TODO
	peekNameEntryTypeRouter,		///< TODO
	peekNameEntryTypeSwitch,		///< TODO
	peekNameEntryTypeRepeater,		///< TODO
	peekNameEntryTypePrinter,		///< TODO
	peekNameEntryTypeAccessPoint	///< TODO
};

/// \internal
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
	peekNameEntrySourceWildcard		///< Name came from wildcard.
};

/// \internal
/// \enum PeekNameTrust
/// \ingroup Name Table
/// \brief Name Table trust types.
/// \see PeekNameEntry
enum PeekNameEntryTrust
{
	peekNameEntryTrustUnknown,		///< Trust is unknown.
	peekNameEntryTrustKnown,		///< Address is known.
	peekNameEntryTrustTrusted		///< Address is trusted.
};

/// \internal
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
	peekNameAddOptionsDefault			= (peekNameAddOptionNoMatchAdd |
											peekNameAddOptionNameMatchReplace |
											peekNameAddOptionAddrMatchSkip),
	peekNameAddOptionsManual			= (peekNameAddOptionNoMatchAdd |
											peekNameAddOptionNameMatchAdd | 
											peekNameAddOptionAddrMatchReplace)
};

/// \internal
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
	TMediaSpec				Spec;		///< TODO
	wchar_t					Name[64];	///< TODO
	SInt32					Group;		///< TODO
	SInt32					Modified;	///< TODO
	SInt32					LastUsed;	///< TODO
	UInt32					Color;		///< TODO
	PeekNameEntryType		Type;		///< TODO
	PeekNameEntrySource		Source;		///< TODO
	PeekNameEntryTrust		Trust;		///< TODO
};

/// \internal
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
};

/// \internal
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

/// \internal
/// \brief Service identifier for name table services.
#define NameTable_SID \
{ 0x5574E41E, 0x2D6C, 0x48F4, {0xB5, 0xA8, 0xA7, 0x85, 0x6F, 0xC6, 0xA2, 0x90} }

#define NameTable_CID \
{ 0xED7AA218, 0xD7F4, 0x4815, {0x9E, 0xC8, 0x7A, 0x73, 0x7C, 0x06, 0x3B, 0x3F} }


#endif /* PEEKNAMES_H */
