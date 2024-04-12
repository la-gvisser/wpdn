// =============================================================================
//	PluginHandlersInterface.h
// =============================================================================
//	Copyright (c) 2012-2017 Savvius, Inc. All rights reserved.

#ifndef PLUGINHANDLERSINTERFACE_H
#define PLUGINHANDLERSINTERFACE_H

#define DlgPreferencesHandler_ID \
{ 0x47617279, 0x2044, 0x4C47, {0xB5, 0xCF, 0x29, 0xDF, 0xB6, 0x07, 0xE5, 0xCD} }

#define HTMLPreferencesHandler_ID \
{ 0x47617279, 0xABCD, 0x4CD9, {0xA3, 0x5C, 0x05, 0x58, 0xF2, 0xEB, 0xC2, 0xE2} }

#define CefPreferencesHandler_ID \
{ 0x47617279, 0x4365, 0x6620, {0x82, 0x56, 0xB3, 0x4A, 0x82, 0xF2, 0x93, 0x63} }

enum HandlerMessageId {
	kId_None,
	kId_About,
	kId_Options,
	kId_Tab,
	kId_Max
};

#include "hepushpack.h"

struct HandlerParam_Options {
	Helium::HEBSTR	fOriginal;
	Helium::HEBSTR	fUpdated;		// Allocate with SysAllocString().
} HE_PACK_STRUCT;
typedef struct HandlerParam_Options	HandlerParam_Options;

union HandlerParamBlock
{
	HandlerParam_Options	fOptions;

	HandlerParamBlock() {
		fOptions.fOriginal = nullptr;
		fOptions.fUpdated = nullptr;
	}
} HE_PACK_STRUCT;
typedef union HandlerParamBlock	HandlerParamBlock;

#include "hepoppack.h"

#endif /* PLUGINHANDLERSINTERFACE_H */
