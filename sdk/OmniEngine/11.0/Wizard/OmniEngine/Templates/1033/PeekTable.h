// =============================================================================
//	PeekTable.h
// =============================================================================
//	Copyright (c) 2012-2017 Savvius, Inc. All rights reserved.

#pragma once

#include "idatatable.h"

class CPeekTable;

static bool ReplicatePeekTable( CPeekTable& PeekTable, IDataTable* pDataTable );


// =============================================================================
//		CPeekTableColumn
// =============================================================================

class CPeekTableColumn
{
	friend class CPeekTableColumnCollection;

protected:
		CHePtr<IDataTableColumn>	m_spColumn;

		UInt16	GetDataType() {
			_ASSERTE( IsValid() );
			if ( IsNotValid() ) { return 0; }

			UInt16 nDataType( 0 );
			Peek::ThrowIfFailed( m_spColumn->GetDataType( &nDataType) );

			return nDataType;
		}


public:
	;		CPeekTableColumn() {}

	virtual ~CPeekTableColumn() {}

	bool		IsNotValid() const { return (m_spColumn == nullptr); }
	bool		IsValid() const { return (m_spColumn != nullptr); }


	bool		GetTableFromColumn( CPeekTable& PeekTable ) {
		_ASSERTE( IsValid() );
		if ( IsNotValid() )		return false;

		CHePtr<IDataTable>	spDataTable;
		Peek::ThrowIfFailed( m_spColumn->GetTable(  &spDataTable.p ) );

		ReplicatePeekTable( PeekTable, spDataTable );

		return true;
	}

	SInt32	GetColumnIndex() {
		_ASSERTE( IsValid() );
		if ( IsNotValid() )		return 0;

		SInt32	nIndex( 0 );
		Peek::ThrowIfFailed( m_spColumn->GetColumnIndex( &nIndex ) );

		return nIndex;
	}

	CPeekString	GetColumnName() {
		_ASSERTE( IsValid() );
		if ( IsNotValid() ) { return CPeekString(L""); }
		Helium::HEBSTR strColumnName;
		Peek::ThrowIfFailed( m_spColumn->GetColumnName( &strColumnName) );
		return (CPeekString)strColumnName;
	}
};


// =============================================================================
//		CPeekTableColumnCollection
// =============================================================================

class CPeekTableColumnCollection
{
	friend class CPeekTable;

protected:
	CHePtr<IDataTableColumnCollection>	m_spCols;


public:
	;		CPeekTableColumnCollection()
	{
	}

	virtual ~CPeekTableColumnCollection() {}

	bool		IsNotValid() const { return (m_spCols == nullptr); }
	bool		IsValid() const { return (m_spCols != nullptr); }

	SInt32		GetCount() {
		_ASSERTE( IsValid() );
		if ( IsNotValid() )		return 0;

		SInt32 nCount( 0 );
		Peek::ThrowIfFailed( m_spCols->GetCount( &nCount ) );
		return nCount;
	}

	bool GetTableColumn( SInt32 inIndex, CPeekTableColumn& outTableColumn ) {
		_ASSERTE( IsValid() );
		if ( IsNotValid() )		return false;

		CHePtr<IDataTableColumn>	m_spColumn;	
		Peek::ThrowIfFailed( m_spCols->GetItem( inIndex, reinterpret_cast<IDataTableColumn**>( &outTableColumn.m_spColumn ) ) );
		return true;
	}

	bool GetTableColFromName( const CPeekString inName, CPeekTableColumn& outTableColumn ) {
		_ASSERTE( IsValid() );
		if ( IsNotValid() ) { return false; }

		const wchar_t* sName( inName );
		CHePtr<IDataTableColumn>	spColumn;

		Peek::ThrowIfFailed( m_spCols->GetItemFromName( sName, &spColumn.p ) );

		outTableColumn.m_spColumn = spColumn;

		return true;
	}

