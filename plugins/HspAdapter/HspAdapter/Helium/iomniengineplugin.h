// =============================================================================
//	iomniengineplugin.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#ifndef IOMNIENGINEPLUGIN_H
#define IOMNIENGINEPLUGIN_H

#include "heunk.h"
#include "hevariant.h"
#include "heid.h"
#include "hecom.h"
#include "MediaTypes.h"

class ICefUIHandler;
class ICefUIClient;

/// Engine-side plugins implement this COM category.
#define Category_OmniEnginePlugin \
{ 0x4C116C92, 0x7B4C, 0x4C93, {0xB5, 0x86, 0xB8, 0x75, 0x70, 0x86, 0x23, 0x37} }

/// Console-side plugins implement this COM category.
#define Category_OmniConsolePlugin \
{ 0xCCF7D60E, 0x46ED, 0x4323, {0xAC, 0x9A, 0x03, 0xB8, 0x54, 0xD1, 0x7D, 0x4E} }

/// Service ID for engine-side plugins.
#define OmniEnginePlugins_SID \
{ 0x0A4C7DC5, 0xC2D1, 0x472B, {0x90, 0x06, 0x83, 0x6C, 0xB1, 0x58, 0xD5, 0xAD} }

/// Interface (internal use only) for engine's Load/Unload of the collection
#define IEnginePluginCollection_IID \
{ 0x294AC6EF, 0x5353, 0x4ed6, {0xA8, 0x76, 0x74, 0x99, 0x82, 0x71, 0x2E, 0xC8} }

class HE_NO_VTABLE IEnginePluginCollection : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IEnginePluginCollection_IID);

	HE_IMETHOD Load() = 0;
	HE_IMETHOD Unload() = 0;
};

/// Interface for receiving notifications when captures are created, etc.
#define IEnginePluginCaptureEvents_IID \
{ 0xC19D52EF, 0x0242, 0x491A, {0xB8, 0x41, 0x3F, 0x4D, 0xC4, 0xDF, 0xC0, 0xC9} }

class HE_NO_VTABLE IEnginePluginCaptureEvents : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IEnginePluginCaptureEvents_IID);

	HE_IMETHOD OnCaptureCreated(IHeUnknown* pCapture) = 0;
	HE_IMETHOD OnCaptureDeleted(IHeUnknown* pCapture) = 0;
};

// The following interfaces are for use by the console or a console side plugin.
// The IPlugin* interfaces should be implemented by the console side plugin.
// The IConsole* and IEngineInfo interfaces are implemented by the console.

/// \internal
/// \interface IPluginRecv
/// \brief Used by a console to inform plugin of message received result
#define IPluginRecv_IID \
{ 0x58CE004C, 0xC917, 0x4BD2, {0xBD, 0x65, 0xB9, 0x5E, 0xF4, 0x6E, 0x66, 0x75} }

class HE_NO_VTABLE IPluginRecv : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPluginRecv_IID);

	HE_IMETHOD Recv(/*in*/ HeResult hrMsgResult, /*in*/ UInt32 dwTransID, /*in*/ Helium::IHeStream* pStream) = 0;
};

#ifdef _WIN32

// IConsoleSend::GetFileList flags.
enum {
	HE_FLAG_RECURSIVE	= 0x0001,
	HE_FLAG_INCLUDEDIRS	= 0x0002,
	HE_FLAG_ABSOLUTE	= 0x0004,
	HE_FLAG_DRIVELIST	= 0x0010
};

/// \internal
/// \interface IConsoleSend
/// \brief Used by a plugin to communicate to the engine (via console's connection)
#define IConsoleSend_IID \
{ 0x9854599D, 0xF7F0, 0x4632, {0xA6, 0x1A, 0xC2, 0xE2, 0x50, 0xD4, 0x08, 0x9B} }

class HE_NO_VTABLE IConsoleSend : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IConsoleSend_IID);

	HE_IMETHOD Send(/*in*/ const Helium::HeCID& pluginID, /*in*/ IPluginRecv* pRecv, 
		/*in*/ UInt32 cbMsgLen, /*in*/ UInt8* pMsg, 
		/*in*/ UInt32 nTimeout, /*out*/ UInt32* pdwTransID ) = 0;

	HE_IMETHOD SendPluginMessage(/*in*/ const Helium::HeCID& pluginID,
		/*in*/ const Helium::HeID& captureID, /*in*/ IPluginRecv* pRecv,
		/*in*/ UInt32 cbMsgLen, /*in*/ UInt8* pMsg, /*in*/ UInt32 nTimeout,
		/*out*/ UInt32* pdwTransID ) = 0;

	HE_IMETHOD SendPluginOmniCommand(/*in*/ UInt32 cmdId, /*in*/ IPluginRecv* pRecv, 
		/*in*/ UInt32 cbMsgLen, /*in*/ UInt8* pMsg, 
		/*in*/ UInt32 nTimeout, /*out*/ UInt32* pdwTransID ) = 0;
};

