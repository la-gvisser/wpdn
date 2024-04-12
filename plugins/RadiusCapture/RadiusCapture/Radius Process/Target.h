#pragma once

#include "Options.h"
#include "RadiusStats.h"
#include "LogQueue.h"
#include "PeekFileWriter.h"

class CRadiusContext;

class CTarget
{
public:
	CTarget(void) 
	{
		m_pOptions = NULL;
		m_pContext = NULL;
		m_pPacketWriter = NULL;
	}

	~CTarget(void);

	X_DECLARE_PTR( COptions, Options );
	X_DECLARE_PTR( CRadiusContext,	Context );
	X_DECLARE_PTR( CPeekFileWriter, PacketWriter );

	int InitWriter();
	int StopCapture();
};
