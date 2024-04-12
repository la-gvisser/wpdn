// ============================================================================
//	NameTableDefault.h
// ============================================================================
//	Copyright (c) 2000-2007 WildPackets, Inc. All rights reserved.
//	Copyright (c) 1998-2000 AG Group, Inc. All rights reserved.

#pragma once

#include <time.h>
#include "NameTable.h"
//#include "Broadcaster.h"
#include "ArrayEx.h"
//#include "ILoadable.h"
//#include "ISaveable.h"
//#include "Listener.h"
#include "MediaSpec.h"


class CNameTableReader;

// ============================================================================
//	CNameSubTableDefault
// ============================================================================

class CNameSubTableReader
	:	public CNameSubTable
{
friend class CNameTableReader;

public:
	CNameSubTableReader();
	~CNameSubTableReader();

	UInt32			GetCount() const { return m_Items.GetCount(); }

	bool			SpecToIndex( const CMediaSpec& inSpec, SInt32& outIndex ) const;
	bool			SpecToWildcardIndex( const CMediaSpec& inSpec, SInt32& outEntry );
	int				NameToIndex( LPCTSTR inString, TMediaSpecType inType, SInt32& outIndex ) const;

	bool			IndexToEntry( SInt32 inIndex, SNameTableEntry& outEntry ) const;
	void			SetEntryAtIndex( SInt32 inIndex, const SNameTableEntry& inEntry );

	UInt32			DoAddEntry( const SNameTableEntry& inEntry );
	void			DoRemoveEntry( SInt32 theIndex );

	UInt32			GetGroupCount() const { return m_Groups.GetCount(); }
	void			GetNthGroupName( SInt32 inGroup, LPTSTR outName ) const;
	UInt8			GroupNameToIndex( LPCTSTR inName ) const;
	UInt8			AddGroup( LPCTSTR inName );
	void			RemoveGroup( LPCTSTR inName, bool inRemoveItems );
	void			RemoveGroup( SInt32 inGroupIndex, bool inRemoveSubItems );
	void			RenameGroup( SInt32 inGroupIndex, LPCTSTR inNewName );
	UInt8			MoveEntryToGroup( const CMediaSpec& inSpec, LPCTSTR inGroup );

	void			CompressItems( bool inRemoveHidden = false );
	void			RemoveAllEntries();
	UInt32			RemoveExpired();

	bool			GetAutoRemove()	const { return m_bUseRemoveAfter; }
	UInt16			GetAutoRemoveDays()	const {	return m_nRemoveAfterDays; }
	void			SetAutoRemove( bool bRemove, UInt16 nDays )
	{
		m_bUseRemoveAfter = bRemove;
		m_nRemoveAfterDays = nDays;
	}

	bool			GetCompression() const { return !m_bCompressionOff; }
	void			SetCompression( bool bOn = true ) { m_bCompressionOff = !bOn; }

	void			SetParent( CNameTable* inParent ) { m_pNameTable = inParent; }

protected:
	enum
	{
		kSpecHashEntries	= 10009,
		kNameHashEntries	= 10009,
		kVendorHashEntries	= 253
	};

	struct SHashItem
	{
		SInt32		nIndex;
		SHashItem*	pNext;
	};

	struct SGroupInfo
	{
		TCHAR	fName[kMaxNameLength+1];
		UInt32	fItemCount;
	};

	UInt32			CalculateHashSpec( const CMediaSpec& inSpec ) const;
	UInt32			CalculateHashVendorSpec( const CMediaSpec& inSpec) const;
	UInt32			CalculateHashName( LPCTSTR inName ) const;
	bool			IsVendorID( const CMediaSpec& inSpec ) const
					{
						return (inSpec.GetClass() == kMediaSpecClass_Address) &&
							((inSpec.GetType() == kMediaSpecType_EthernetAddr) ||
							(inSpec.GetType() == kMediaSpecType_WirelessAddr) ||
							(inSpec.GetType() == kMediaSpecType_TokenRingAddr)) &&
							(inSpec.fMask == 0xE0000000);
					}

	void			CreateWildcardName( const CMediaSpec& inSpec, LPCTSTR inName,
						UInt32 inMask, LPTSTR outName );

	void			Rehash();

	SHashItem*		m_SpecHash[kSpecHashEntries];
	SHashItem*		m_VendorHash[kVendorHashEntries];
	SHashItem*		m_NameHash[kNameHashEntries];

	CArrayEx		m_Items;
	CArrayEx		m_Wildcards;
	CArrayEx		m_Groups;

	bool			m_bCompressionOff;

	bool			m_bUseRemoveAfter;
	UInt16			m_nRemoveAfterDays;
	CNameTable*		m_pNameTable;

private:
	void			InitHashTables();
	void			CleanupHashTables();
	void			MarkDeletedWildcardEntriesFor( const CMediaSpec &inVendorID );
	
protected:
	bool			IndexToEntryPtr( SInt32 inIndex, SNameTableEntry*& outPtr ) const;
	static bool		IsMediaSpecEqual( const CMediaSpec& inOne, const CMediaSpec& inTwo ); 
	static bool		IsMediaSpecTypeEqual( TMediaSpecType inOne, TMediaSpecType inTwo ); 
};


