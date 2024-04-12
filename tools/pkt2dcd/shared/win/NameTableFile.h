// =============================================================================
//	NameTableFile.h
// =============================================================================
//	Copyright (c) 2000-2007 WildPackets, Inc. All rights reserved.
//	Copyright (c) 1999-2000 AG Group, Inc. All rights reserved.

#ifndef NAMETABLEFILE_H
#define NAMETABLEFILE_H

class CByteStream;
class CNameTable;
class CNameSubTable;
struct SNameTableEntry;

namespace NameTableFile
{

	enum NameTableColumn
	{
		kNameTableColumn_ID,
		kNameTableColumn_Name,
		kNameTableColumn_Type,
		kNameTableColumn_Entry,
		kNameTableColumn_Color,
		kNameTableColumn_Group,
		kNameTableColumn_LastModDate,
		kNameTableColumn_LastUsedDate,
		kNameTableColumn_NodeType,
		kNameTableColumn_ResolveType,
		kNameTableColumn_ResolveSource,
		kNameTableColumn_Trust
	};

	bool	NameTableExportText( const CNameTable* inNameTable, 
				CByteStream& outNameTableStream, const TCHAR inSeparator,
				const NameTableColumn* pColumns, const int nColumns );
    bool    NameTableExportSelectedText( const CNameSubTable* inSubTable,
				UInt32* inEntryArray, UInt32 inEntryCount, CByteStream& outNameTableStream,
				const TCHAR inSeparator );
	bool	NameTableExportXML( const CNameTable* inNameTable, 
				CByteStream& outNameTableStream );
	bool	NameTableExportSelectedXML( const CNameSubTable* inSubTable,
				UInt32* inEntryArray, UInt32 inEntryCount, CByteStream& outNameTableStream );
	bool	NameTableImportText( const CByteStream& inNameTableStream, 
				CNameTable* inNameTable, UInt32& outValidNames, 
				UInt32& outImported, UInt32& outSkipped );
	bool	NameTableImportXML( const CByteStream& inNameTableStream, 
				CNameTable* inNameTable, UInt32& outValidNames, UInt32& outImported, 
				UInt32& outSkipped );
}

#endif
