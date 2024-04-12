// =============================================================================
//	PluginController.h
// =============================================================================
//	Copyright (c) 2011-2015 Savvius, Inc. All rights reserved.

#pragma once

#include <vector>

#ifdef WP_LINUX
class	IConsoleSend;
#endif // WP_LINUX


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Plugin Controller
//
//	The Plugin Controller is a Framework object that binds Omni Plugin objects
//	to a list of Remote Plugins. The Omni Plugin is linked/associated to the
//	Remote Plugin by a common IConsoleSend address.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		CPluginController
// =============================================================================

class
CPluginController
	:	public HeLib::CHeObjRoot<HeLib::CHeMultiThreadModel>
{
	typedef struct {
		COmniPluginPtr				fPlugin;
		CPeekArray<CRemotePlugin*>	fOwners;
	} tItem;

protected:
	CPeekArray<tItem>	m_ayPlugins;

protected:
	size_t	InternalAdd( IConsoleSend* inConsoleSend, COmniPluginPtr& outPlugin ) {
		tItem	item = { COmniPluginPtr( new COmniPlugin ) };
		size_t	nIndex = m_ayPlugins.Add( item );
		outPlugin = m_ayPlugins[nIndex].fPlugin;
		_ASSERTE( outPlugin.IsValid() );
		(void)inConsoleSend;
		return nIndex;
	}

	bool	InternalFind( IConsoleSend* /*inConsoleSend*/, size_t& outIndex ) const {
		size_t	nCount = m_ayPlugins.GetCount();
		if ( nCount > 0 ) {
			COmniPluginPtr	spPlugin = m_ayPlugins[0].fPlugin;
			if ( spPlugin.IsValid() ) {
				outIndex = 0;
				return true;
			}
		}
		return false;
	}
	bool	InternalFind( const CGlobalId& inId, size_t& outIndex ) const {
		size_t	nCount = m_ayPlugins.GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			const CPeekArray<CRemotePlugin*>&	ayOwners = m_ayPlugins[i].fOwners;
			size_t	nOwners = ayOwners.GetCount();
			for ( size_t n = 0; n < nOwners; n++ ) {
				const CRemotePlugin*	pOwner = ayOwners[n];
				if ( pOwner->GetContextId() == inId ) {
					outIndex = i;
					return true;
				}
			}
		}
		return false;
	}
	bool	InternalFindOwner( size_t inIndex, const CGlobalId& inId,
				CRemotePlugin*& outRemotePlugin ) const {
		tItem	item = m_ayPlugins[inIndex];
		size_t	nCount = item.fOwners.GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CRemotePlugin*	pOwner = item.fOwners[i];
			if ( pOwner->GetContextId() == inId ) {
				outRemotePlugin = item.fOwners[i];
				return true;
			}
		}
		return false;
	}

public:
	;		CPluginController() {}
	;		~CPluginController() {}

	void	Add( IConsoleSend* inConsoleSend, CRemotePlugin* inRemotePlugin,
		COmniPluginPtr& outPlugin ) {
		size_t	nIndex( kIndex_Invalid );
		if ( !InternalFind( inConsoleSend, nIndex ) ) {
			nIndex = InternalAdd( inConsoleSend, outPlugin );
		}
		m_ayPlugins[nIndex].fOwners.AddUnique( inRemotePlugin );
	}
	void	AddOwner( IConsoleSend* inConsoleSend, CRemotePlugin* inRemotePlugin ) {
		size_t	nIndex( kIndex_Invalid );
		if ( InternalFind( inConsoleSend, nIndex ) ) {
			m_ayPlugins[nIndex].fOwners.AddUnique( inRemotePlugin );
		}
	}

	bool	Find( IConsoleSend* inConsoleSend, COmniPluginPtr& outPlugin ) const {
		size_t	nIndex( kIndex_Invalid );
		if ( InternalFind( inConsoleSend, nIndex ) ) {
			outPlugin = m_ayPlugins[nIndex].fPlugin;
			return true;
		}
		return false;
	}
	bool	Find( IConsoleSend* inConsoleSend, const CGlobalId& inId,
				CRemotePlugin*& outRemotePlugin ) const {
		size_t	nIndex( kIndex_Invalid );
		if ( InternalFind( inConsoleSend, nIndex ) ) {
			return InternalFindOwner( nIndex, inId, outRemotePlugin );
		}
		return false;
	}
	bool	Find( const CGlobalId& inId, COmniPluginPtr& outPlugin ) const {
		size_t	nIndex( kIndex_Invalid );
		if ( InternalFind( inId, nIndex ) ) {
			outPlugin = m_ayPlugins[nIndex].fPlugin;
			return true;
		}
		return false;
	}
	bool	FindFirst( COmniPluginPtr& outPlugin ) {
		size_t	nCount = m_ayPlugins.GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			COmniPluginPtr	spPlugin = m_ayPlugins[i].fPlugin;
			if ( spPlugin.IsValid() && spPlugin->IsInitialized() ) {
				outPlugin = spPlugin;
				return true;
			}
		}
		return false;
	}

#ifdef _WIN32
	bool	HasOwnerOwner( IConsoleSend* inConsoleSend ) {
		size_t	nIndex( kIndex_Invalid );
		if ( InternalFind( inConsoleSend, nIndex ) ) {
			return !m_ayPlugins[nIndex].fOwners.IsEmpty();
		}
		return false;
	}

	void	Remove( IConsoleSend* inConsoleSend ) {
		size_t	nIndex( kIndex_Invalid );
		if ( InternalFind( inConsoleSend, nIndex ) ) {
			m_ayPlugins.RemoveAt( nIndex );
		}
	}
	void	RemoveOwner( IConsoleSend* inConsoleSend, CRemotePlugin* inRemotePlugin ) {
		size_t	nIndex( kIndex_Invalid );
		if ( InternalFind( inConsoleSend, nIndex ) ) {
			m_ayPlugins[nIndex].fOwners.Remove( inRemotePlugin );
		}
	}
#endif
};