	SInt32 GetIndexFromName( CPeekString inName ) {
		_ASSERTE( IsValid() );
		if ( IsNotValid() ) { return 0; }

		SInt32 nIndex( 0 );
		const wchar_t* sName( inName );
		Peek::ThrowIfFailed( m_spCols->GetColumnIndexFromName( sName, &nIndex ) );

		return nIndex;
	}

	bool Add( CPeekString inColumnName, UInt16 inDataType ) {
		_ASSERTE( IsValid() );
		if ( IsNotValid() ) { return false; }

		const wchar_t* sColumnName( inColumnName );
		Peek::ThrowIfFailed( m_spCols->Add( sColumnName, inDataType, nullptr ) );

		return true;
	}

	bool AddBoolCol( CPeekString inColumnName ) {
		return Add( inColumnName, Helium::HE_VT_BOOL );
	}

	bool AddStringCol( CPeekString inColumnName ) {
		return Add( inColumnName, Helium::HE_VT_BSTR );
	}

	bool AddUInt8Col( CPeekString inColumnName ) {
		return Add( inColumnName, Helium::HE_VT_UI1 );
	}

	bool AddUInt16Col( CPeekString inColumnName ) {
		return Add( inColumnName, Helium::HE_VT_UI2 );
	}

	bool AddUInt32Col( CPeekString inColumnName ) {
		return Add( inColumnName, Helium::HE_VT_UI4 );
	}

	bool AddUInt64Col( CPeekString inColumnName ) {
		return Add( inColumnName, Helium::HE_VT_UI8 );
	}

	bool AddSInt8Col( CPeekString inColumnName ) {
		return Add( inColumnName, Helium::HE_VT_I1 );
	}

	bool AddSInt16Col( CPeekString inColumnName ) {
		return Add( inColumnName, Helium::HE_VT_I2 );
	}

	bool AddSInt32Col( CPeekString inColumnName ) {
		return Add( inColumnName, Helium::HE_VT_I4 );
	}

	bool AddSInt64Col( CPeekString inColumnName ) {
		return Add( inColumnName, Helium::HE_VT_I8 );
	}
};


// =============================================================================
//		CPeekTableRow
// =============================================================================

class CPeekTableRow
{
	friend class CPeekTableRowCollection;

protected:
		CHePtr<IDataTableRow>	m_spRow;

		HeLib::CHeVariant		GetItem( const UInt16 inColumnNo ) {
			_ASSERTE( IsValid() );
			if ( IsNotValid() )		return 0;

			HeLib::CHeVariant varItemNo( inColumnNo );
			HeLib::CHeVariant outVar;
			Peek::ThrowIfFailed( m_spRow->GetItem( varItemNo, &outVar ) );

			return outVar;
		}

		HeLib::CHeVariant GetItemFromName( const CPeekString inName, Helium::HEVARTYPE inVarType ) {
			_ASSERTE( IsValid() );
			if ( IsNotValid() ) { return false; }

			HeLib::CHeVariant outVar;
			const wchar_t* sName( inName );

			Peek::ThrowIfFailed( m_spRow->GetItemFromName( sName, inVarType, &outVar ) );

			return outVar;
		}

		bool	SetItem( UInt16 inItem, HeLib::CHeVariant& inVar ) {
			_ASSERTE( IsValid() );
			if ( IsNotValid() )		return false;

			HeLib::CHeVariant varItemNo( inItem );
			Peek::ThrowIfFailed( m_spRow->SetItem( varItemNo, &inVar ) );

			return true;
		}

		bool	SetItemByName( const CPeekString inItemName, HeLib::CHeVariant& inVar ) {
			_ASSERTE( IsValid() );
			if ( IsNotValid() )		return false;

			HeLib::CHeVariant varItemNo( inItemName );
			Peek::ThrowIfFailed( m_spRow->SetItemByName( inItemName, &inVar ) );

			return true;
		}

public:
	;		CPeekTableRow()	{}

	virtual ~CPeekTableRow() {}

	bool		IsNotValid() const { return (m_spRow == nullptr); }
	bool		IsValid() const { return (m_spRow != nullptr); }

