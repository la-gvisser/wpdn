// =============================================================================
//	NameTable.h
// =============================================================================
//	Copyright (c) 1998-2009 WildPackets, Inc. All rights reserved.

#pragma once

#include "ArrayEx.h"
//#include "Broadcaster.h"
//#include "ILoadable.h"
//#include "ISaveable.h"
//#include "Listener.h"
#include "MediaSpec.h"
#include <string>
#include <time.h>

typedef UInt32 NameTableIndex;

enum
{
	kNameTable_SubTableMask		= 0xE0000000,
	kNameTable_IndexMask		= 0x1FFFFFFF,
	kNameTable_SubTableShift	= 29,
	kNameSubTable_Addresses		= 1,
	kNameSubTable_Protocols		= 2,
	kNameSubTable_Ports			= 3
};

enum
{
	kNameEntryType_Unknown,
	kNameEntryType_Workstation,
	kNameEntryType_Server,
	kNameEntryType_Router,
	kNameEntryType_Switch,
	kNameEntryType_Repeater,
	kNameEntryType_Printer,
	kNameEntryType_AccessPoint
};

enum
{
	kNameEntrySource_ResolveMask		= 0xF0,
	kNameEntrySource_ResolveUnknown		= 0x00,	// name came from nowhere or old import
	kNameEntrySource_ResolveActive		= 0x10,	// name came from active resolver
	kNameEntrySource_ResolvePassive		= 0x20,	// name came from passive resolver
	kNameEntrySource_ResolveUser		= 0x30,	// name came from user
	kNameEntrySource_ResolvePlugin		= 0x40,	// name came from a plug-in
	kNameEntrySource_ResolveWildcard	= 0x60,	// name came from wildcard
	kNameEntrySource_ResolveNone		= 0xF0,	// name has not been resolved

	kNameEntrySource_TypeMask			= 0x0F,
	kNameEntrySource_TypeUnknown		= 0x00,
	kNameEntrySource_TypeIP				= 0x01,	// name is from ip name
	kNameEntrySource_TypeAT				= 0x02	// name is from appletalk name
};

enum
{
	kMaxNameLength		= 63
};

enum
{
	kGroupID_None		= 0xFF
};

enum
{
	kNameLookupResult_NoMatch,
	kNameLookupResult_TypeMatched,
	kNameLookupResult_NameMatched
};

enum NameTrust
{
	kNameTrust_Unknown,
	kNameTrust_Known,
	kNameTrust_Trusted
};

enum NameFileFormat
{
	kNameFileFormat_Xml,
	kNameFileFormat_TabbedText,
	kNameFileFormat_Csv
};

enum NameTableFlags
{
	kNameTableFlag_Deleted	= 0x01,
	kNameTableFlag_Hidden	= 0x02
};

struct SNameTableEntry
{
	CMediaSpec		fSpec;
	TCHAR			fName[kMaxNameLength+1];
	COLORREF		fColor;
	time_t			fDateModified;
	time_t			fDateLastUsed;
	UInt8			fType;
	UInt8			fSource;
	UInt8			fGroup;
	UInt8			fFlags;
	UInt8			fTrust;
};

class CNameTable;

// ============================================================================
//	CNameSubTable
// ============================================================================

class CNameSubTable 
	//:
	//public CBroadcaster
{
public:
	enum
	{
		kMsg_ItemAdded			= 'NADD',
		kMsg_ItemRemoved		= 'NDEL',
		kMsg_ItemChanged		= 'NCHA',
		kMsg_IndiciesChanged	= 'NIND',
		kMsg_GroupAdded			= 'GADD',
		kMsg_GroupRemoved		= 'GDEL',
		kMsg_GroupChanged		= 'GCHA'
	};

					CNameSubTable() {}
	virtual			~CNameSubTable() {}

	virtual UInt32	GetCount() const = 0;

	virtual bool	SpecToIndex( const CMediaSpec& inSpec, SInt32& outIndex ) const = 0;
	virtual bool	SpecToWildcardIndex( const CMediaSpec& inSpec, SInt32& outEntry ) = 0;
	virtual int		NameToIndex( LPCTSTR inString, TMediaSpecType inType, SInt32& outIndex ) const = 0;

	virtual bool	IndexToEntry( SInt32 inIndex, SNameTableEntry& outEntry ) const = 0;
	virtual void	SetEntryAtIndex( SInt32 inIndex, const SNameTableEntry& inEntry ) = 0;

	virtual UInt32	DoAddEntry( const SNameTableEntry& inEntry ) = 0;
	virtual void	DoRemoveEntry( SInt32 theIndex ) = 0;

	virtual UInt32	GetGroupCount() const  = 0;
	virtual void	GetNthGroupName( SInt32 inGroup, LPTSTR outName ) const = 0;
	virtual UInt8	GroupNameToIndex( LPCTSTR inName ) const = 0;
	virtual UInt8	AddGroup( LPCTSTR inName ) = 0;
	virtual void	RemoveGroup( LPCTSTR inName, bool inRemoveItems ) = 0;
	virtual void	RemoveGroup( SInt32 inGroupIndex, bool inRemoveSubItems ) = 0;
	virtual void	RenameGroup( SInt32 inGroupIndex, LPCTSTR inNewName ) = 0;
	virtual UInt8	MoveEntryToGroup( const CMediaSpec& inSpec, LPCTSTR inGroup ) = 0;

	virtual void	CompressItems( bool inRemoveHidden = false ) = 0;
	virtual void	RemoveAllEntries() = 0;
	virtual UInt32	RemoveExpired() = 0;

	virtual bool	GetAutoRemove()	const = 0;
	virtual UInt16	GetAutoRemoveDays()	const = 0;
	virtual void	SetAutoRemove( bool inRemove, UInt16 inDays ) = 0;

	virtual bool	GetCompression() const = 0;
	virtual void	SetCompression( bool inOn = true ) = 0;

	virtual void	SetParent( CNameTable* inParent ) = 0;

	static bool		IsDeleted( const SNameTableEntry& inEntry )
					{
						return ( 0 != ( kNameTableFlag_Deleted & inEntry.fFlags ) );
					}
	static bool		IsHidden( const SNameTableEntry& inEntry )
					{
						return ( 0 != ( kNameTableFlag_Hidden & inEntry.fFlags ) );
					}
};


