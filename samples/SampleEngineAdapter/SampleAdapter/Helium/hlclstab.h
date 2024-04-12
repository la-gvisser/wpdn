// =============================================================================
//	hlclstab.h
// =============================================================================
//	Copyright (c) 2005-2008 WildPackets, Inc. All rights reserved.

#ifndef HLCLSTAB_H
#define HLCLSTAB_H

#include "hlbase.h"

namespace HeLib
{

/// \class CHeClassTableEntry
/// \brief An entry in a class table. Corresponds to _ATL_OBJMAP_ENTRY30.
struct CHeClassTableEntry
{
	Helium::HeCID			m_CID;
	HE_CREATORFUNC*			m_pfnGetClassObject;
	HE_CREATORFUNC*			m_pfnCreateInstance;
	UInt32					m_nReg;
};

/// \def HE_CLASS_TABLE_BEGIN
/// \brief Start a class table.
#define HE_CLASS_TABLE_BEGIN(TableName) \
static HeLib::CHeClassTableEntry TableName[] = \
{

/// \def HE_CLASS_TABLE_ENTRY
/// \brief Insert an entry in the class table.
#define HE_CLASS_TABLE_ENTRY(cid, class) \
	{ \
		cid, \
		class::_ClassFactoryCreatorClass::CreateInstance, \
		class::_CreatorClass::CreateInstance, \
		0 \
	},

/// \def HE_CLASS_TABLE_END
/// \brief End a class table.
#define HE_CLASS_TABLE_END() \
	{ HE_NULL_ID, 0, 0, 0 } \
};

/// \fn HeClassTableGetClassObject
/// \brief Get a class object from a class table.
inline HeResult
HeClassTableGetClassObject(
	HeLib::CHeClassTableEntry*	pTable, 
	const Helium::HeCID&		clsid, 
	const Helium::HeIID&		iid,
	void**						ppv )
{
	if ( ppv == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_E_NOT_AVAILABLE;

	*ppv = NULL;

	if ( pTable != NULL )
	{
		while (	!pTable->m_CID.IsNull() )
		{
			char szCId[50];
			pTable->m_CID.ToString(szCId,50);

			if ( pTable->m_CID.Equals( clsid ) )
			{
				if ( pTable->m_pfnGetClassObject != NULL )
				{
					hr = (*pTable->m_pfnGetClassObject)(
						reinterpret_cast<void*>(pTable->m_pfnCreateInstance), iid, ppv );
				}

				break;
			}

			++pTable;
		}
	}

	return hr;
}

} /* namespace HeLib */

#endif /* HLCLSTAB_H */