	SInt32		GetCount() {
		_ASSERTE( IsValid() );
		if ( IsNotValid() )		return 0;

		SInt32 nCount( 0 );
		Peek::ThrowIfFailed( m_spRow->GetCount( &nCount ) );
		return nCount;
	}

	void		 Clear() {
		_ASSERTE( IsValid() );
		if ( IsNotValid() )		return;

		Peek::ThrowIfFailed( m_spRow->Clear() );
	}

	bool		SetItem( const UInt16 inColumnNo, bool inItem ) {
		HeLib::CHeVariant var( inItem );
		return SetItem( inColumnNo, var );
	}

	bool		SetItem( const UInt16 inColumnNo, const wchar_t* inSzString ) {
		HeLib::CHeVariant var( inSzString );
		return SetItem( inColumnNo, var );
	}

	bool		SetItem( const UInt16 inColumnNo, const CPeekString &inString ) {
		HeLib::CHeVariant var( inString );
		return SetItem( inColumnNo, var );
	}

	bool		SetItem( const UInt16 inColumnNo, UInt8 inItem ) {
		HeLib::CHeVariant var( inItem );
		return SetItem( inColumnNo, var );
	}

	bool		SetItem( const UInt16 inColumnNo, UInt16 inItem ) {
		HeLib::CHeVariant var( inItem );
		return SetItem( inColumnNo, var );
	}

	bool		SetItem( const UInt16 inColumnNo, UInt32 inItem ) {
		HeLib::CHeVariant var( inItem );
		return SetItem( inColumnNo, var );
	}

	bool		SetItem( const UInt16 inColumnNo, UInt64 inItem ) {
		HeLib::CHeVariant var( inItem );
		return SetItem( inColumnNo, var );
	}

	bool		SetItem( const UInt16 inColumnNo, SInt8 inItem ) {
		HeLib::CHeVariant var( inItem );
		return SetItem( inColumnNo, var );
	}

	bool		SetItem( const UInt16 inColumnNo, SInt16 inItem ) {
		HeLib::CHeVariant var( inItem );
		return SetItem( inColumnNo, var );
	}

	bool		SetItem( const UInt16 inColumnNo, SInt32 inItem ) {
		HeLib::CHeVariant var( inItem );
		return SetItem( inColumnNo, var );
	}

	bool		SetItem( const UInt16 inColumnNo, SInt64 inItem ) {
		HeLib::CHeVariant var( inItem );
		return SetItem( inColumnNo, var );
	}

	bool		GetItem( const UInt16 inColumnNo, bool& outItem ) {
		outItem = false;
		HeLib::CHeVariant var = GetItem( inColumnNo );
		outItem = var.uiVal ? true : false;
		return true;
	}

	bool		GetItem( const UInt16 inColumnNo, CPeekString& outString ) {
		outString = outString.Empty();
		HeLib::CHeVariant var = GetItem( inColumnNo );
		if ( var.vt != Helium::HE_VT_BSTR ) return false;

		outString = var.bstrVal;
		return true;
	}

	bool		GetItem( const UInt16 inColumnNo, UInt8& outItem ) {
		outItem = 0;
		HeLib::CHeVariant var = GetItem( inColumnNo );
		outItem = static_cast<UInt8>( var.uiVal );
		return true;
	}

	bool		GetItem( const UInt16 inColumnNo, UInt16& outItem ) {
		outItem = 0;
		HeLib::CHeVariant var = GetItem( inColumnNo );
		outItem = var.uiVal;
		return true;
	}

	bool		GetItem( const UInt16 inColumnNo, UInt32& outItem ) {
		outItem = 0;
		HeLib::CHeVariant var = GetItem( inColumnNo );
		outItem = var.uiVal;
		return true;
	}

	bool		GetItem( const UInt16 inColumnNo, UInt64& outItem ) {
		outItem = 0;
		HeLib::CHeVariant var = GetItem( inColumnNo );
		outItem = var.ullVal;
		return true;
	}