// ============================================================================
//	CNameTableOptions
// ============================================================================

class CNameTableOptions
{ 
public:
	UInt16			m_nResolveActions;
	bool			m_bAssignPhysicalNames;
	bool			m_bAppendPhysicalName;
	bool			m_bPassiveNameResolution;
	UInt8			m_nPassiveNameGroupID;
	CString			m_strNameSuffix;
	CString			m_strPassiveNameGroup;
	NameFileFormat	m_nSaveFormat;

public:
	CNameTableOptions() :
		m_nResolveActions( 0 ),
		m_bAssignPhysicalNames( false ),
		m_bAppendPhysicalName( false ),
		m_bPassiveNameResolution( false ),
		m_nPassiveNameGroupID( 0 ),
//		m_strNameSuffix( _T("") ),
//		m_strPassiveNameGroup( _T("") ),
		m_nSaveFormat( kNameFileFormat_Xml )
	{
	}

	CNameTableOptions& operator=( const CNameTableOptions& rhs )
	{
		if ( this != &rhs )
		{
			m_nResolveActions = rhs.m_nResolveActions;
			m_bAssignPhysicalNames = rhs.m_bAssignPhysicalNames;
			m_bAppendPhysicalName = rhs.m_bAppendPhysicalName;
			m_bPassiveNameResolution = rhs.m_bPassiveNameResolution;
			m_nPassiveNameGroupID = rhs.m_nPassiveNameGroupID;
			m_strNameSuffix = rhs.m_strNameSuffix;
			m_strPassiveNameGroup = rhs.m_strPassiveNameGroup;
			m_nSaveFormat = rhs.m_nSaveFormat;
		}

		return *this;
	}
};


// ============================================================================
//	CNameTable
// ============================================================================