/// \internal
/// \interface IConsoleUI
/// \brief Used by a plugin to manage UI
#define IConsoleUI_IID \
{ 0x577279F5, 0x68A6, 0x4EA0, {0x9C, 0xBC, 0xB8, 0xA7, 0x4A, 0x3F, 0x94, 0xAE} }

class HE_NO_VTABLE IConsoleUI : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IConsoleUI_IID);

	HE_IMETHOD AddTabWindow(/*in*/ Helium::HEBSTR bstrInsertPoint, /*in*/ BOOL bInsertAfter,
		/*in*/ Helium::HEBSTR bstrCmd, /*in*/ Helium::HEBSTR bstrWindowClass, 
		/*in*/ Helium::HEBSTR bstrWindowName, /*out*/ HWND* pWnd) = 0;
	HE_IMETHOD AddTabControl(/*in*/ Helium::HEBSTR bstrInsertPoint, /*in*/ BOOL bInsertAfter,
		/*in*/ Helium::HEBSTR bstrCmd, /*in*/ Helium::HEBSTR bstrControlClass, 
		/*in*/ Helium::HEBSTR bstrWindowName, /*out*/ IUnknown** ppUnk) = 0;
	HE_IMETHOD DoRefresh(/*in*/ Helium::HEBSTR bstrTab) = 0;
	HE_IMETHOD DoSelectPacketsByTimeAndFilter(/*in*/ UInt64 nStartTimestamp, /*in*/ UInt64 nStopTimestamp,
		/*in*/ Helium::HEBSTR bstrFilterString, /*in*/ BOOL bReplaceSelection, /*in*/ BOOL bShowResults ) = 0;
	HE_IMETHOD DoForensicSearch(/*in*/ UInt64 nStartTimestamp, /*in*/ UInt64 nStopTimestamp, /*in*/ Helium::HEBSTR bstrFilterString ) = 0;
	HE_IMETHOD GetMediaInfo(/*out*/ TMediaType* pmt, /*out*/ TMediaSubType* pmst) = 0;
	HE_IMETHOD CreateCefWindow(/*in*/ HWND hWnd, /*in*/ Helium::HEBSTR bstrURL, /*in*/ ICefUIHandler* pUIHandler, /*out*/ ICefUIClient** ppClient) = 0;
};

/// \internal
/// \interface IPluginUI
/// \brief Implemented by plugins that run UI for an Engine's' view
#define IPluginUI_IID \
{ 0xAD1C96C6, 0xEB4F, 0x4123, {0xB9, 0xAB, 0x16, 0xCE, 0xFD, 0x87, 0xD9, 0x83} }

class HE_NO_VTABLE IPluginUI : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPluginUI_IID);

	HE_IMETHOD SetEngine(/*in*/ Helium::IHeUnknown* pEngine) = 0;
	HE_IMETHOD SetConsoleUI(/*in*/ IConsoleUI* pUI) = 0;
	HE_IMETHOD GetUIInfo(/*out*/ Helium::HEBSTR* pbstrCommand, /*out*/ Helium::HEBSTR* pbstrText) = 0;
	HE_IMETHOD LoadView() = 0;
	HE_IMETHOD UnloadView() = 0;
};

// \internal
// \interface IEngineInfo
// \brief Implemented by engine connection to serve info about the connection
#define IEngineInfo_IID \
{ 0x8D870923, 0x22C6, 0x4352, {0x9D, 0x74, 0x0A, 0x2D, 0x2D, 0xA4, 0xF8, 0x21} }

class HE_NO_VTABLE IEngineInfo : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IEngineInfo_IID);

	HE_IMETHOD GetAddress(/*out*/ Helium::HEBSTR* pbstrAddress) = 0;
	HE_IMETHOD GetPort(/*out*/ UInt16* pnPort) = 0;
	HE_IMETHOD GetAuthentication(/*out*/ Helium::HEBSTR* pbstrAuth) = 0;
	HE_IMETHOD GetDomain(/*out*/ Helium::HEBSTR* pbstrDomain) = 0;
	HE_IMETHOD GetUserName(/*out*/ Helium::HEBSTR* pbstrUserName) = 0;
	HE_IMETHOD GetName(/*out*/ Helium::HEBSTR* pbstrName) = 0;
	HE_IMETHOD GetEngineType(/*out*/ Helium::HEBSTR* pbstrEngineType) = 0;
	HE_IMETHOD GetOperatingSystem(/*out*/ Helium::HEBSTR* pbstrOperatingSystem) = 0;
	HE_IMETHOD GetDataFolder(/*out*/ Helium::HEBSTR* pbstrDataFolder) = 0;
};

#endif /* _WIN32 */

#endif /* IOMNIENGINEPLUGIN_H */