	bool		GetItem( const UInt16 inColumnNo, SInt8& outItem ) {
		outItem = 0;
		HeLib::CHeVariant var = GetItem( inColumnNo );
		outItem = static_cast<UInt8>( var.uiVal );
		return true;
	}

	bool		GetItem( const UInt16 inColumnNo, SInt16& outItem ) {
		outItem = 0;
		HeLib::CHeVariant var = GetItem( inColumnNo );
		outItem = var.uiVal;
		return true;
	}

	bool		GetItem( const UInt16 inColumnNo, SInt32& outItem ) {
		outItem = 0;
		HeLib::CHeVariant var = GetItem( inColumnNo );
		outItem = var.uiVal;
		return true;
	}

	bool		GetItem( const UInt16 inColumnNo, SInt64& outItem ) {
		outItem = 0;
		HeLib::CHeVariant var = GetItem( inColumnNo );
		outItem = var.ullVal;
		return true;
	}


	bool		GetItemFromName( const CPeekString inName, CPeekString& outString ) {
		outString == outString.Empty();
		HeLib::CHeVariant var = GetItemFromName( inName, Helium::HE_VT_BSTR );
		if ( var.vt != Helium::HE_VT_BSTR ) return false;

		outString = var.bstrVal;
		return true;
	}

	bool		GetTableFromRow( CPeekTable& PeekTable ) {
		_ASSERTE( IsValid() );
		if ( IsNotValid() )		return false;

		CHePtr<IDataTable>			spDataTable;
		Peek::ThrowIfFailed( m_spRow->GetTable( reinterpret_cast<IDataTable**>( &spDataTable ) ) );

		ReplicatePeekTable( PeekTable, spDataTable );

		return true;
	}

	bool		SetItemByName( const CPeekString inItemName, const CPeekString inItemValue ) {
		HeLib::CHeVariant varString( inItemValue );
		return SetItemByName( inItemName, varString );
	}
};


// =============================================================================
//		CPeekTableRowCollection
// =============================================================================

class CPeekTableRowCollection
{
	friend class CPeekTable;

protected:
	CHePtr<IDataTableRowCollection>		m_spRows;


public:
	;		CPeekTableRowCollection() {}

	virtual ~CPeekTableRowCollection() {}

	bool		IsNotValid() const { return (m_spRows == nullptr); }
	bool		IsValid() const { return (m_spRows != nullptr); }

	SInt32		GetCount() {
		_ASSERTE( IsValid() );
		if ( IsNotValid() )		return 0;

		SInt32 nCount( 0 );
		Peek::ThrowIfFailed( m_spRows->GetCount( &nCount ) );
		return nCount;
	}

	bool GetTableRow( SInt32 inIndex, CPeekTableRow& outTableRow ) {
		_ASSERTE( IsValid() );
		if ( IsNotValid() )		return false;

		CHePtr<IDataTableRow>	m_spRow;	
		Peek::ThrowIfFailed( m_spRows->GetItem( inIndex, reinterpret_cast<IDataTableRow**>( &outTableRow.m_spRow ) ) );
		return true;
	}

	bool Add( CPeekTableRow& outTableRow ) {
		_ASSERTE( IsValid() );
		if ( IsNotValid() ) { return false; }

		CHePtr<IDataTableRow>	spRow;	
 		Peek::ThrowIfFailed( m_spRows->Add( &spRow.p ) );

		outTableRow.m_spRow = spRow;

		return true;
	}

	bool Clear() {
		_ASSERTE( IsValid() );
		if ( IsNotValid() ) { return false; }

		Peek::ThrowIfFailed( m_spRows->Clear() );

		return true;
	}

	bool Remove( CPeekTableRow& inTableRow ) {
		_ASSERTE( IsValid() );
		if ( IsNotValid() ) { return false; }

		Peek::ThrowIfFailed( m_spRows->Remove( inTableRow.m_spRow ) );

		return true;
	}

	bool RemoveAt( SInt32 sIndex ) {
		_ASSERTE( IsValid() );
		if ( IsNotValid() ) { return false; }

		Peek::ThrowIfFailed( m_spRows->RemoveAt( sIndex ) );

		return true;
	}
};


