// =============================================================================
//	idatatable.h
// =============================================================================
//	Copyright (c) 2005-2015 Savvius, Inc. All rights reserved.

#ifndef	IDATATABLE_H
#define	IDATATABLE_H

#include "heunk.h"

class IDataTable;

#define IDataTableColumn_IID \
{ 0x2FFDB158, 0xF401, 0x4E19, {0x81, 0xFD, 0xCD, 0x62, 0x38, 0xB5, 0x7A, 0x57} }

class HE_NO_VTABLE IDataTableColumn : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IDataTableColumn_IID)

	HE_IMETHOD GetTable(/*out*/ IDataTable** ppTable) = 0;
	HE_IMETHOD GetColumnIndex(/*out*/ SInt32* pIndex) = 0;
	HE_IMETHOD GetColumnName(/*out*/ Helium::HEBSTR* pbstrName) = 0;
	HE_IMETHOD GetDataType(/*out*/ Helium::HEVARTYPE* pvt) = 0;
};

#define IDataTableColumnCollection_IID \
{ 0x9D21CA86, 0xFD4E, 0x48AD, {0xB5, 0x35, 0x1C, 0x69, 0x4A, 0x79, 0xAE, 0x46} }

class HE_NO_VTABLE IDataTableColumnCollection : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IDataTableColumnCollection_IID)

	HE_IMETHOD GetCount(/*out*/ SInt32* pcColumns) = 0;
	HE_IMETHOD GetItem(/*in*/ SInt32 index, /*out*/ IDataTableColumn** ppColumn) = 0;
	HE_IMETHOD GetItemFromName(/*in*/ const wchar_t* pszName, /*out*/ IDataTableColumn** ppColumn) = 0;
	HE_IMETHOD GetColumnIndexFromName(/*in*/ const wchar_t* pszName, /*out*/ SInt32* pIndex) = 0;
	HE_IMETHOD Add(/*in*/ const wchar_t* pszColumnName, 
		/*in*/ Helium::HEVARTYPE dataType, /*out*/ IDataTableColumn** ppColumn) = 0;
};

#define IDataTableRow_IID \
{ 0x871592E3, 0x42F2, 0x4D44, {0xB1, 0xD7, 0xD6, 0x96, 0x94, 0x4D, 0x1D, 0xA8} }

class HE_NO_VTABLE IDataTableRow : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IDataTableRow_IID)

	HE_IMETHOD GetTable(/*out*/ IDataTable** ppTable) = 0;
	HE_IMETHOD GetCount(/*out*/ SInt32* pcValues) = 0;
	HE_IMETHOD GetItem(/*in*/ Helium::HEVARIANT varIndex, /*out*/ Helium::HEVARIANT* pVal) = 0;
	HE_IMETHOD SetItem(/*in*/ Helium::HEVARIANT varIndex, /*in*/ const Helium::HEVARIANT* pVal) = 0;
	HE_IMETHOD GetItemFromName(/*in*/ const wchar_t* pszName, /*in*/ Helium::HEVARTYPE vt, /*out*/ Helium::HEVARIANT* pVal) = 0;
	HE_IMETHOD SetItemByName(/*in*/ const wchar_t* pszName, /*in*/ const Helium::HEVARIANT* pVal) = 0;
	HE_IMETHOD Clear() = 0;
};

#define IDataTableRowCollection_IID \
{ 0x696A7CFF, 0x7DAD, 0x490A, {0xAF, 0xA5, 0x73, 0x75, 0x08, 0x18, 0xA3, 0x04} }

class HE_NO_VTABLE IDataTableRowCollection : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IDataTableRowCollection_IID)

	HE_IMETHOD GetCount(/*out*/ SInt32* pcRows) = 0;
	HE_IMETHOD GetItem(/*in*/ SInt32 index, /*out*/ IDataTableRow** ppRow) = 0;
	HE_IMETHOD Add(/*out*/ IDataTableRow** ppRow) = 0;
	HE_IMETHOD Remove(/*in*/ IDataTableRow* pRow) = 0;
	HE_IMETHOD RemoveAt(/*in*/ SInt32 index) = 0;
	HE_IMETHOD Clear() = 0;
};

#define IDataTable_IID \
{ 0x0EC9C4EA, 0x1A36, 0x4259, {0xBC, 0xB7, 0x2B, 0xF7, 0xC3, 0xED, 0xBC, 0xA1} }

class HE_NO_VTABLE IDataTable : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IDataTable_IID)

	HE_IMETHOD GetTableName(/*out*/ Helium::HEBSTR* pbstrTableName) = 0;
	HE_IMETHOD SetTableName(/*in*/ Helium::HEBSTR bstrTableName) = 0;
	HE_IMETHOD GetColumns(/*out*/ IDataTableColumnCollection** ppColumns) = 0;
	HE_IMETHOD GetRows(/*out*/ IDataTableRowCollection** ppRows) = 0;
	HE_IMETHOD Clear() = 0;
	HE_IMETHOD Merge(/*in*/ IDataTable* pDataTable) = 0;
};

#endif /* IDATATABLE_H */
