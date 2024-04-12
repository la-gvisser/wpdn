// ============================================================================
//	main.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2010. All rights reserved.

// Unit Tests

#include "StdAfx.h"
#include <stdio.h>

extern int UT_GlobalId();
extern int UT_PeekStrings();
extern int UT_PeekTime();
extern int UT_ByteVectRef();
extern int UT_IpAddress();
extern int UT_Xml();
extern int UT_FilePath();

int
main(
	int		argc,
	char*	argv[] )
{
	for ( int i = 0; i < argc; i++ ) {
		printf( "%s ", argv[i] );
	}
	printf( "\n" );

	int	nFailures( 0 );
	nFailures = UT_GlobalId();
	ASSERT( nFailures == 0 );

	nFailures = UT_PeekStrings();
	ASSERT( nFailures == 0 );

	nFailures = UT_PeekTime();
	ASSERT( nFailures == 0 );

	nFailures = UT_ByteVectRef();
	ASSERT( nFailures == 0 );

	nFailures = UT_IpAddress();
	ASSERT( nFailures == 0 );

	//nFailures = UT_Xml();
	//ASSERT( nFailures == 0 );

	nFailures = UT_FilePath();
	ASSERT( nFailures == 0 );

	return 0;
}
