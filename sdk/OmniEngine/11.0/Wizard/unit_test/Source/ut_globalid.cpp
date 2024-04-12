// ============================================================================
//	ut_globalid.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2010. All rights reserved.

#include "StdAfx.h"
#include "GlobalId.h"

int
UT_GlobalId()
{
	int		nFailures( 0 );

	CGlobalId	idInvalid;
	ASSERT( idInvalid.IsNotValid() );
	if ( idInvalid.IsValid() ) nFailures++;
	if ( idInvalid.IsSelfCreated() ) nFailures++;

	CGlobalId	idNull( GUID_NULL );
	ASSERT( idNull.IsNull() );
	if ( idNull.IsNotNull() ) nFailures++;
	if ( idNull.IsSelfCreated() ) nFailures++;

	CGlobalId	idNew( true );
	ASSERT( idNew.IsValid() );
	ASSERT( idNew.IsNotNull() );
	if ( idNew.IsNotValid() ) nFailures++;
	if ( idNew.IsNull() ) nFailures++;
	if ( idNew.IsNotSelfCreated() ) nFailures++;

	return nFailures;
}
