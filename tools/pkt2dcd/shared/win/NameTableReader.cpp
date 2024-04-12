// =============================================================================
//	NameTableDefault.cpp
// =============================================================================
//	Copyright (c) 1998-2015 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "NameTableReader.h"
#include "ByteStream.h"
#include "FileUtil.h"
#include "MediaSpecUtil.h"
#include "MemUtil.h"
#include "NameTableFile.h"
#include "NameTableParser.h"
//#include "NAVLDescriptionsLoader.h"
#include "OutputFileStream.h"
//#include "PeekSetup.h"
//#include "PerfUtil.h"
#include "Protospecs.h"
#include "Resource.h"
#include <atlpath.h>
#include <string.h>
#include <stdio.h>

#if defined(OPT_PROFILE_TIMINGS)
#include "PerfCollector.h"
#endif

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// =============================================================================
//	CNameSubTableReader
// =============================================================================

// -----------------------------------------------------------------------------
//		CNameSubTableReader
// -----------------------------------------------------------------------------

CNameSubTableReader::CNameSubTableReader()
	:	m_Items( sizeof(SNameTableEntry) ),
		m_Wildcards( sizeof(SInt32) ),
		m_Groups( sizeof(SGroupInfo) ),
		m_bCompressionOff( false ),
		m_bUseRemoveAfter( false ),
		m_nRemoveAfterDays( 30 ),
		m_pNameTable( NULL )
{
	InitHashTables();
}


// -----------------------------------------------------------------------------
//		~CNameSubTableReader
// -----------------------------------------------------------------------------

CNameSubTableReader::~CNameSubTableReader()
{
	CleanupHashTables();
}


// -----------------------------------------------------------------------------
//		SpecToIndex
// -----------------------------------------------------------------------------

bool
CNameSubTableReader::SpecToIndex(
	const CMediaSpec&	inSpec,
	SInt32&				outIndex ) const
{
	// Start with an invalid index.
	outIndex = CArrayEx::kArrayIndex_Invalid;

	// Sanity check.
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;

	if ( inSpec.IsWildcard() )
	{
		if ( IsVendorID( inSpec ) )
		{
			// Search the vendor hash.
			UInt32	nHashValue = CalculateHashVendorSpec( inSpec );
			for ( SHashItem* pItem = m_VendorHash[nHashValue]; pItem != NULL ; pItem = pItem->pNext )
			{
				// Compare the two items.
				SNameTableEntry*	pEntry = NULL;
				pEntry = (SNameTableEntry*) m_Items.GetItemPtr( pItem->nIndex );
				ASSERT( pEntry != NULL );
				if ( NULL == pEntry ) continue;
				if ( IsDeleted( *pEntry ) ) continue;
				if ( !IsMediaSpecEqual(pEntry->fSpec, inSpec) ) continue;
				
				outIndex = pItem->nIndex;
				break;
			}
		}
		else
		{
			SInt32	nCount = m_Wildcards.GetCount();
			for ( SInt32 i = CArrayEx::kArrayIndex_First; i < nCount; i++ )
			{
				SInt32	nIndex;
				if ( m_Wildcards.GetItem( i, &nIndex ) )
				{
					SNameTableEntry*	pEntry;
					pEntry = (SNameTableEntry*) m_Items.GetItemPtr( nIndex );
					if ( (pEntry != NULL) && IsMediaSpecEqual(pEntry->fSpec, inSpec) )
					{
						outIndex = nIndex;
						break;
					}
				}
			}
		}
	}
	else
	{
		// Search the hash.
		UInt32	nHashValue = CalculateHashSpec( inSpec );
		for ( SHashItem* pItem = m_SpecHash[nHashValue]; pItem != NULL; pItem = pItem->pNext )
		{
			// Compare the two items.
			SNameTableEntry*	pEntry = NULL;
			pEntry = (SNameTableEntry*) m_Items.GetItemPtr( pItem->nIndex );
			ASSERT( pEntry != NULL );
			if ( NULL == pEntry ) continue;
			if ( IsDeleted( *pEntry ) ) continue;
			if ( !IsMediaSpecEqual(pEntry->fSpec, inSpec) ) continue;
			
			outIndex = pItem->nIndex;
			break;
		}
	}

	return (outIndex != CArrayEx::kArrayIndex_Invalid);
}


// -----------------------------------------------------------------------------
//		SpecToWildcardIndex
// -----------------------------------------------------------------------------

bool
CNameSubTableReader::SpecToWildcardIndex(
	const CMediaSpec&	inSpec,
	SInt32&				outIndex )
{
	// Start with an invalid index.
	outIndex = CArrayEx::kArrayIndex_Invalid;

	// Sanity check.
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;

	UInt32	nCount = m_Wildcards.GetCount();
	for ( SInt32 i = CArrayEx::kArrayIndex_First; i < (SInt32) nCount; i++ )
	{
		// Get the entry index.
		SInt32	nEntryIndex;
		if ( m_Wildcards.GetItem( i, &nEntryIndex ) )
		{
			// Get the next wildcard entry.
			SNameTableEntry*	pEntry = (SNameTableEntry*) m_Items.GetItemPtr( nEntryIndex );
			ASSERT( pEntry != NULL );

			// Compare the wildcard entry.
			if ( (pEntry != NULL) && inSpec.CompareWithMask( pEntry->fSpec ) )
			{
				// Setup the new entry.
				SNameTableEntry	theEntry;
				memset( &theEntry, 0, sizeof(SNameTableEntry) );
				theEntry.fSpec = inSpec;
				CreateWildcardName( inSpec, pEntry->fName,
					pEntry->fSpec.fMask, theEntry.fName );
				theEntry.fColor = pEntry->fColor;
				theEntry.fDateModified = time( NULL );
				theEntry.fDateLastUsed = theEntry.fDateModified;
				theEntry.fType = kNameEntryType_Unknown;
				theEntry.fSource = kNameEntrySource_TypeUnknown | kNameEntrySource_ResolveWildcard;
				theEntry.fGroup = kGroupID_None;
				theEntry.fFlags = kNameTableFlag_Hidden;

				// Add the entry.
				outIndex = DoAddEntry( theEntry );
				break;
			}
		}
	}

	if ( (outIndex == CArrayEx::kArrayIndex_Invalid) && 
		(inSpec.GetClass() == kMediaSpecClass_Address) &&
		((inSpec.GetType() == kMediaSpecType_EthernetAddr) ||
		(inSpec.GetType() == kMediaSpecType_WirelessAddr) ||
		(inSpec.GetType() == kMediaSpecType_TokenRingAddr) ||
		(inSpec.GetType() == kMediaSpecType_WAN_DLCIAddr) ) )
	{
		// Lookup the item in the vendor hash.
		UInt32		nVendorHash	= CalculateHashVendorSpec( inSpec );
		SHashItem*	pHashEntry	= m_VendorHash[nVendorHash];

		while ( pHashEntry != NULL )
		{
			SNameTableEntry*	pVendorEntry = NULL;
			if ( IndexToEntryPtr( pHashEntry->nIndex, pVendorEntry ) &&
				inSpec.CompareWithMask( pVendorEntry->fSpec ) )
			{
				// Ignore deleted entries that have yet to be Compress()ed into oblivion. 
				if ( 0 != ( kNameTableFlag_Deleted & pVendorEntry->fFlags ) ) continue;
				
				// Create the new entry.
				SNameTableEntry	theEntry;
				memset( &theEntry, 0, sizeof(SNameTableEntry) );
				CreateWildcardName( inSpec, pVendorEntry->fName,
					pVendorEntry->fSpec.fMask, theEntry.fName );
				theEntry.fSpec = inSpec;
				theEntry.fColor = pVendorEntry->fColor;
				theEntry.fDateModified = time( NULL );
				theEntry.fDateLastUsed = theEntry.fDateModified;
				theEntry.fType = kNameEntryType_Unknown;
				theEntry.fSource = (kNameEntrySource_TypeUnknown | kNameEntrySource_ResolveWildcard);
				theEntry.fGroup = kGroupID_None;
				theEntry.fFlags = kNameTableFlag_Hidden;
				
				// Add the entry.
				outIndex = DoAddEntry( theEntry );
				break;
			}

			pHashEntry = pHashEntry->pNext;
		}
	}

	return (outIndex != CArrayEx::kArrayIndex_Invalid);
}


// -----------------------------------------------------------------------------
//		NameToIndex
// -----------------------------------------------------------------------------

