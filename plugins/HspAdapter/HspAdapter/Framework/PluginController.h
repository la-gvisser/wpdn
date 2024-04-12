// =============================================================================
//	PluginController.h
// =============================================================================
//	Copyright (c) 2011-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "RemotePlugin.h"
#include "RemoteStatsPlugin.h"
#include <vector>


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
protected:
	std::unique_ptr<COmniPlugin>	m_spPlugin;
	CPeekArray<CRemotePlugin*>		m_ayOwners;
#ifdef IMPLEMENT_SUMMARY
	CPeekArray<CRemoteStatsPlugin*>	m_ayStatOwners;
#endif	// IMPLEMENT_SUMMARY

public:
	;		CPluginController() {}
	;		~CPluginController() {}

	void	Add( CRemotePlugin* inRemotePlugin ) {
		if ( !m_spPlugin ) {
			m_spPlugin = make_unique<COmniPlugin>();
		}
		m_ayOwners.AddUnique( inRemotePlugin );
	}

#ifdef IMPLEMENT_SUMMARY
	void	Add( CRemoteStatsPlugin* inRemotePlugin ) {
		if ( !m_spPlugin ) {
			m_spPlugin = make_unique<COmniPlugin>();
		}
		m_ayStatOwners.AddUnique( inRemotePlugin );
	}
#endif	// IMPLEMENT_SUMMARY

	bool	Find( const CGlobalId& inId, CRemotePlugin*& outRemotePlugin ) const {
		for ( auto itr = m_ayOwners.begin(); itr != m_ayOwners.end(); ++itr ) {
			if ( (*itr)->GetContextId() == inId ) {
				outRemotePlugin = *itr;
				return true;
			}
		}
		return false;
	}

#ifdef IMPLEMENT_SUMMARY
	bool	Find( const CGlobalId& inId, CRemoteStatsPlugin*& outRemotePlugin ) const {
		for ( auto itr = m_ayStatOwners.begin(); itr != m_ayStatOwners.end(); ++itr ) {
			if ( (*itr)->GetContextId() == inId ) {
				outRemotePlugin = *itr;
				return true;
			}
		}
		return false;
	}
#endif	// IMPLEMENT_SUMMARY

	COmniPlugin*	GetPlugin() const { return m_spPlugin.get(); }

	size_t	RemoveOwner( CRemotePlugin* inRemotePlugin ) {
		for ( auto itr = m_ayOwners.begin(); itr != m_ayOwners.end(); ++itr ) {
			if ( *itr == inRemotePlugin ) {
				m_ayOwners.erase( itr );
				break;
			}
		}
		return m_ayOwners.size();
	}
#ifdef IMPLEMENT_SUMMARY
	void	RemoveOwner( CRemoteStatsPlugin* inRemotePlugin ) {
		for ( auto itr = m_ayStatOwners.begin(); itr != m_ayStatOwners.end(); ++itr ) {
			if ( *itr == inRemotePlugin ) {
				m_ayStatOwners.erase( itr );
				break;
			}
		}
	}
#endif	// IMPLEMENT_SUMMARY
};