class CNameTable :
	public CNameTableOptions//,
	//public CBroadcaster,
	//public ILoadable,
	//public ISaveable,
	//private CListener
{
public:
	enum
	{
		kMsg_NamesUpdated		= 'NUPD'
	};

	enum
	{
		kResolveOption_NoMatchAdd			= 0x0001,
		kResolveOption_NoMatchSkip			= 0x0002,
		kResolveOption_NameMatchAdd			= 0x0010,
		kResolveOption_NameMatchReplace		= 0x0020,
		kResolveOption_NameMatchSkip		= 0x0040,
		kResolveOption_AddrMatchAdd			= 0x0100,
		kResolveOption_AddrMatchReplace		= 0x0200,
		kResolveOption_AddrMatchSkip		= 0x0400,

		kResolveOptions_Default = kResolveOption_NoMatchAdd |
									kResolveOption_AddrMatchSkip |
									kResolveOption_NameMatchReplace,

		kResolveOptions_Manual	= kResolveOption_NoMatchAdd |
									kResolveOption_NameMatchAdd | 
									kResolveOption_AddrMatchReplace
	};

	CNameTable() {}
	~CNameTable() {}

	CNameTable&	operator=( const CNameTableOptions& rhs );

	static bool		GetNameEntryTypeString( const UInt8 nNameEntryType, CString& strNameEntryType );
	static bool		GetTrustString( const UInt8 nTrust, CString& strTrust );

	static COLORREF	GenerateAutoColor( const CMediaSpec& inSpec );
	static COLORREF	SpecToColor( const CMediaSpec& inSpec );

	// Get the icon for a spec/node type.
	virtual int		IconFromType( TMediaSpecType inSpecType, UInt8 inNameEntryType ) const;

	// Receive results from active resolver.
	virtual void	ReceiveResult( const CMediaSpec& inLogical,
						const CMediaSpec& inPhysical, LPCTSTR inName );

	// Convert a spec to a string, attempt to find a name, otherwise
	// calls SpecToString returns true if a name was found.
	virtual bool	SpecToName( const CMediaSpec& inSpec, LPTSTR outString, 
						bool inAddPrefix = true, COLORREF* outColor = NULL, 
						UInt8* outType = NULL, UInt8* outSource = NULL,
						LPTSTR outGroup = NULL ) = 0;
	virtual bool	SpecToName( const CMediaSpec& inSpec, std::wstring& outString, 
						bool inAddPrefix = true, COLORREF* outColor = NULL, 
						UInt8* outType = NULL, UInt8* outSource = NULL,
						LPTSTR outGroup = NULL ) = 0;

	// Converts a spec to name, will not use wildcards.
	virtual bool	ExactSpecToName( const CMediaSpec& inSpec, LPTSTR outString ) const = 0;

	// Converts the spec to a string, no name lookups.
	virtual void	SpecToString( const CMediaSpec& inSpec, LPTSTR outString, 
						bool inAddPrefix = true ) const = 0;
	virtual void	SpecToString( const CMediaSpec& inSpec, std::wstring& outString, 
						bool inAddPrefix = true ) const = 0;

	// Converts a string to a spec, uses StringToSpec, if that fails
	// it looks the name up in the name table.
	virtual int		NameToSpec( LPCTSTR inString, TMediaSpecType inType, CMediaSpec& outSpec ) const = 0;

	// Converts a string to a spec, no name lookups.
	virtual bool	StringToSpec( LPCTSTR inString,
						TMediaSpecType inType, CMediaSpec& outSpec ) const = 0;

	// Get the type name for a spec.
	virtual void	SpecToTypeString( const CMediaSpec& inSpec, LPTSTR outString ) const = 0;

	// Get entry given index (which encodes sub-table and index).
	virtual bool	IndexToEntry( NameTableIndex inIndex, SNameTableEntry& outEntry ) const = 0;
	virtual bool	SpecToEntry( const CMediaSpec & inSpec, SNameTableEntry & outEntry ) const = 0;

	virtual UInt32	GetCount() const = 0;

	virtual bool	AddEntry( const SNameTableEntry& inEntry, UInt16 inResolveActions ) = 0;
	virtual bool	AddEntryWithCurrentOptions( const SNameTableEntry& inEntry ) = 0;

	virtual void	RemoveEntry( const CMediaSpec& inSpec ) = 0;
	virtual void	RemoveEntry( SInt32 inIndex, CNameSubTable* inTable ) = 0;

	virtual void	RemoveAllEntries() = 0;

	virtual CNameSubTable*	GetAddressSubTable() const  = 0;
	virtual CNameSubTable*	GetProtocolSubTable() const  = 0;
	virtual CNameSubTable*	GetPortSubTable() const  = 0;
	
	virtual CNameSubTable*	ClassToSubTable( const TMediaSpecClass inClass,
								const TMediaSpecType  inType = kMediaSpecType_Null ) const = 0;

	// Load/save.
	virtual void	LoadFromFile( const std::wstring & in_strFilePath ) = 0;
	virtual void	Load( const std::wstring& inPath ) = 0;
	virtual void	Load( const std::wstring& inPath, UInt32* pOutValidNames, 
						UInt32* pOutImported, UInt32* pOutSkipped ) = 0;
	virtual void	Save( const std::wstring& inPath ) = 0;	// not a const since it removes expired entries before saving
	virtual void	SaveToFile( const std::wstring & in_strFilePath ) = 0;

	// Do options dialog.
	virtual void	DoOptions() = 0;

	// Options accessors.
	virtual bool	IsPassiveResolverEnabled() const  = 0;
	virtual UInt8	GetPassiveResolveGroup() const  = 0;
	virtual void	SetPassiveResolve( bool inEnable ) = 0;

	virtual void	SyncOptions() = 0;

	virtual void	StartBroadcasting() = 0;
	virtual void	StopBroadcasting() = 0;

	virtual void	NamesImported() = 0;

	virtual void	CompressItems( bool inRemoveHidden = false ) = 0;
	
	virtual bool	NameTableIndexToSubTableAndIndex( NameTableIndex inIndex,
						CNameSubTable*& outSubTable, SInt32& outIndex ) const = 0;
	virtual bool	SubTableAndIndexToNameTableIndex( CNameSubTable* inSubTable,
						SInt32 inIndex, NameTableIndex& outIndex ) const = 0;

	virtual UInt32	GetChangeCounter() const = 0;
};
