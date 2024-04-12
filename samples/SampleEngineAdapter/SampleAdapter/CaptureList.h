#pragma once
#include "stdafx.h"

#define ICaptureList_IID \
  {	0xE40E2417, 0xD6A5, 0x4576, { 0xA7, 0xEA, 0x90, 0x89, 0x2F, 0xF1, 0xF7, 0xC7 } }

class HE_NO_VTABLE ICaptureList : public  Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICaptureList_IID);

	HE_IMETHOD AddCapture(IHeUnknown* pUnknown)=0;
	HE_IMETHOD RemoveCapture(IHeUnknown* pUnknown)=0;
};