// =============================================================================
//		CPeekTable
// =============================================================================

class CPeekTable
{
	friend class CPeekTableColumn;
	friend class CPeekTableColumnCollection;
	friend class CPeekTableRowCollection;

private:

protected:
	CHePtr<IDataTable>			m_spDataTable;
	CPeekTableColumnCollection	m_Columns;
	CPeekTableRowCollection		m_Rows;

	;		CPeekTable( CPeekTable& inPeekTable ) {
		inPeekTable.m_spDataTable = m_spDataTable;
		inPeekTable.m_Columns = m_Columns;
		inPeekTable.m_Rows = m_Rows;
	}

	bool GetColumnCollection( CPeekTableColumnCollection &outColumnCollection) {
		_ASSERTE( IsValid() );
		if ( IsNotValid() )		return false;

		Peek::ThrowIfFailed( m_spDataTable->GetColumns( &outColumnCollection.m_spCols.p ) );

		return true;
	}

	bool GetRowCollection( CPeekTableRowCollection &outRowCollection) {
		_ASSERTE( IsValid() );
		if ( IsNotValid() )		return false;

		CHePtr<IDataTableRowCollection>	spRows;
		Peek::ThrowIfFailed( m_spDataTable->GetRows( &spRows.p ) );

		outRowCollection.m_spRows = spRows;

		return true;
	}

public:
	;		CPeekTable( bool bCreateInstance = true ) {
		if ( bCreateInstance ) {
			CHePtr<IDataTable>	spDataTable;

			Peek::ThrowIfFailed( m_spDataTable.CreateInstance( "PeekUtil.DataTable" ) );
			Peek::ThrowIf( IsNotValid() );	
			GetColumnCollection( m_Columns );
			GetRowCollection( m_Rows );
		}
	}

	virtual ~CPeekTable() {}

	bool		IsNotValid() const { return (m_spDataTable == nullptr); }
	bool		IsValid() const { return (m_spDataTable != nullptr); }

	void		ReplicateFromPtr( CPeekTable& PeekTable, IDataTable* pDataTable ) {
		PeekTable.m_spDataTable = pDataTable;
		PeekTable.GetColumnCollection( m_Columns );
		PeekTable.GetRowCollection( m_Rows );
	}

	CPeekString GetTableName() {
		_ASSERTE( IsValid() );
		if ( IsNotValid() ) { return CPeekString(L""); }
		Helium::HEBSTR strHeTableName;
		Peek::ThrowIfFailed( m_spDataTable->GetTableName( &strHeTableName) );
		return (CPeekString)strHeTableName;
	}

	void SetTableName( CPeekString inTableName ) {
		_ASSERTE( IsValid() );
		if ( IsNotValid() )		return;
		CHeBSTR	bstrTableName = inTableName;
		Peek::ThrowIfFailed( m_spDataTable->SetTableName( bstrTableName ) );
	}

	void Clear() {
		_ASSERTE( IsValid() );
		if ( IsNotValid() )		return;

		Peek::ThrowIfFailed( m_spDataTable->Clear() );
	}

	void Merge( CPeekTable& inOther ) {
		_ASSERTE( IsValid() && inOther.IsValid());
		if ( IsNotValid() || inOther.IsNotValid() )		return;
		Peek::ThrowIfFailed( m_spDataTable->Merge( inOther.m_spDataTable ) );
	}

	CPeekTableColumnCollection& GetColumnCollection() {
		_ASSERTE( IsValid() );
		return m_Columns;
	}

	CPeekTableRowCollection& GetRowCollection() {
		_ASSERTE( IsValid() );
		return m_Rows;
	}
};


// =============================================================================
//		ReplicatePeekTable
// =============================================================================

static bool ReplicatePeekTable( CPeekTable& PeekTable, IDataTable* pDataTable )
{
	PeekTable.ReplicateFromPtr( PeekTable, pDataTable );
	return true;
}
