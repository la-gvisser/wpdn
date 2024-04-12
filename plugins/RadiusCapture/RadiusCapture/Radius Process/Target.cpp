#include "StdAfx.h"
#include ".\target.h"
#include "Radius.h"
#include "RadiusContext.h"

extern CRadiusApp	theApp;

CTarget::~CTarget(void)
{
	if (m_pPacketWriter)
	{
		delete m_pPacketWriter;
		m_pPacketWriter = NULL;
	}
}


// -----------------------------------------------------------------------------
//		Init
// -----------------------------------------------------------------------------


int 
CTarget::InitWriter() 
{
	ASSERT( m_pOptions );
	if (m_pOptions == NULL) return -1;

	tIntegrityOptions theInitegrityOptions;
	m_pOptions->GetIntegrityOptions( theInitegrityOptions );

	m_pPacketWriter = new CPeekFileWriter( theInitegrityOptions );
	ASSERT(m_pPacketWriter);
	if (m_pPacketWriter == NULL)
	{
		return -1;
	}

	return 0;
}


// -----------------------------------------------------------------------------
//		StopCapture
// -----------------------------------------------------------------------------

int
CTarget::StopCapture()
{
	if (m_pPacketWriter->IsOpen())
	{
		m_pPacketWriter->Close();
	}

	return 0;
}
