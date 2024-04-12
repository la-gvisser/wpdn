// ============================================================================
//	XBitField.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.

#ifndef XBITFIELD_H
#define XBITFIELD_H

#include "XObject.h"


//------------------------------------------

// {7D9B9411-3FC0-11d4-8874-0050DA087DA3}
X_DEFINE_GUID(XBitFieldID, 
0x7d9b9411, 0x3fc0, 0x11d4, 0x88, 0x74, 0x0, 0x50, 0xda, 0x8, 0x7d, 0xa3);

template<class X>  
class DECLSPEC XBitField : 
	virtual public XObject  
{
  public:
	XBitField() : m_iBits(0) {}
	virtual ~XBitField() {}

	X_BEGIN_ID_MAP(XBitField)
		X_ID_ENTRY(XBitFieldID)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP

	X_DECLARE_CREATE(XBitField)

	virtual X_RESULT Init()
	{
		m_iBits = 0;
		return X_OK;
	}

	virtual X_RESULT Reset()
	{
		m_iBits = 0;
		return X_OK;
	}
	
	virtual int TurnOn(X iMask)
	{
		m_iBits |= iMask;
		return X_OK;
	}

	virtual bool IsOn(X iMask)
	{
		return  (m_iBits & iMask) ? true : false;
	}
	
	virtual int TurnOff(X iMask)
	{
		m_iBits &= ~iMask;
		return X_OK;
	}
	
	virtual bool IsOff(X iMask)
	{
		return  (IsOn(iMask)==false) ? true : false;
	}

	virtual int SetState(X iMask, bool bState)
	{
		if (bState)
		{
			m_iBits |= iMask;		
		}
		else
		{
			m_iBits &= ~iMask;	
		}
		return X_OK;
	}

	virtual X GetBits()
	{
		return m_iBits;
	}
	
  protected:
	  
	X m_iBits;
};

typedef XBitField<UInt8>  XBitField8;
typedef XBitField<UInt16> XBitField16;
typedef XBitField<UInt32> XBitField32;
typedef XBitField<UInt64> XBitField64;

#endif
