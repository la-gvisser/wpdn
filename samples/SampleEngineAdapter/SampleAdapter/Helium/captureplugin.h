// =============================================================================
//	captureplugin.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "stdafx.h"


/// \interface IPluginCapture
#define IPluginCapture_IID \
{ 0x67773F43, 0xD324, 0x45B5, {0x8D, 0x1D, 0x79, 0x55, 0x4E, 0xB9, 0x05, 0x5B} }

class HE_NO_VTABLE IPluginCapture : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPluginCapture_IID);

	HE_IMETHOD AddPluginCapture(const Helium::HeID& idCapture, void* pPlugin) = 0;
	HE_IMETHOD RemovePluginCapture(const Helium::HeID& idCapture) = 0;
	HE_IMETHOD GetPluginCapture(const Helium::HeID& idCapture, void* pPlugin) = 0;	
};

/// \interface IPluginCaptureOption
#define IPluginCaptureOption_IID \
{ 0x1CBD4ABF, 0x5ABC, 0x4B7E, { 0xB2, 0x00, 0x0C, 0x39, 0xFA, 0x67, 0x16, 0x74 } }

class HE_NO_VTABLE IPluginCaptureOption : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPluginCaptureOption_IID);

	HE_IMETHOD AddPluginCaptureOption(const Helium::HeID& idCapture) = 0;		
	HE_IMETHOD RemovePluginCaptureOption(const Helium::HeID& idCapture) = 0;
	HE_IMETHOD GetPluginCaptureOption(const Helium::HeID& idCapture, void* pOption) = 0;
	HE_IMETHOD SetPluginCaptureOption(const Helium::HeID& idCapture, void* pOption) = 0;
};

/// \interface ICaptureList
#define ICaptureList_IID \
  {	0xE40E2417, 0xD6A5, 0x4576, {0xA7, 0xEA, 0x90, 0x89, 0x2F, 0xF1, 0xF7, 0xC7} }

class HE_NO_VTABLE ICaptureList : public  Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICaptureList_IID);

	HE_IMETHOD AddCapture(/*in*/ IHeUnknown* pUnknown) = 0;
	HE_IMETHOD RemoveCapture(/*in*/ IHeUnknown* pUnknown) = 0;
};