// ============================================================================
//	CNameTableDefault
// ============================================================================

class CNameTableReader
	:	public CNameTable
{
public:
				CNameTableReader();
	virtual		~CNameTableReader();

	CNameTable&	operator=(const CNameTableOptions& rhs);

				// Convert a spec to a string, attempt to find a name, otherwise
				// calls SpecToString returns true if a name was found.
	bool		SpecToName( const CMediaSpec& inSpec, LPTSTR outString, 
					bool inAddPrefix = true, COLORREF* outColor = NULL, 
					UInt8* outType = NULL, UInt8* outSource = NULL,
					LPTSTR outGroup = NULL );
	bool		SpecToName( const CMediaSpec& inSpec, std::wstring& outString, 
					bool inAddPrefix = true, COLORREF* outColor = NULL, 
					UInt8* outType = NULL, UInt8* outSource = NULL,
					LPTSTR outGroup = NULL );

				// Converts a spec to name, will not use wildcards.
	bool		ExactSpecToName( const CMediaSpec& inSpec, LPTSTR outString ) const;

				// Converts the spec to a string, no name lookups.
	void		SpecToString( const CMediaSpec& inSpec, LPTSTR outString, 
					bool inAddPrefix = true ) const;
	void		SpecToString( const CMediaSpec& inSpec, std::wstring& outString, 
					bool inAddPrefix = true ) const;

				// Converts a string to a spec, uses StringToSpec, if that fails
				// it looks the name up in the name table.
	int			NameToSpec( LPCTSTR inString, TMediaSpecType inType, CMediaSpec& outSpec ) const;

				// Converts a string to a spec, no name lookups.
	bool		StringToSpec( LPCTSTR inString,
					TMediaSpecType inType, CMediaSpec& outSpec ) const;

				// Get the type name for a spec.
	void		SpecToTypeString( const CMediaSpec& inSpec, LPTSTR outString ) const;

				// Get entry given index (which encodes sub-table and index).
	bool		IndexToEntry( NameTableIndex inIndex, SNameTableEntry& outEntry ) const;
	bool		SpecToEntry( const CMediaSpec& inSpec, SNameTableEntry& outEntry ) const;

	UInt32		GetCount() const;

	bool		AddEntry( const SNameTableEntry& inEntry, UInt16 inResolveActions );
	bool		AddEntryWithCurrentOptions( const SNameTableEntry& inEntry );

	void		RemoveEntry( const CMediaSpec& inSpec );
	void		RemoveEntry( SInt32 inIndex, CNameSubTable* inTable );

	void		RemoveAllEntries();

	CNameSubTable*	GetAddressSubTable() const { return (CNameSubTable*) &m_Addresses; }
	CNameSubTable*	GetProtocolSubTable() const { return (CNameSubTable*) &m_Protocols; }
	CNameSubTable*	GetPortSubTable() const { return (CNameSubTable*) &m_Ports; }
	
	CNameSubTable*	
	ClassToSubTable( const TMediaSpecClass inClass, 
						const TMediaSpecType inType = kMediaSpecType_Null ) const;

	// Load/save.
	virtual void	LoadFromFile( const std::wstring& inFilePath );
	void			Load( const std::wstring& inPath );
	void			Load( const std::wstring& inPath, UInt32* outValidNames, UInt32* outImported, UInt32* outSkipped );
	void			Save( const std::wstring& inPath );	// not a const since it removes expired entries before saving
	virtual void	SaveToFile( const std::wstring& inFilePath );

	// Do options dialog.
	void			DoOptions();

	// Options accessors.
	bool			IsPassiveResolverEnabled() const { return m_bPassiveNameResolution; }
	UInt8			GetPassiveResolveGroup() const { return m_nPassiveNameGroupID; }
	void			SetPassiveResolve( bool inEnable ) { m_bPassiveNameResolution = inEnable; }

	void			SyncOptions();

	virtual void	StartBroadcasting();
	virtual void	StopBroadcasting();

	void			NamesImported();

	void			CompressItems( bool inRemoveHidden = false );
	
	bool			NameTableIndexToSubTableAndIndex( NameTableIndex inIndex,
						CNameSubTable*& outSubTable, SInt32& outIndex ) const;
	bool			SubTableAndIndexToNameTableIndex( CNameSubTable* inSubTable,
						SInt32 inIndex, NameTableIndex& outIndex ) const;

	UInt32			GetChangeCounter() const;

private :
	bool			IndexToEntryPtr( NameTableIndex inIndex, SNameTableEntry*& outPtr ) const;

	void			LoadOptions();
	void			SaveOptions() const;
	virtual void	ListenToMessage( UInt32 inMessage, void* ioParam );
	
private:
	CNameSubTableReader	m_Addresses;
	CNameSubTableReader	m_Protocols;
	CNameSubTableReader	m_Ports;
	UInt32              m_nChangeCounter;
};