int
CNameSubTableReader::NameToIndex(
	LPCTSTR			inName,
	TMediaSpecType	inType,
	SInt32&			outIndex ) const
{
	// Sanity check.
	ASSERT( inName != NULL );
	if ( inName == NULL ) return kNameLookupResult_NoMatch;

	// Search the hash.
	int			nResult = kNameLookupResult_NoMatch;
	UInt32		nHashValue = CalculateHashName( inName );
	SHashItem*	pItem = m_NameHash[nHashValue];
	while ( pItem != NULL )
	{
		// Compare the two items.
		SNameTableEntry*	pEntry = 
			(SNameTableEntry*) m_Items.GetItemPtr( pItem->nIndex );
		ASSERT( pEntry != NULL );
		if ( _tcsicmp( inName, pEntry->fName ) == 0 )
		{
			if ( IsMediaSpecTypeEqual(pEntry->fSpec.GetType(), inType ) )
			{
				outIndex = pItem->nIndex;
				nResult = kNameLookupResult_TypeMatched;
				break;
			}
			else
			{
				outIndex = pItem->nIndex;
				nResult = kNameLookupResult_NameMatched;
			}
		}

		pItem = pItem->pNext;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		IndexToEntry
// -----------------------------------------------------------------------------

bool
CNameSubTableReader::IndexToEntry(
	SInt32				inIndex,
	SNameTableEntry&	outEntry ) const
{
	return m_Items.GetItem( inIndex, &outEntry );
}


// -----------------------------------------------------------------------------
//		SetEntryAtIndex
// -----------------------------------------------------------------------------

void
CNameSubTableReader::SetEntryAtIndex(
	SInt32					inIndex,
	const SNameTableEntry&	inEntry )
{

	SNameTableEntry*	pEntry = (SNameTableEntry*) m_Items.GetItemPtr( inIndex );
	ASSERT( pEntry != NULL );
	if ( pEntry != NULL )
	{
		// Use DoRemoveItem then DoAddItem to change
		// the name or the protospec, otherwise
		// hash tables will become invalid
		ASSERT( IsMediaSpecEqual( pEntry->fSpec, inEntry.fSpec ) );
		ASSERT( _tcsicmp( pEntry->fName, inEntry.fName ) == 0 );

		*pEntry = inEntry;

		NameTableIndex	nIndex = 0;
		m_pNameTable->SubTableAndIndexToNameTableIndex( this, inIndex, nIndex );
		//BroadcastMessage( kMsg_ItemChanged, (void*) nIndex );
	}
}


// -----------------------------------------------------------------------------
//		DoAddEntry
// -----------------------------------------------------------------------------

UInt32
CNameSubTableReader::DoAddEntry(
	const SNameTableEntry&	inEntry )
{
	SHashItem*	pSpecHashItem = NULL;
	SHashItem*	pNameHashItem = NULL;
	SInt32		nNewEntryIndex = CArrayEx::kArrayIndex_Invalid;
	try
	{
		pSpecHashItem = NULL;
		pNameHashItem = NULL;
		
		nNewEntryIndex = m_Items.AddItem( &inEntry );

		// Add to the name hash.
		UInt32	nNameHashIndex = CalculateHashName( inEntry.fName );
		pNameHashItem = new SHashItem;
		pNameHashItem->nIndex = nNewEntryIndex;
		pNameHashItem->pNext = m_NameHash[nNameHashIndex];
		m_NameHash[nNameHashIndex] = pNameHashItem;

		if ( !inEntry.fSpec.IsWildcard() )
		{
			// Add the the spec hash.
			UInt32	nSpecHashIndex = CalculateHashSpec( inEntry.fSpec );

			pSpecHashItem = new SHashItem;
			pSpecHashItem->nIndex = nNewEntryIndex;
			pSpecHashItem->pNext = m_SpecHash[nSpecHashIndex];
			m_SpecHash[nSpecHashIndex] = pSpecHashItem;
		}
		else
		{
			// Add to the list of wildcards.
			if ( !IsVendorID( inEntry.fSpec ) )
			{
				m_Wildcards.AddItem( &nNewEntryIndex );
			}
			else
			{
				// Add to vendor hash.
				UInt32	nVendorHashIndex = CalculateHashVendorSpec( inEntry.fSpec );

				pSpecHashItem = new SHashItem;
				
				pSpecHashItem->nIndex = nNewEntryIndex;
				pSpecHashItem->pNext = m_VendorHash[nVendorHashIndex];
				m_VendorHash[nVendorHashIndex] = pSpecHashItem;
			}
		}
	}
	catch (...)
	{
		delete pSpecHashItem;
		delete pNameHashItem;

		if ( nNewEntryIndex != CArrayEx::kArrayIndex_Invalid )
		{
			// Mark the item as deleted.
			SNameTableEntry*	pEntry = 
				(SNameTableEntry*) m_Items.GetItemPtr( nNewEntryIndex );
			if ( pEntry != NULL )
			{
				pEntry->fFlags |= kNameTableFlag_Deleted;
			}
		}

		nNewEntryIndex = CArrayEx::kArrayIndex_Invalid;
	}

	if ( nNewEntryIndex != CArrayEx::kArrayIndex_Invalid )
	{
		NameTableIndex	nIndex = 0;
		m_pNameTable->SubTableAndIndexToNameTableIndex( this, nNewEntryIndex, nIndex );
		//BroadcastMessage( kMsg_ItemAdded, (void*) nIndex );
	}

	return nNewEntryIndex;

}


// -----------------------------------------------------------------------------
//		DoRemoveEntry
// -----------------------------------------------------------------------------

void
CNameSubTableReader::DoRemoveEntry(
	SInt32	inIndex )
{

	// HACKASS: When deleting non-vendor wildcards, CompressItems is called
	// to delete entries. It then broadcasts a message, which makes the list
	// completely rebuild itself. Then, at the end, we broadcast itemRemoved,
	// and the list removes the wrong item. Now we remember if we compressed
	// items, and if so we don't broadcast itemRemoved.
	bool	bCompressedItems = false;

	static DWORD dwTotal1 = 0,
				 dwTotal2 = 0,
				 dwTotal3 = 0,
				 dwTotal4 = 0;

	// Get the entry.
	SNameTableEntry*	pEntry = (SNameTableEntry*) m_Items.GetItemPtr( inIndex );
	ASSERT( pEntry != NULL );
	if ( pEntry == NULL ) return;

	pEntry->fFlags |= kNameTableFlag_Deleted;

	if ( pEntry->fSpec.IsWildcard() )
	{
		if ( !IsVendorID( pEntry->fSpec ) )
		{
			// Remove from wildcard array.
			SInt32	nWildcardIndex = m_Wildcards.GetIndexOf( &inIndex );
			if ( nWildcardIndex != CArrayEx::kArrayIndex_Invalid )
			{
				m_Wildcards.RemoveItems( 1, nWildcardIndex );
			}
			
			// HACK: Should probably do this more often.
			// Compress Items, removing hidden entries
			// forces any entries based on this wildcard to be removed.
			if (!m_bCompressionOff)
			{
				CompressItems( true );
				bCompressedItems = true;
			}
		}
		else
		{
			// Remove the vendor hash entry for this item.
			// first get the hash for this spec
			UInt32	nVendorHashIndex = CalculateHashVendorSpec( pEntry->fSpec );
			
			SHashItem*	pCurItem = m_VendorHash[nVendorHashIndex];
			ASSERT(pCurItem != NULL);
			if ( pCurItem != NULL )
			{
				if ( pCurItem->nIndex == inIndex )
				{
					m_VendorHash[nVendorHashIndex] = pCurItem->pNext;
					delete pCurItem;
				}
				else
				{
					while ( pCurItem != NULL )
					{
						SHashItem*	pPrevItem = pCurItem;
						pCurItem = pCurItem->pNext;

						// Check if the current item is the entry for the 
						// item we're removing.
						if ( (pCurItem != NULL) && (pCurItem->nIndex == inIndex) )
						{
							pPrevItem->pNext = pCurItem->pNext;
							delete pCurItem;
							break;
						}
					}
				}
			}

			// TT15402 Name Table must clear vendor ID cache after deleting a vendor ID entry.
			// Remove any entries who exist solely as a cache of the deleted vendor ID.
			MarkDeletedWildcardEntriesFor( pEntry->fSpec );
		}
	}
	else
	{
		// Fix any spec hash entries.
		// first get the hash for this spec
		UInt32		nSpecHashIndex = CalculateHashSpec( pEntry->fSpec );
		SHashItem*	pHashItem = m_SpecHash[nSpecHashIndex];
		if ( pHashItem != NULL )
		{
			if ( pHashItem->nIndex == inIndex )
			{
				m_SpecHash[nSpecHashIndex] = pHashItem->pNext;
				delete pHashItem;
			}
			else
			{
				while ( pHashItem != NULL )
				{
					SHashItem*	pPrevItem = pHashItem;
					pHashItem = pHashItem->pNext;

					// Check if the current item is the entry for
					// the item we're removing.
					if ( (pHashItem != NULL) && (pHashItem->nIndex == inIndex) )
					{
						pPrevItem->pNext = pHashItem->pNext;
						delete pHashItem;
						break;
					}
				}
			}
		}
	}

	// Fix any name hash entries
	// first get the hash for this name
	UInt32	nNameHashIndex = CalculateHashName( pEntry->fName );

	SHashItem*	pHashItem = m_NameHash[nNameHashIndex];
	if ( pHashItem != NULL )
	{
		if ( pHashItem->nIndex == inIndex )
		{
			m_NameHash[nNameHashIndex] = pHashItem->pNext;
			delete pHashItem;
		}
		else
		{
			while ( pHashItem != NULL )
			{
				SHashItem*	pPrevItem = pHashItem;
				pHashItem = pHashItem->pNext;

				// Check if the current item is the entry for
				// the item we're removing.
				if ( (pHashItem != NULL) && (pHashItem->nIndex == inIndex) )
				{
					pPrevItem->pNext = pHashItem->pNext;
					delete pHashItem;
					break;
				}
			}
		}
	}

	if ( !bCompressedItems )
	{
		NameTableIndex	nIndex = 0;
		m_pNameTable->SubTableAndIndexToNameTableIndex( this, inIndex, nIndex );
		//BroadcastMessage( kMsg_ItemRemoved, (void*) nIndex );
	}
}


// -----------------------------------------------------------------------------
//		GetNthGroupName
// -----------------------------------------------------------------------------

void
CNameSubTableReader::GetNthGroupName(
	SInt32 		inGroupIndex,
	LPTSTR		outName ) const
{
	// Sanity check.
	ASSERT( outName != NULL );
	if ( outName == NULL ) return;

	// Start with an empty group name.
	outName[0] = 0;

	SGroupInfo*	pGroupInfo = (SGroupInfo*) m_Groups.GetItemPtr( inGroupIndex );
	if ( pGroupInfo != NULL )
	{
		_tcscpy( outName, pGroupInfo->fName );
	}
}


// -----------------------------------------------------------------------------
//		GroupNameToIndex
// -----------------------------------------------------------------------------

UInt8
CNameSubTableReader::GroupNameToIndex(
	LPCTSTR	inGroupName ) const
{
	// Sanity check.
	ASSERT( inGroupName != NULL );
	if ( inGroupName == NULL ) return kGroupID_None;

	SInt32	nIndex = CArrayEx::kArrayIndex_First;
	SInt32	nCount = m_Groups.GetCount();
	while ( nIndex < nCount )
	{
		SGroupInfo*	pGroupInfo = (SGroupInfo*) m_Groups.GetItemPtr( nIndex );
		ASSERT( pGroupInfo != NULL );
		if ( _tcsicmp( pGroupInfo->fName, inGroupName ) == 0 )
		{
			return (UInt8) nIndex;
		}

		nIndex++;
	}

	return kGroupID_None;
}


// -----------------------------------------------------------------------------
//		AddGroup
// -----------------------------------------------------------------------------

UInt8
CNameSubTableReader::AddGroup(
	LPCTSTR	inGroupName )
{

	// Sanity check.
	ASSERT( inGroupName != NULL );
	if ( inGroupName == NULL ) return kGroupID_None;

	// Setup the group info.
	SGroupInfo	theGroupInfo;
	memset( &theGroupInfo, 0, sizeof(SGroupInfo) );
	// GV: replaced _tcscpy with _tcsncpy since the Group name may come from
	//     a third-party Plugin and we do not document the max length.
	//     And the UI does not check.
	_tcsncpy( theGroupInfo.fName, inGroupName, kMaxNameLength );
	theGroupInfo.fName[kMaxNameLength] = 0;
	theGroupInfo.fItemCount = 0;

	SInt32	nIndex;
	try
	{
		nIndex = m_Groups.AddItem( &theGroupInfo );
	}
	catch (...)
	{
		return kGroupID_None;
	}

	//BroadcastMessage( kMsg_GroupAdded, LongToPtr( nIndex ) );

	return (UInt8) nIndex;
}


// -----------------------------------------------------------------------------
//		RemoveGroup
// -----------------------------------------------------------------------------

void
CNameSubTableReader::RemoveGroup(
	LPCTSTR		inGroupName,
	bool		inRemoveSubItems )
{

	// Sanity check.
	ASSERT( inGroupName != NULL );
	if ( inGroupName == NULL ) return;

	// Get the group index.
	SInt32	nIndex = GroupNameToIndex( inGroupName );

	if ( nIndex != kGroupID_None )
	{
		// Remove the group.
		RemoveGroup( nIndex, inRemoveSubItems );
	}
}


// -----------------------------------------------------------------------------
//		RemoveGroup
// -----------------------------------------------------------------------------

void
CNameSubTableReader::RemoveGroup(
	SInt32	inGroupIndex,
	bool	inRemoveSubItems )
{
	SInt32	nCount = m_Items.GetCount();
	for ( SInt32 i = CArrayEx::kArrayIndex_First; i < nCount; i++ )
	{
		SNameTableEntry*	pEntry = (SNameTableEntry*) m_Items.GetItemPtr( i );
		ASSERT( pEntry != NULL );
		if ( pEntry != NULL )
		{
			if ( pEntry->fGroup == inGroupIndex )
			{
				if ( inRemoveSubItems )
				{
					pEntry->fFlags |= kNameTableFlag_Deleted;
				}
				else
				{
					pEntry->fGroup = kGroupID_None;
				}
			}
			else if ( (pEntry->fGroup > inGroupIndex) && 
				(pEntry->fGroup != kGroupID_None) )
			{
				pEntry->fGroup--;
			}
		}
	}

	m_Groups.RemoveItems( 1, inGroupIndex );

	//BroadcastMessage( kMsg_GroupRemoved, LongToPtr( inGroupIndex ) );
}


// -----------------------------------------------------------------------------
//		RenameGroup
// -----------------------------------------------------------------------------

void
CNameSubTableReader::RenameGroup(
	SInt32		inGroupIndex,
	LPCTSTR		inNewName )
{
	SGroupInfo*	pGroupInfo = (SGroupInfo*) m_Groups.GetItemPtr( inGroupIndex );

	if ( pGroupInfo != NULL )
	{
		// GV: replaced _tcscpy with _tcsncpy
		//     User can rename a group with a string longer than kMaxNameLength
		_tcsncpy( pGroupInfo->fName, inNewName, kMaxNameLength );
		pGroupInfo->fName[kMaxNameLength] = 0;
	}

	//BroadcastMessage( kMsg_GroupChanged, LongToPtr( inGroupIndex ) );
}


// -----------------------------------------------------------------------------
//		MoveEntryToGroup
// -----------------------------------------------------------------------------

UInt8
CNameSubTableReader::MoveEntryToGroup(
	const CMediaSpec&	inSpec,
	LPCTSTR				inGroup )
{
	SInt32	nNewGroupIndex;
	if ( inGroup == NULL || _tcslen( inGroup ) == 0 )
	{
		nNewGroupIndex = kGroupID_None;
	}
	else
	{
		nNewGroupIndex = GroupNameToIndex( inGroup );
		if ( nNewGroupIndex == kGroupID_None )
		{
			return kGroupID_None;
		}
	}

	SInt32				nIndex;
	SNameTableEntry*	pEntry = NULL;
	if ( SpecToIndex( inSpec, nIndex ) &&
		IndexToEntryPtr( nIndex, pEntry ) )
	{
		if ( pEntry->fGroup != nNewGroupIndex )
		{
			pEntry->fGroup = (UInt8) nNewGroupIndex;

			// Doing the following will broadcast a
			// message so lists will know item changed.
			SetEntryAtIndex( nIndex, *pEntry );
		}
	}

	return (UInt8) nNewGroupIndex;
}


// -----------------------------------------------------------------------------
//		CompressItems
// -----------------------------------------------------------------------------

void
CNameSubTableReader::CompressItems(
	bool	inRemoveHidden )
{
	if ( m_bCompressionOff ) return;

	RemoveExpired();
	
	// Loop through, removing deleted items.
	for ( SInt32 i = m_Items.GetCount() - 1; i >= 0; i-- )
	{
		SNameTableEntry*	pEntry;
		pEntry = (SNameTableEntry*) m_Items.GetItemPtr( i );
		ASSERT( pEntry != NULL );

		if ( ((pEntry->fFlags & kNameTableFlag_Deleted) != 0) ||
			(inRemoveHidden && (pEntry->fFlags & kNameTableFlag_Hidden) != 0) )
		{
			SInt32	nStartIndex = 0;
			SInt32	nEndIndex = i;

			for ( ; i >= 0; i-- )
			{
				pEntry = (SNameTableEntry*) m_Items.GetItemPtr( i );
				ASSERT( pEntry != NULL );
				if ( !((pEntry->fFlags & kNameTableFlag_Deleted) != 0 ||
					(inRemoveHidden && (pEntry->fFlags & kNameTableFlag_Hidden) != 0)))
				{
					break;
				}
			}
			nStartIndex = i + 1;

			m_Items.RemoveItems( nEndIndex - nStartIndex + 1, nStartIndex );
		}
	}

	//BroadcastMessage( kMsg_IndiciesChanged );

	// temporarily turn of the auto-remove feature since we just did it at the top of this function
	bool bOld = m_bUseRemoveAfter;
	m_bUseRemoveAfter = false;
	// Rehash all the items.
	Rehash();
	m_bUseRemoveAfter = bOld;
}


// -----------------------------------------------------------------------------
//		RemoveAllEntries
// -----------------------------------------------------------------------------

void 
CNameSubTableReader::RemoveAllEntries()
{
	// Remove all.
	m_Items.RemoveAllItems();
	m_Groups.RemoveAllItems();

	// This will remove wildcards, and cleanup (dispose) the hash tables.
	Rehash();
}


// -----------------------------------------------------------------------------
//		RemoveExpired
// -----------------------------------------------------------------------------

UInt32
CNameSubTableReader::RemoveExpired()
{
#ifdef NEVER
	if ( !m_bUseRemoveAfter ) return 0;

	SNameTableEntry entry;
	memset( &entry, 0, sizeof( entry ) );

	// convert days->hours->minutes->seconds, subtract from current time
	// if the entry is bigger than this number, we keep it
	time_t nDaysDiff = time(NULL) - m_nRemoveAfterDays*24*60*60;	
	SInt32 nIdx = (SInt32) GetCount();
	UInt32 nRemoved = 0;

	// walk through the list and remove old entries
	for (nIdx--; nIdx >= 0; nIdx--)
	{
		if (IndexToEntry(nIdx, entry))
		{
			if ((entry.fSource & kNameEntrySource_ResolveMask) == kNameEntrySource_ResolvePassive &&
				((entry.fFlags & kNameTableFlag_Deleted) == 0) &&
				((entry.fFlags & kNameTableFlag_Hidden) == 0))
			{
				// check the # days since it was used
				if (entry.fDateLastUsed < nDaysDiff)
				{
					DoRemoveEntry(nIdx);
					nRemoved++;
				}
			}
		}
		else
		{
			ASSERT(FALSE);
		}
	}

	if ( nRemoved )
	{
		BroadcastMessage( CNameTable::kMsg_NamesUpdated );
	}
	
	return nRemoved;
#else
	return 0;
#endif
}


// -----------------------------------------------------------------------------
//		CalculateHashSpec
// -----------------------------------------------------------------------------

UInt32
CNameSubTableReader::CalculateHashSpec(
	const CMediaSpec&	inSpec ) const
{
	UInt32	nHashValue;
	UInt16	nDataLen = inSpec.GetDataLength();

	if ( nDataLen >= 4 )
	{
		nHashValue = *(UInt32*) &inSpec.fData[nDataLen - 4];
	}
	else
	{
		nHashValue = 0;
		for ( int i = 0; i < nDataLen; i++ )
		{
			nHashValue = nHashValue << 8;
			nHashValue = nHashValue | inSpec.fData[i];
		}
	}

	return (nHashValue % kSpecHashEntries);
}


// -----------------------------------------------------------------------------
//		CalculateHashVendorSpec
// -----------------------------------------------------------------------------

UInt32
CNameSubTableReader::CalculateHashVendorSpec(
	const CMediaSpec&	inSpec ) const
{
	// Only the first 3 bytes of vendor IDs matter.
	UInt32	nHashValue = inSpec.fData[0];
	nHashValue += inSpec.fData[1];
	nHashValue += inSpec.fData[2];

	return (nHashValue % kVendorHashEntries);
}


// -----------------------------------------------------------------------------
//		CalculateHashName
// -----------------------------------------------------------------------------

UInt32
CNameSubTableReader::CalculateHashName(
	LPCTSTR	inName ) const
{
	ASSERT( inName != NULL );
	
	UInt32			nHashValue = 0;
	UInt16			nDataLen = (UInt16) _tcslen( inName );
	const UInt16*	pData;

	if ( (nDataLen % 2) != 0 )
	{
		nHashValue = *(UInt8*)inName;
		pData = (UInt16*)(inName + 1);
	}
	else
	{
		pData = (UInt16*) inName;
	}

	while ( nDataLen > 2 )
	{
		nHashValue += *pData;
		pData++;
		nDataLen -= 2;
	}

	return (nHashValue % kSpecHashEntries);
}


// -----------------------------------------------------------------------------
//		CreateWildcardName
// -----------------------------------------------------------------------------

void
CNameSubTableReader::CreateWildcardName(
	const CMediaSpec&	inSpec,
	LPCTSTR				inName,
	UInt32				inMask,
	LPTSTR				outName )
{
	// Start with an empty name.
	outName[0] = 0;

	// Sanity check.
	ASSERT( inName != NULL );
	if ( inName == NULL ) return;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return;

	switch ( inSpec.GetType() )
	{
		case kMediaSpecType_EthernetProto:
		case kMediaSpecType_WAN_PPP_Proto:
		case kMediaSpecType_WAN_FrameRelay_Proto:
		case kMediaSpecType_WAN_X25_Proto:
		case kMediaSpecType_WAN_X25e_Proto:
		case kMediaSpecType_WAN_U200_Proto:
		case kMediaSpecType_WAN_Ipars_Proto:
		case kMediaSpecType_WAN_Q931_Proto:
		{
			_stprintf( outName, _T("%s %2.2X-%2.2X"),
				inName, inSpec.fData[0], inSpec.fData[1] );
		}
		break;

		case kMediaSpecType_WAN_DLCIAddr:
		{	// it's a 10 bit value
			_stprintf( outName, _T("%s %3.3X"),
				inName, *(UInt16*) inSpec.fData  );
		}
		break;

		case kMediaSpecType_EthernetAddr:
		case kMediaSpecType_TokenRingAddr:
		case kMediaSpecType_WirelessAddr:
		{
			UInt32	nMask = ~inMask;
			UInt32	nShiftMask = 0x80000000;
			for ( int i = 0; i < 6; i++ )
			{
				if ( (nShiftMask & nMask) != 0 )
				{
					TCHAR	szText[8];
					_stprintf( szText, _T("%2.2X"), inSpec.fData[i] );
					_tcscat( outName, szText );
				}
				else
				{
					// Jump past all bytes that aren't
					// relevant by the mask.
					for ( ; (i < 6) && ((nShiftMask & nMask) == 0); i++ )
					{
						nShiftMask >>= 1;
					}
					i--;
					nShiftMask <<= 1;

					_tcscat( outName, inName );
				}

				if ( i != 5 )
				{
					_tcscat( outName, _T(":") );
				}

				nShiftMask >>= 1;
			}
		}
		break;

		case kMediaSpecType_AppleTalkAddr:
		{
			UInt16	nNetwork = NETWORKTOHOST16( *(UInt16*) inSpec.fData );
			UInt8	nNode = inSpec.fData[2];

			if ( inMask == 0xC0000000 )
			{
				// Node is wild.
				_stprintf( outName, _T("%s %u"), inName, nNetwork );
			}
			else if ( inMask == 0x20000000 )
			{
				// Network is wild.
				_stprintf( outName, _T("%s %u"), inName, nNode );
			}
			else
			{
				// No wild.
				ASSERT( inMask == 0xE0000000 );
				_stprintf( outName, _T("%s %u.%u"), inName, nNetwork, nNode );
			}
		}
		break;

		case kMediaSpecType_IPAddr:
		case kMediaSpecType_IPXAddr:
		case kMediaSpecType_DECnetAddr:
		case kMediaSpecType_IPv6Addr:
		default:
		{
			_tcscpy( outName, inName );
		}
		break;
	}
}


// -----------------------------------------------------------------------------
//		Rehash
// -----------------------------------------------------------------------------

void
CNameSubTableReader::Rehash()
{
	// remove expired entires
	RemoveExpired();
	
	// Delete the current hash table.
	CleanupHashTables();

	// Delete the wildcard list.
	m_Wildcards.RemoveAllItems();

	SHashItem*	pSpecHashItem = NULL;
	SHashItem*	pNameHashItem = NULL;
	SInt32		nItemCount = m_Items.GetCount();
	for ( SInt32 curIndex = CArrayEx::kArrayIndex_First; curIndex < nItemCount; curIndex++ )
	{
		SNameTableEntry*	pEntry	= NULL;

		try
		{
			pEntry = (SNameTableEntry*) m_Items.GetItemPtr( curIndex );

			// Check if it's a wildcard.
			if ( pEntry->fSpec.IsWildcard() )
			{
				if ( IsVendorID( pEntry->fSpec ) )
				{
					// Add to vendor hash.
					pSpecHashItem = new SHashItem;
					UInt32	nVendorHashIndex = CalculateHashVendorSpec( pEntry->fSpec );
					pSpecHashItem->nIndex = curIndex;
					pSpecHashItem->pNext = m_VendorHash[nVendorHashIndex];
					m_VendorHash[nVendorHashIndex] = pSpecHashItem;
				}
				else
				{
					// Add to the list of wildcards.
					m_Wildcards.AddItem( &curIndex );
				}
			}
			else
			{
				// Add to spec hash.
				pSpecHashItem = new SHashItem;
				pNameHashItem = new SHashItem;

				UInt32	nSpecHashIndex = CalculateHashSpec( pEntry->fSpec );
				UInt32	nNameHashIndex = CalculateHashName( pEntry->fName );

				pSpecHashItem->nIndex = curIndex;
				pNameHashItem->nIndex = curIndex;

				pSpecHashItem->pNext = m_SpecHash[nSpecHashIndex];
				m_SpecHash[nSpecHashIndex] = pSpecHashItem;

				pNameHashItem->pNext = m_NameHash[nNameHashIndex];
				m_NameHash[nNameHashIndex] = pNameHashItem;
			}
		}
		catch (...)
		{
			delete pSpecHashItem;
			delete pNameHashItem;
		}
	}
}


// -----------------------------------------------------------------------------
//		InitHashTables
// -----------------------------------------------------------------------------

void
CNameSubTableReader::InitHashTables()
{
	for ( int i = 0; i < kSpecHashEntries; i++ )
	{
		m_SpecHash[i] = NULL;
	}

	for ( int j = 0; j < kVendorHashEntries; j++ )
	{
		m_VendorHash[j] = NULL;
	}

	for ( int k = 0; k < kNameHashEntries; k++ )
	{
		m_NameHash[k] = NULL;
	}
}


// -----------------------------------------------------------------------------
//		CleanupHashTables
// -----------------------------------------------------------------------------

void
CNameSubTableReader::CleanupHashTables()
{
	for ( int i = 0; i < kSpecHashEntries; i++ )
	{
		SHashItem*	pItem = m_SpecHash[i];
		m_SpecHash[i] = NULL;
		while ( pItem != NULL )
		{
			SHashItem*	pNextItem = pItem->pNext;
			delete pItem;
			pItem = pNextItem;
		}
	}

	for ( int j = 0; j < kVendorHashEntries; j++ )
	{
		SHashItem*	pItem = m_VendorHash[j];
		m_VendorHash[j] = NULL;
		while ( pItem != NULL )
		{
			SHashItem* pNextItem = pItem->pNext;
			delete pItem;
			pItem = pNextItem;
		}
	}

	for ( int k = 0; k < kNameHashEntries; k++ )
	{
		SHashItem*	pItem = m_NameHash[k];
		m_NameHash[k] = NULL;
		while ( pItem != NULL )
		{
			SHashItem*	pNextItem = pItem->pNext;
			delete pItem;
			pItem = pNextItem;
		}
	}
}


// -----------------------------------------------------------------------------
//		MarkDeletedWildcardEntriesFor
// -----------------------------------------------------------------------------
///	Iterate through all entries and if any entrie is a kNameEntrySource_ResolveWildcard
/// entry created for the given inVendorID, mark it deleted.
void
CNameSubTableReader::MarkDeletedWildcardEntriesFor(
	const CMediaSpec&	inVendorID )
{
	const SInt32	nItemCount		= m_Items.GetCount();
	const UInt8		CACHE_SOURCE	= kNameEntrySource_TypeUnknown | kNameEntrySource_ResolveWildcard;
	for ( SInt32 itemIndex = 0; itemIndex < nItemCount; ++itemIndex )
	{
		SNameTableEntry*	pCacheEntry	= (SNameTableEntry*) m_Items.GetItemPtr( itemIndex );
		if ( NULL == pCacheEntry ) continue;
		if ( pCacheEntry->fSource != CACHE_SOURCE ) continue;
		if ( false == pCacheEntry->fSpec.CompareWithMask( inVendorID ) ) continue;

		// Mark for deletion.
		pCacheEntry->fFlags |= kNameTableFlag_Deleted;
	}
}


// -----------------------------------------------------------------------------
//		IndexToEntryPtr
// -----------------------------------------------------------------------------

bool
CNameSubTableReader::IndexToEntryPtr(
	SInt32				inIndex,
	SNameTableEntry*&	outEntry ) const
{
	outEntry = (SNameTableEntry*) m_Items.GetItemPtr( inIndex );
	return (outEntry != NULL);
}


// -----------------------------------------------------------------------------
//		IsMediaSpecEqual												[static]
// -----------------------------------------------------------------------------

bool			
CNameSubTableReader::IsMediaSpecEqual(
	const CMediaSpec& inOne, 
	const CMediaSpec& inTwo )
{
	// Compare the class and data, but fudge a little on type
	return ( (inOne.fClass == inTwo.fClass) && 
		IsMediaSpecTypeEqual((TMediaSpecType)inOne.fType, (TMediaSpecType)inTwo.fType) &&
		(inOne.fMask == inTwo.fMask ) &&
		(memcmp( inOne.fData, inTwo.fData, inOne.GetDataLength()) == 0) );
}


// -----------------------------------------------------------------------------
//		IsMediaSpecTypeEqual											[static]
// -----------------------------------------------------------------------------

bool			
CNameSubTableReader::IsMediaSpecTypeEqual(
	TMediaSpecType inOne, 
	TMediaSpecType inTwo )
{
	return ((inOne == inTwo) || 
			(inOne == kMediaSpecType_EthernetAddr && inTwo == kMediaSpecType_WirelessAddr) ||
			(inTwo == kMediaSpecType_EthernetAddr && inOne == kMediaSpecType_WirelessAddr));
}


// =============================================================================
//	CNameTableReader
// =============================================================================

// -----------------------------------------------------------------------------
//		CNameTableReader
// -----------------------------------------------------------------------------

CNameTableReader::CNameTableReader()
	:	m_nChangeCounter( 0 )
{
	m_nResolveActions = kResolveOptions_Default;
	m_bAssignPhysicalNames = false;
	m_bAppendPhysicalName = false;
	m_bPassiveNameResolution = false;
	m_nPassiveNameGroupID = kGroupID_None;
//	m_strNameSuffix = _T("");
//	m_strPassiveNameGroup = _T("");
	m_nSaveFormat = kNameFileFormat_Xml;
	m_Addresses.SetParent( this );
	m_Protocols.SetParent( this );
	m_Ports.SetParent( this );

	//m_Addresses.AddListener( this );
	//m_Protocols.AddListener( this );
	//m_Ports.AddListener( this );
}


// -----------------------------------------------------------------------------
//		~CNameTableReader
// -----------------------------------------------------------------------------

CNameTableReader::~CNameTableReader()
{
}


// -----------------------------------------------------------------------------
//		operator=(const CNameTableOptions&)
// -----------------------------------------------------------------------------

CNameTable&
CNameTableReader::operator=(
	const CNameTableOptions&	rhs )
{
	CNameTableOptions::operator =( rhs );

	return *this;
}


// -----------------------------------------------------------------------------
//		SpecToName
// -----------------------------------------------------------------------------

bool
CNameTableReader::SpecToName(
	const CMediaSpec&	inSpec,
	LPTSTR				outString,
	bool				inAddPrefix,
	COLORREF*			outColor,
	UInt8*				outType,
	UInt8*				outSource,
	LPTSTR				outGroup )
{
	bool	bResult = true;

	// Check if it's a protospec.
	// If it's a protospec, use protospec utils
	// else, use the name table.
	CNameSubTableReader*	pTable = (CNameSubTableReader*) this->ClassToSubTable( inSpec.GetClass(), inSpec.GetType() );

	if ( pTable != NULL )
	{
		SNameTableEntry*	pEntry	= NULL;
		SInt32				nIndex;
		if ( pTable->SpecToIndex( inSpec, nIndex ) )
		{
			pTable->IndexToEntryPtr( nIndex, pEntry );

			if ( outString != NULL )
			{
				_tcscpy( outString, pEntry->fName );
			}

			if ( outColor != NULL )
			{
				*outColor = pEntry->fColor;
			}

			if ( outType != NULL )
			{
				*outType = pEntry->fType;
			}

			if ( outSource != NULL )
			{
				*outSource = pEntry->fSource;
			}

			if( outGroup != NULL && pEntry->fGroup != kGroupID_None )
			{
				pTable->GetNthGroupName( pEntry->fGroup, outGroup );
			}

			pEntry->fDateLastUsed = time( NULL );
		}
		else
		{
			// The entry wasn't found, try wildcards.
			if ( pTable->SpecToWildcardIndex( inSpec, nIndex ) )
			{
				pTable->IndexToEntryPtr( nIndex, pEntry );

				if ( outString != NULL )
				{
					_tcscpy( outString, pEntry->fName );
				}

				if ( outColor != NULL )
				{
					*outColor = pEntry->fColor;
				}

				if ( outType != NULL )
				{
					*outType = pEntry->fType;
				}

				if ( outSource != NULL )
				{
					*outSource = pEntry->fSource;
				}

				pEntry->fDateLastUsed = time( NULL );
			}
			else
			{
				// All else has failed, just convert to a string.
				bResult = false;

				if ( outString != NULL )
				{
					SpecToString( inSpec, outString, inAddPrefix );
				}

				if ( outColor != NULL )
				{
					*outColor = SpecToColor( inSpec );
				}

				if ( outType != NULL )
				{
					*outType = kNameEntryType_Unknown;
				}

				if ( outSource != NULL )
				{
					*outSource = kNameEntrySource_ResolveNone | kNameEntrySource_TypeUnknown;
				}
			}
		}
	}
	else
	{
		bResult = false;

		if ( outString != NULL )
		{
			SpecToString( inSpec, outString, inAddPrefix );
		}

		if ( outColor != NULL )
		{
			*outColor = SpecToColor( inSpec );
		}

		if ( outType != NULL )
		{
			*outType = kNameEntryType_Unknown;
		}

		if ( outSource != NULL )
		{
			*outSource = kNameEntrySource_ResolveUnknown | kNameEntrySource_TypeUnknown;
		}
	}

	return bResult;
}


// -----------------------------------------------------------------------------
//		SpecToName
// -----------------------------------------------------------------------------

bool
CNameTableReader::SpecToName(
	const CMediaSpec&	inSpec,
	std::wstring&		outString,
	bool				inAddPrefix,
	COLORREF*			outColor,
	UInt8*				outType,
	UInt8*				outSource,
	LPTSTR				outGroup )
{
	TCHAR	sz[256];
	sz[0] = 0;
	const bool	bResult = SpecToName( inSpec, sz, inAddPrefix, outColor,
							outType, outSource, outGroup );
	outString = sz;
	return bResult;
}


// -----------------------------------------------------------------------------
//		ExactSpecToName
// -----------------------------------------------------------------------------

bool
CNameTableReader::ExactSpecToName(
	const CMediaSpec&	inSpec,
	LPTSTR				outName ) const
{
	// Sanity check.
	ASSERT( outName != NULL );
	if ( outName == NULL ) return false;

	// Start with an empty name.
	outName[0] = 0;

	// Sanity check.
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;

	// Get the sub table.
	CNameSubTableReader*	pTable = (CNameSubTableReader*) ClassToSubTable( inSpec.GetClass() );

	if ( pTable != NULL )
	{
		SInt32				nIndex;
		SNameTableEntry*	pEntry;
		if ( pTable->SpecToIndex( inSpec, nIndex ) &&
			pTable->IndexToEntryPtr( nIndex, pEntry ) )
		{
			_tcscpy( outName, pEntry->fName );
			return true;
		}
	}

	return false;
}


// -----------------------------------------------------------------------------
//		SpecToString
// -----------------------------------------------------------------------------

void
CNameTableReader::SpecToString(
	const CMediaSpec&	inSpec,
	LPTSTR				outName,
	bool				inAddPrefix ) const
{
	ASSERT( outName != NULL );
	if ( outName == NULL ) return;

	// Clear out name.
	outName[0] = 0;

	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return;

	if ( inSpec.GetType() == kMediaSpecType_ApplicationID )
	{
		//TODO
		//std::wstring	name;
		//if ( CPeekSetup::GetInstance()->GetNAVLDescriptions()->IDToName( (const char*) inSpec.fData, name ) )
		//{
		//	_tcsncpy( outName, name.c_str(), 255 );
		//	outName[255] = 0;
		//}
	}
	else
	{
		size_t	nPrefixLen = 0;
	
		if ( inAddPrefix )
		{
			// Get the prefix string.
			LPCTSTR	pszPrefix = UMediaSpecUtils::GetMediaTypePrefix( inSpec.GetType() );

			if ( pszPrefix != NULL )
			{
				_tcscpy( outName, pszPrefix );
				nPrefixLen = _tcslen( outName );
			}
		}

		UMediaSpecUtils::SpecToString( inSpec, outName + nPrefixLen );
	}
}


// -----------------------------------------------------------------------------
//		SpecToString
// -----------------------------------------------------------------------------

void
CNameTableReader::SpecToString(
	const CMediaSpec&	inSpec,
	std::wstring&		outName,
	bool				inAddPrefix ) const
{
	TCHAR	sz[256];
	sz[0] = 0;
	SpecToString( inSpec, sz, inAddPrefix );
	outName = sz;
}


// -----------------------------------------------------------------------------
//		NameToSpec
// -----------------------------------------------------------------------------

int
CNameTableReader::NameToSpec(
	LPCTSTR			inName,
	TMediaSpecType	inType,
	CMediaSpec&		outSpec ) const
{
	// Start with an invalid spec.
	outSpec.Invalidate();

	// Sanity check.
	ASSERT( inName != NULL );
	if ( inName == NULL ) return kNameLookupResult_NoMatch;

	// Special case for Application.
	if ( inType == kMediaSpecType_ApplicationID )
	{
	
		//TODO
		char appGuid[8] = "TempVal";
		//if ( CPeekSetup::GetInstance()->GetNAVLDescriptions()->NameToID( inName, appGuid ) )
		//{
			outSpec.SetClass( kMediaSpecClass_Protocol );
			outSpec.SetType( kMediaSpecType_ApplicationID );
			outSpec.fMask = CMediaSpec::GetTypeMask( kMediaSpecType_ApplicationID );
			memcpy(outSpec.fData, appGuid, sizeof(appGuid) );
			return kNameLookupResult_TypeMatched;
		//}
	}

	// Try parsing the name first.
	if ( StringToSpec( inName, inType, outSpec ) )
	{
		return kNameLookupResult_TypeMatched;
	}

	int		nResult = kNameLookupResult_NoMatch;

	// Try the addresses.
	if ( nResult != kNameLookupResult_TypeMatched )
	{
		SInt32	nIndex;
		nResult = m_Addresses.NameToIndex( inName, inType, nIndex );
		if ( nResult != kNameLookupResult_NoMatch )
		{
			SNameTableEntry*	pEntry;
			if ( m_Addresses.IndexToEntryPtr( nIndex, pEntry ) )
			{
				outSpec = pEntry->fSpec;
			}
		}
	}

	// Try the protocols.
	if ( nResult != kNameLookupResult_TypeMatched )
	{
		SInt32	nIndex;
		nResult = m_Protocols.NameToIndex( inName, inType, nIndex );
		if ( nResult != kNameLookupResult_NoMatch )
		{
			SNameTableEntry*	pEntry;
			if ( m_Protocols.IndexToEntryPtr( nIndex, pEntry ) )
			{
				outSpec = pEntry->fSpec;
			}
		}
	}

	// Try the ports.
	if ( nResult != kNameLookupResult_TypeMatched )
	{
		SInt32	nIndex;
		nResult = m_Ports.NameToIndex( inName, inType, nIndex );
		if ( nResult != kNameLookupResult_NoMatch )
		{
			SNameTableEntry*	pEntry;
			if ( m_Ports.IndexToEntryPtr( nIndex, pEntry ) )
			{
				outSpec = pEntry->fSpec;
			}
		}
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		StringToSpec
// -----------------------------------------------------------------------------

bool
CNameTableReader::StringToSpec(
	LPCTSTR			inString,
	TMediaSpecType	inType,
	CMediaSpec&		outSpec ) const
{
	return UMediaSpecUtils::StringToSpec( inString, inType, outSpec );
}


// -----------------------------------------------------------------------------
//		SpecToTypeString
// -----------------------------------------------------------------------------

void
CNameTableReader::SpecToTypeString(
	const CMediaSpec&	inSpec,
	LPTSTR				outString ) const
{
#ifdef TODO
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return;

	// Start with an empty string.
	*outString = 0;

	// Get the type string.
	UINT	nStringID = 0;
	switch ( inSpec.GetType() )
	{
		case kMediaSpecType_EthernetAddr:
			nStringID = IDS_MEDIASPECTYPE_ETHERNETADDR;
			break;
		case kMediaSpecType_TokenRingAddr:
			nStringID = IDS_MEDIASPECTYPE_TOKENRINGADDR;
			break;
		case kMediaSpecType_WirelessAddr:
			nStringID = IDS_MEDIASPECTYPE_WIRELESSADDR;
			break;
		case kMediaSpecType_WAN_DLCIAddr:
			nStringID = IDS_MEDIASPECTYPE_WAN_DLCI_ADDR;
			break;
		case kMediaSpecType_AppleTalkAddr:
			nStringID = IDS_MEDIASPECTYPE_APPLETALKADDR;
			break;
		case kMediaSpecType_IPAddr:
			nStringID = IDS_MEDIASPECTYPE_IPADDR;
			break;
		case kMediaSpecType_DECnetAddr:
			nStringID = IDS_MEDIASPECTYPE_DECADDR;
			break;
		case kMediaSpecType_IPXAddr:
			nStringID = IDS_MEDIASPECTYPE_IPXADDR;
			break;
		case kMediaSpecType_IPv6Addr:
			nStringID = IDS_MEDIASPECTYPE_IPV6ADDR;
			break;
		case kMediaSpecType_EthernetProto:
			nStringID = IDS_MEDIASPECTYPE_ETHERPROTO;
			break;
		case kMediaSpecType_LSAP:
			nStringID = IDS_MEDIASPECTYPE_LSAP;
			break;
		case kMediaSpecType_SNAP:
			nStringID = IDS_MEDIASPECTYPE_SNAP;
			break;
		case kMediaSpecType_IPPort:
			nStringID = IDS_MEDIASPECTYPE_IPPORT;
			break;
		case kMediaSpecType_NWPort:
			nStringID = IDS_MEDIASPECTYPE_NWPORT;
			break;
		case kMediaSpecType_WAN_PPP_Proto:
			nStringID = IDS_MEDIASPECTYPE_WAN_PPP_PROTO;
			break;
		case kMediaSpecType_WAN_FrameRelay_Proto:
			nStringID = IDS_MEDIASPECTYPE_WAN_FRAMERELAY_PROTO;
			break;
		case kMediaSpecType_WAN_X25_Proto:
			nStringID = IDS_MEDIASPECTYPE_WAN_X25_PROTO;
			break;
		case kMediaSpecType_WAN_X25e_Proto:
			nStringID = IDS_MEDIASPECTYPE_WAN_X25E_PROTO;
			break;
		case kMediaSpecType_WAN_U200_Proto:
			nStringID = IDS_MEDIASPECTYPE_WAN_U200_PROTO;
			break;
		case kMediaSpecType_WAN_Ipars_Proto:
			nStringID = IDS_MEDIASPECTYPE_WAN_IPARS_PROTO;
			break;
		case kMediaSpecType_WAN_Q931_Proto:
			nStringID = IDS_MEDIASPECTYPE_WAN_Q931_PROTO;
			break;

	}

	if ( nStringID != 0 )
	{
		// Copy the type string.
		_tcscpy( outString, LoadResourceString( nStringID ) );
	}
#else
	outString = L"Something";
#endif
}


// -----------------------------------------------------------------------------
//		IndexToEntry
// -----------------------------------------------------------------------------

bool
CNameTableReader::IndexToEntry(
	NameTableIndex		inIndex,
	SNameTableEntry&	outEntry ) const
{
	CNameSubTable*	pSubTable = NULL;
	SInt32			nIndex = 0;
	if ( NameTableIndexToSubTableAndIndex( inIndex, pSubTable, nIndex ) )
	{
		return pSubTable->IndexToEntry( nIndex, outEntry );
	}

	return false;
}


// -----------------------------------------------------------------------------
//		IndexToEntryPtr
// -----------------------------------------------------------------------------

bool
CNameTableReader::IndexToEntryPtr(
	NameTableIndex		inIndex,
	SNameTableEntry*&	outEntry ) const
{
	CNameSubTable*	pSubTable = NULL;
	SInt32			nIndex = 0;
	if ( NameTableIndexToSubTableAndIndex( inIndex, pSubTable, nIndex ) )
	{
		CNameSubTableReader*	pSubTableDefault = (CNameSubTableReader*) pSubTable;
		return pSubTableDefault->IndexToEntryPtr( nIndex, outEntry );
	}

	return false;
}


// -----------------------------------------------------------------------------
//		SpecToEntry
// -----------------------------------------------------------------------------

bool
CNameTableReader::SpecToEntry(
	const CMediaSpec&	inSpec,
	SNameTableEntry&	outEntry ) const
{
	enum
	{ 
		NOT_FOUND = false,
		FOUND = true
	};
	
	CNameSubTableReader*	pTable = (CNameSubTableReader*) this->ClassToSubTable( inSpec.GetClass(), inSpec.GetType() );
	if ( NULL == pTable ) return NOT_FOUND;

	SInt32	nIndex	= 0;
	bool	bGotIt	= pTable->SpecToIndex( inSpec, nIndex );
	if ( !bGotIt ) return NOT_FOUND;

	SNameTableEntry*	pEntry	= NULL;
	bGotIt = pTable->IndexToEntryPtr( nIndex, pEntry );
	if ( !bGotIt || ( NULL == pEntry )) return NOT_FOUND;

	::memcpy( &outEntry, pEntry, sizeof( SNameTableEntry ) );

	return FOUND;
}

// -----------------------------------------------------------------------------
//		GetCount
// -----------------------------------------------------------------------------

UInt32
CNameTableReader::GetCount() const
{
	return (UInt32)(m_Addresses.GetCount() + m_Protocols.GetCount() + m_Ports.GetCount());
}


// -----------------------------------------------------------------------------
//		AddEntryWithCurrentOptions
// -----------------------------------------------------------------------------

bool
CNameTableReader::AddEntryWithCurrentOptions(
	const SNameTableEntry&	inEntry )
{
	return AddEntry( inEntry, m_nResolveActions );
}


// -----------------------------------------------------------------------------
//		AddEntry
// -----------------------------------------------------------------------------

bool
CNameTableReader::AddEntry(
	const SNameTableEntry&	inEntry,
	UInt16					inResolveOptions )
{
	CNameSubTableReader*	pSubTable = (CNameSubTableReader*) ClassToSubTable( inEntry.fSpec.GetClass() );
	ASSERT( pSubTable != NULL );
	if ( pSubTable == NULL )
	{
		return false;
	}

	// Find old index and entry (by spec).
	SNameTableEntry*	pOldSpecEntry = NULL;
	SInt32				nSpecIndex = CArrayEx::kArrayIndex_Invalid;
	if ( pSubTable->SpecToIndex( inEntry.fSpec, nSpecIndex ) )
	{
		pSubTable->IndexToEntryPtr( nSpecIndex, pOldSpecEntry );
	}

	// Find old index and entry (by name).
	SNameTableEntry*	pOldNameEntry = NULL;
	SInt32				nNameIndex = CArrayEx::kArrayIndex_Invalid;
	if ( pSubTable->NameToIndex( inEntry.fName, inEntry.fSpec.GetType(), nNameIndex ) ==
		 kNameLookupResult_TypeMatched )
	{
		pSubTable->IndexToEntryPtr( nNameIndex, pOldNameEntry );
	}

	// If we're not supposed to replace, and the item we would replace is not hidden.
	if ( (pOldSpecEntry != NULL) && 
		(!(inResolveOptions & kResolveOption_AddrMatchReplace )) &&
		(( pOldSpecEntry->fFlags & kNameTableFlag_Hidden ) == 0) )
	{
		return false;
	}
	else
	{
		inResolveOptions |= kResolveOption_AddrMatchReplace;
	}

	if ( (pOldNameEntry != NULL) && 
		((!(inResolveOptions & kResolveOption_NameMatchReplace)) &&
		(!(inResolveOptions & kResolveOption_NameMatchAdd))) &&
		((pOldNameEntry->fFlags & kNameTableFlag_Hidden) == 0) )
	{
		return false;
	}
	else
	{
		/// +++ HUH? What now? Add or replace?
//		inResolveOptions |= kResolveOption_NameMatchReplace;
//		inResolveOptions |= kResolveOption_NameMatchAdd;
	}

	if ( (nSpecIndex == nNameIndex) &&
		(nSpecIndex != CArrayEx::kArrayIndex_Invalid) )
	{
		const bool	bOldHidden	= (pOldSpecEntry != NULL) && ( 0 != ( pOldSpecEntry->fFlags & kNameTableFlag_Hidden ) );
		const bool	bNewHidden	= ( 0 != ( inEntry.fFlags & kNameTableFlag_Hidden ) );
		
		// Changing the color and/or the group.
		pSubTable->SetEntryAtIndex( nSpecIndex, inEntry );

		// "un-hiding" should be treated as "adding"
		if ( bOldHidden && !bNewHidden )
		{
			NameTableIndex nNameTableIndex;
			SubTableAndIndexToNameTableIndex( pSubTable, nSpecIndex, nNameTableIndex );
			//pSubTable->BroadcastMessage( CNameSubTable::kMsg_ItemAdded, (void*) nNameTableIndex );
		}
		else
		{
			//BroadcastMessage( kMsg_NamesUpdated );
		}
		return true;
	}
	else
	{
		SInt32	nNewIndex = CArrayEx::kArrayIndex_Invalid;

		if ( nSpecIndex != CArrayEx::kArrayIndex_Invalid )
		{
			// Already have spec.
			if ( inResolveOptions & kResolveOption_AddrMatchReplace )
			{
				RemoveEntry( nSpecIndex, pSubTable );
				nNewIndex = pSubTable->DoAddEntry( inEntry );
			}
		}
		else if ( nNameIndex != CArrayEx::kArrayIndex_Invalid )
		{
			// Already have name.
			if ( inResolveOptions & kResolveOption_NameMatchReplace )
			{
				RemoveEntry( nNameIndex, pSubTable );
				nNewIndex = pSubTable->DoAddEntry( inEntry );
			}
			else if ( inResolveOptions & kResolveOption_NameMatchAdd )
			{
				nNewIndex = pSubTable->DoAddEntry( inEntry );
			}
		}
		else
		{
			// Have neither name nor spec.
			nNewIndex = pSubTable->DoAddEntry( inEntry );
		}

		if ( nNewIndex != CArrayEx::kArrayIndex_Invalid )
		{
			//BroadcastMessage( kMsg_NamesUpdated );
			return true;
		}
	}
	return false;
}


// -----------------------------------------------------------------------------
//		RemoveEntry
// -----------------------------------------------------------------------------

void
CNameTableReader::RemoveEntry(
	const CMediaSpec&	inSpec )
{

	// Sanity check.
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return;

	// Get the sub table.
	CNameSubTable*	pSubTable = ClassToSubTable( inSpec.GetClass() );
	if ( pSubTable != NULL )
	{
		SInt32	nIndex;
		if ( pSubTable->SpecToIndex( inSpec, nIndex ) )
		{
			RemoveEntry( nIndex, pSubTable );
		}
	}
}


// -----------------------------------------------------------------------------
//		RemoveEntry
// -----------------------------------------------------------------------------

void
CNameTableReader::RemoveEntry(
	SInt32			inIndex,
	CNameSubTable*	inSubTable )
{
	// Sanity check.
	ASSERT( inSubTable != NULL );
	if ( inSubTable == NULL ) return;
	ASSERT( inIndex != CArrayEx::kArrayIndex_Invalid );
	if ( inIndex == CArrayEx::kArrayIndex_Invalid ) return;

	// Remove the entry.
	inSubTable->DoRemoveEntry( inIndex );
	//BroadcastMessage( kMsg_NamesUpdated );
}


// -----------------------------------------------------------------------------
//		ClassToSubTable
// -----------------------------------------------------------------------------

CNameSubTable*
CNameTableReader::ClassToSubTable(
	const TMediaSpecClass inClass,
	const TMediaSpecType  inType /*= kMediaSpecType_Null*/ ) const
{
	switch ( inClass )
	{
		case kMediaSpecClass_Address :
			return const_cast< CNameSubTableReader* >( &m_Addresses );
			
		case kMediaSpecClass_Protocol:
			if ( kMediaSpecType_ProtoSpec != inType )
			{
				return const_cast< CNameSubTableReader* >( &m_Protocols );
			}
			return NULL;
				
		case kMediaSpecClass_Port    :
			if ( kMediaSpecType_ATPort != inType )
			{
				return const_cast< CNameSubTableReader* >( &m_Ports );
			}
			return NULL;
	}
	return NULL;
}


// -----------------------------------------------------------------------------
//		LoadOptions
// -----------------------------------------------------------------------------

void
CNameTableReader::LoadOptions()
{

	//nTemp = g_Prefs.GetProfileInt( kKeyName_Resolver,
	//	kKeyValue_ResolveActions, kResolveOptions_Default );
	m_nResolveActions = (UInt16) 1057;

	//nTemp = g_Prefs.GetProfileInt(
	//	kKeyName_Resolver, kKeyValue_AssignPhysicalNames, 
	//	kAssignPhysicalNames_Default );
	m_bAssignPhysicalNames = true;

	//nTemp = g_Prefs.GetProfileInt(
	//	kKeyName_Resolver, kKeyValue_AppendPhysicalNames, 
	//	kAppendPhysicalNames_Default );
	m_bAppendPhysicalName = true;

	//nTemp = g_Prefs.GetProfileInt(
	//	kKeyName_Resolver, kKeyValue_PassiveResolution, 
	//	kPassiveResolution_Default );
	m_bPassiveNameResolution = true;

	m_strNameSuffix = "[PHY]"; /*g_Prefs.GetProfileString(
		kKeyName_Resolver, kKeyValue_PhysicalNameSuffix, 
		kPhysicalNameSuffix_Default );*/

	m_strPassiveNameGroup = "Passively Resolved Names"; /* g_Prefs.GetProfileString(
		kKeyName_Resolver, kKeyValue_PassiveNameGroup, 
		LoadResourceString( IDS_PASSIVE_NAME_GROUP_DEFAULT ) );*/

	m_nSaveFormat = kNameFileFormat_TabbedText; /* (NameFileFormat)g_Prefs.GetProfileInt(
		kKeyName_NameTable, kKeyValue_NameFileFormat,
		kNameFileFormat_TabbedText );*/

	// Get group ID.
	if ( m_strPassiveNameGroup.GetLength() > 0 )
	{
		SyncOptions();
	}


}


// -----------------------------------------------------------------------------
//		SaveOptions
// -----------------------------------------------------------------------------

void
CNameTableReader::SaveOptions() const
{
#ifdef TODO
	g_Prefs.WriteProfileInt( kKeyName_Resolver,
		kKeyValue_ResolveActions, m_nResolveActions );
		
	g_Prefs.WriteProfileInt( kKeyName_Resolver,
		kKeyValue_AssignPhysicalNames, m_bAssignPhysicalNames );
	
	g_Prefs.WriteProfileInt( kKeyName_Resolver,
		kKeyValue_AppendPhysicalNames, m_bAppendPhysicalName );
	
	g_Prefs.WriteProfileString( kKeyName_Resolver,
		kKeyValue_PhysicalNameSuffix, m_strNameSuffix );
	
	g_Prefs.WriteProfileInt( kKeyName_Resolver,
		kKeyValue_PassiveResolution, m_bPassiveNameResolution );
	
	g_Prefs.WriteProfileString( kKeyName_Resolver,
		kKeyValue_PassiveNameGroup, m_strPassiveNameGroup );

	bool bUseRemoveAfter = m_Addresses.GetAutoRemove();
	g_Prefs.WriteProfileInt( kKeyName_Resolver,
		kKeyValue_PassiveNameRemoval, bUseRemoveAfter);

	UInt16 nRemoveAfterDays = m_Addresses.GetAutoRemoveDays();
	g_Prefs.WriteProfileInt( kKeyName_Resolver,
		kKeyValue_PassiveNameRemovalDays, nRemoveAfterDays);
#endif
}


// -----------------------------------------------------------------------------
//		DoOptions
// -----------------------------------------------------------------------------

void
CNameTableReader::DoOptions()
{
}


// -----------------------------------------------------------------------------
//		SyncOptions
// -----------------------------------------------------------------------------

void
CNameTableReader::SyncOptions()
{
	// Update m_nPassiveNameGroupID from m_sPassiveNameGroup.
	// This needs to be done if a group is deleted.
	if ( m_strPassiveNameGroup.GetLength() == 0 || !IsPassiveResolverEnabled() )
	{
		m_nPassiveNameGroupID = kGroupID_None;
	}
	else
	{
		m_nPassiveNameGroupID = 
			GetAddressSubTable()->GroupNameToIndex( m_strPassiveNameGroup );
		if ( m_nPassiveNameGroupID == kGroupID_None )
		{
			m_nPassiveNameGroupID = m_Addresses.AddGroup( m_strPassiveNameGroup );
		}
	}
}


// -----------------------------------------------------------------------------
//		LoadFromFile
// -----------------------------------------------------------------------------

void
CNameTableReader::LoadFromFile(
	const std::wstring & in_strFilePath )
{
	// Load options.
	LoadOptions();

	this->Load( in_strFilePath );
}


// -----------------------------------------------------------------------------
//		Load
// -----------------------------------------------------------------------------

void
CNameTableReader::Load(
	const std::wstring&	inPath,
	UInt32*				outValidNames,
	UInt32*				outImported,
	UInt32*				outSkipped )
{

	// Check to see if this is an XML file
	if ( FileUtil::IsXMLFile( inPath.c_str() ) )
	{
		CNameTableParser* pParser = const_cast<CNameTableParser*>(CNameTableParser::NewInstance());
		if ( pParser != NULL )
		{
			NTParserErrCodeType nErrCode = pParser->Parse( inPath.c_str(), this, outValidNames, outImported, outSkipped );
			if ( nErrCode == NT_PARSER_ERR_WRONG_VERSION )
			{
				// Try reading using old implementation
				CByteStream	theStream;
				theStream.ReadFromFile( inPath.c_str() );
				NameTableFile::NameTableImportXML( theStream, this, *outValidNames, *outImported, *outSkipped );
			}
		}
	}
	else
	{
		CByteStream	theStream;
		theStream.ReadFromFile( inPath.c_str() );
		NameTableFile::NameTableImportText( theStream, this, *outValidNames, *outImported, *outSkipped );
	}
	
	// Groups aren't created until we import names, so re-sync options.
	SyncOptions();

	m_Addresses.RemoveExpired();

}


// -----------------------------------------------------------------------------
//		Load
// -----------------------------------------------------------------------------

void
CNameTableReader::Load(
	const std::wstring&	inPath )
{
#if defined(OPT_PROFILE_TIMINGS)
	// Mark the clock 
	UInt64 nStart, nEnd;
	CLOCK_MARK(nStart);

	static CPerfSampler	s_PerfXml;
	static CPerfSampler	s_PerfTxt;
#endif

	// Import XML.
	UInt32	nValid;
	UInt32	nImported;
	UInt32	nSkipped;

	// Check to see if this is an XML file
	if ( FileUtil::IsXMLFile( inPath.c_str() ) )
	{
#if defined(OPT_PROFILE_TIMINGS)
		StPerfSamplerHelper	perfHelper( s_PerfXml );
#endif

		CNameTableParser* pParser = const_cast<CNameTableParser*>(CNameTableParser::NewInstance());
		if ( pParser != NULL )
		{
			NTParserErrCodeType nErrCode = pParser->Parse( inPath.c_str(), this, &nValid, &nImported, &nSkipped );
			if ( nErrCode == NT_PARSER_ERR_WRONG_VERSION )
			{
				// Try reading using old implementation
				CByteStream	theStream;
				theStream.ReadFromFile( inPath.c_str() );
				NameTableFile::NameTableImportXML( theStream, this, nValid, nImported, nSkipped );
			}
		}
	}
	else
	{
#if defined(OPT_PROFILE_TIMINGS)
		StPerfSamplerHelper	perfHelper( s_PerfTxt );
#endif

		CByteStream	theStream;
		theStream.ReadFromFile( inPath.c_str() );
		NameTableFile::NameTableImportText( theStream, this, nValid, nImported, nSkipped );
	}
	
	// Groups aren't created until we import names, so re-sync options.
	SyncOptions();

	//m_Addresses.RemoveExpired();

#if defined(OPT_PROFILE_TIMINGS)
	CLOCK_MARK(nEnd);

	// WRITE The perf-connter
	CPerfCollector* pPerfCollector = CPerfCollector::NewInstance();
	pPerfCollector->AddEvent( new CPerfEvent( _T("NameTable Load"), nEnd-nStart ) );

	TRACE( _T("type\ttime\tsamples    # name table load\n") );
	TRACE( _T("xml\t%f\t%u\n"), s_PerfXml.GetAvgTime(), s_PerfXml.m_nCount );
	TRACE( _T("txt\t%f\t%u\n"), s_PerfTxt.GetAvgTime(), s_PerfTxt.m_nCount );
#endif

}


// -----------------------------------------------------------------------------
//		Save
// -----------------------------------------------------------------------------

void
CNameTableReader::Save(
	const std::wstring&	inFilePath ) 
{
#if defined(OPT_PROFILE_TIMINGS)
	// Mark the clock 
	UInt64 nStart, nEnd;
	CLOCK_MARK(nStart);

	static CPerfSampler	s_PerfXml;
	static CPerfSampler	s_PerfCsv;
	static CPerfSampler	s_PerfTxt;
#endif

	m_Addresses.RemoveExpired();
	
	// Save options.
	SaveOptions();
	
	COutputFileStream	theStream( inFilePath.c_str(), 128 * 1024 );

	switch ( m_nSaveFormat )
	{
		case kNameFileFormat_Xml:
		{
#if defined(OPT_PROFILE_TIMINGS)
			StPerfSamplerHelper	perfHelper( s_PerfXml );
#endif

			// XML format.
			NameTableFile::NameTableExportXML( this, theStream );
		}
		break;

		case kNameFileFormat_TabbedText:
		{
#if defined(OPT_PROFILE_TIMINGS)
			StPerfSamplerHelper	perfHelper( s_PerfTxt );
#endif

			// Tab-delimited text format.
			const TCHAR cSep = _T('\t');
			NameTableFile::NameTableExportText( this, theStream, cSep, NULL, 0 );
		}
		break;

		case kNameFileFormat_Csv:
		{
#if defined(OPT_PROFILE_TIMINGS)
			StPerfSamplerHelper	perfHelper( s_PerfCsv );
#endif

			// Comma Separated Values format.
			const TCHAR cSep = _T(',');
			NameTableFile::NameTableExportText( this, theStream, cSep, NULL, 0 );
		}
		break;
	}

#if defined(OPT_PROFILE_TIMINGS)
	CLOCK_MARK(nEnd);

	// WRITE The perf-counter
	CPerfCollector* pPerfCollector = CPerfCollector::NewInstance();
	pPerfCollector->AddEvent( new CPerfEvent( _T("NameTable Save"), nEnd-nStart ) );

	TRACE( _T("type\ttime\tsamples    # name table save\n") );
	TRACE( _T("xml\t%f\t%u\n"), s_PerfXml.GetAvgTime(), s_PerfXml.m_nCount );
	TRACE( _T("csv\t%f\t%u\n"), s_PerfCsv.GetAvgTime(), s_PerfCsv.m_nCount );
	TRACE( _T("txt\t%f\t%u\n"), s_PerfTxt.GetAvgTime(), s_PerfTxt.m_nCount );
#endif

}


// -------------------------------------------------------------------
//		SaveToFile
// -------------------------------------------------------------------
void
CNameTableReader::SaveToFile(
	const std::wstring& inFilePath )
{
	this->Save( inFilePath );
}


// -----------------------------------------------------------------------------
//		RemoveAllEntries
// -----------------------------------------------------------------------------

void
CNameTableReader::RemoveAllEntries()
{
	// Remove all.
	m_Addresses.RemoveAllEntries();
	m_Protocols.RemoveAllEntries();
	m_Ports.RemoveAllEntries();
	m_nChangeCounter = 0;
	
}


// -----------------------------------------------------------------------------
//		StartBroadcasting
// -----------------------------------------------------------------------------

void			
CNameTableReader::StartBroadcasting()
{
}


// -----------------------------------------------------------------------------
//		StopBroadcasting
// -----------------------------------------------------------------------------

void			
CNameTableReader::StopBroadcasting()
{
}


// -----------------------------------------------------------------------------
//		NamesImported
// -----------------------------------------------------------------------------

void			
CNameTableReader::NamesImported()
{
}


// -----------------------------------------------------------------------------
//		NameTableIndexToSubTableAndIndex
// -----------------------------------------------------------------------------

bool
CNameTableReader::NameTableIndexToSubTableAndIndex(
	NameTableIndex	inIndex,
	CNameSubTable*&	outSubTable, 
	SInt32&			outIndex ) const
{
	UInt32	nSubTable = ((inIndex & kNameTable_SubTableMask) >> kNameTable_SubTableShift);
	outIndex = (inIndex & kNameTable_IndexMask);
	outSubTable	= NULL;

	switch( nSubTable )
	{
		case kNameSubTable_Addresses:
			outSubTable = (CNameSubTable*) &m_Addresses;
			break;

		case kNameSubTable_Protocols:
			outSubTable = (CNameSubTable*) &m_Protocols;
			break;

		case kNameSubTable_Ports:
			outSubTable = (CNameSubTable*) &m_Ports;
			break;
	}

	return (outSubTable != NULL);
}


// -----------------------------------------------------------------------------
//		SubTableAndIndexToNameTableIndex
// -----------------------------------------------------------------------------

bool
CNameTableReader::SubTableAndIndexToNameTableIndex(
	CNameSubTable*	inSubTable, 
	SInt32			inIndex,
	NameTableIndex&	outIndex) const
{
	UInt32	nSubTableIndex = 0;
	if( inSubTable == &m_Addresses )
	{
		nSubTableIndex = kNameSubTable_Addresses;
	}
	else if( inSubTable == &m_Protocols )
	{
		nSubTableIndex = kNameSubTable_Protocols;
	}
	else if( inSubTable == &m_Ports )
	{
		nSubTableIndex = kNameSubTable_Ports;
	}

	outIndex = nSubTableIndex << kNameTable_SubTableShift;
	outIndex |= inIndex;

	return ( nSubTableIndex != 0 );
}


// -----------------------------------------------------------------------------
//		GetChangeCounter
// -----------------------------------------------------------------------------
///	Return a number that increments any time our name table (or its sub tables) changes.
///
/// If your class caches name table data, this works as a check for invalidating that cache.

UInt32
CNameTableReader::GetChangeCounter() const
{
	return m_nChangeCounter;
}


// -----------------------------------------------------------------------------
//		CompressItems
// -----------------------------------------------------------------------------

void
CNameTableReader::CompressItems(
	bool	inRemoveHidden /* = false */ )
{
	m_Addresses.RemoveExpired();
		
	m_Addresses.CompressItems( inRemoveHidden );
	m_Protocols.CompressItems( inRemoveHidden );
	m_Ports.CompressItems( inRemoveHidden );
}


// -----------------------------------------------------------------------------
//		ListenToMessage
// -----------------------------------------------------------------------------
// Bump change counter when sub table changes.
//
// Listening to our sub tables is stronger than just bumping when various
// CNameTable functions get called, because outside classes can reach into our
// sub tables and change them directly without our knowledge.

void
CNameTableReader::ListenToMessage(
	UInt32	inMessage,
	void*	/*ioParam*/ )
{
	switch ( inMessage )
	{
		case CNameSubTable::kMsg_ItemAdded		:
		case CNameSubTable::kMsg_ItemRemoved	:
		case CNameSubTable::kMsg_ItemChanged	:
			m_nChangeCounter++;
			break;
	}
}
