// ============================================================================
//	XDebugger.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.

#ifndef XDEBUGGER_H
#define XDEBUGGER_H

#include "XObject.h"

// {9550CEBC-6D39-4ec6-AD60-F66394724787}
X_DEFINE_GUID(XDebuggerID, 
0x9550cebc, 0x6d39, 0x4ec6, 0xad, 0x60, 0xf6, 0x63, 0x94, 0x72, 0x47, 0x87);

class XDebugger : virtual public XObject
{
public:

	X_BEGIN_ID_MAP(XDebugger)
		X_ID_ENTRY(XDebuggerID)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP

	virtual void Break()=0;
	virtual void CreateThread( LPVOID pParam )=0;
	virtual bool IsFinished()=0;
	virtual void AdjustStack( const char* pName, int nStackDepth )=0;
	virtual void IncrFunctions()=0;
	virtual void IncrInstructions()=0;
	virtual void Log( const char* pString )=0;
	virtual void Wait()=0;
};

#endif 
