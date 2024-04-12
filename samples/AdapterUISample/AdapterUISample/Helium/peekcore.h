// =============================================================================
//	peekcore.h
// =============================================================================
//	Copyright (c) 2001-2015 Savvius, Inc. All rights reserved.

#ifndef PEEKCORE_H
#define PEEKCORE_H

#include "heunk.h"
#include "hevariant.h"
#include "heid.h"
#include "hecom.h"
#include "MediaTypes.h"
#include "PeekPacket.h"
#include "peeknotify.h"

class IPacket;
class IPacketBuffer;
class IAdapter;
class IConsoleSend;

/// \internal
/// \brief Service ID for configuration properties (not the same as AppConfig below).
#define Config_SID \
{ 0xB3DB807A, 0x4A78, 0x4CE5, {0xB5, 0x67, 0x73, 0xE9, 0x4A, 0xC3, 0x36, 0x9E} }

/// \internal
/// \interface IAppConfig
/// \brief Application configuration information.
#define IAppConfig_IID \
{ 0xC40503E0, 0xCDFF, 0x4249, {0x94, 0x31, 0x6E, 0x09, 0x2F, 0x6C, 0x0B, 0xC6} }

class HE_NO_VTABLE IAppConfig : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IAppConfig_IID);

	/// Location of configuration files. For example, 
	/// C:\Documents and Settings\All Users\Application Data\Savvius\OmniEngine\,
	/// or /etc/omni/.
	HE_IMETHOD GetConfigDir(/*out*/ Helium::HEBSTR* pbstrDir) = 0;
	/// Location of shared libraries. For example, 
	/// C:\Program Files\Savvius\OmniEngine\, or /usr/lib/omni/.
	HE_IMETHOD GetLibDir(/*out*/ Helium::HEBSTR* pbstrDir) = 0;
	/// Location of application resources. For example, 
	/// C:\Program Files\Savvius\OmniEngine\, or /var/lib/omni/.
	HE_IMETHOD GetResourceDir(/*out*/ Helium::HEBSTR* pbstrDir) = 0;
	/// Location of documentation such as help or man files. For example, 
	/// C:\Program Files\Savvius\OmniEngine\, or /usr/shared/doc/omni.
	HE_IMETHOD GetDocDir(/*out*/ Helium::HEBSTR* pbstrDir) = 0;
	/// Location of log files. For example, 
	/// C:\Documents and Settings\All Users\Application Data\Savvius\OmniEngine\,
	/// or /var/log/.
	HE_IMETHOD GetLogDir(/*out*/ Helium::HEBSTR* pbstrDir) = 0;
	/// Location of application data. For example, 
	/// C:\Documents and Settings\All Users\Application Data\Savvius\OmniEngine\,
	/// or /home/root/omni/.
	HE_IMETHOD GetDataDir(/*out*/ Helium::HEBSTR* pbstrDir) = 0;
	/// List of storage locations for application data. Absolute paths separated
	/// by a semicolon.
	HE_IMETHOD GetDataDirList(/*out*/ Helium::HEBSTR* pbstrDirList) = 0;
	/// Full path to the application database.
	HE_IMETHOD GetDatabasePath(/*out*/ Helium::HEBSTR* pbstrPath) = 0;
	/// Full path to the application log.
	HE_IMETHOD GetLogFilePath(/*out*/ Helium::HEBSTR* pbstrPath) = 0;
	/// Full path to the packet header info file's directory path .
	HE_IMETHOD GetPacketHeaderInfoStoragePath(/*out*/ Helium::HEBSTR* pbstrPath) = 0;
	/// Full path to the application security events.
	HE_IMETHOD GetSecurityFilePath(/*out*/ Helium::HEBSTR* pbstrPath) = 0;
};

/// \defgroup Adapter Adapter interfaces and classes implemented by Omni.
/// \brief Adapter-related interfaces and classes.

/// \struct WirelessChannel
/// \brief Detailed wireless channel info for wireless packets
/// \see IWirelessPacket
#include "hepushpack.h"
struct WirelessChannel
{
	SInt16		Channel;	///< Channel number, sometimes negative for Japanese 802.11A.
	UInt32		Frequency;	///< Frequency the channel operates on in MHz.
	UInt32		Band;		///< Band the channel is from (802.11A, B, G, A turbo, G Turbo, etc.).
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \interface IMediaInfo
/// \ingroup Adapter
/// \brief Everything you need to know about a network media.
#define IMediaInfo_IID \
{ 0x4B6FE574, 0xF29B, 0x4897, {0x96, 0xE0, 0x4E, 0xF6, 0xC9, 0xD3, 0x2B, 0x3E} }

class HE_NO_VTABLE IMediaInfo : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IMediaInfo_IID);

	/// Get the media type.
	HE_IMETHOD GetMediaType(/*out*/ TMediaType* pmt) = 0;
	/// Set the media type.
	HE_IMETHOD SetMediaType(/*in*/ TMediaType mt) = 0;
	/// Get the media sub type.
	HE_IMETHOD GetMediaSubType(/*out*/ TMediaSubType* pmst) = 0;
	/// Set the media sub type.
	HE_IMETHOD SetMediaSubType(/*in*/ TMediaSubType mst) = 0;
	/// Get the wireless domain.
	HE_IMETHOD GetMediaDomain(/*out*/ TMediaDomain* pmd) = 0;
	/// Set the wireless domain.
	HE_IMETHOD SetMediaDomain(/*in*/ TMediaDomain md) = 0;
	/// Get the link speed in bits/second units.
	HE_IMETHOD GetLinkSpeed(/*out*/ UInt64* pnLinkSpeed) = 0;
	/// Set the link speed in bits/second units.
	HE_IMETHOD SetLinkSpeed(/*in*/ UInt64 nLinkSpeed) = 0;
	/// Get the channels.
	HE_IMETHOD GetChannels(/*out*/ UInt32** ppChannels, /*out*/ UInt32* pnChannels) = 0;
	/// Set the channels.
	HE_IMETHOD SetChannels(/*in*/ const UInt32* pChannels, /*in*/ UInt32 nChannels) = 0;
	/// Get the wireless channels.
	HE_IMETHOD GetWirelessChannels(/*out*/ WirelessChannel** ppChannels, /*out*/ UInt32* pnChannels) = 0;
	/// Set the wireless channels.
	HE_IMETHOD SetWirelessChannels(/*in*/ const WirelessChannel* pChannels, /*in*/ UInt32 nChannels) = 0;
	/// Get the wireless data rates.
	HE_IMETHOD GetWirelessDataRates(/*out*/ UInt32** ppDataRates, /*out*/ UInt32* pnDataRates) = 0;
	/// Set the wireless data rates.
	HE_IMETHOD SetWirelessDataRates(/*in*/ const UInt32* pDataRates, /*in*/ UInt32 nDataRates) = 0;
};

/// \enum PeekAdapterType
/// \ingroup Adapter
/// \brief Adapter types.
/// \see IAdapter
enum PeekAdapterType
{
	peekNullAdapter,		///< Null adapter, none.
	peekNetworkAdapter,		///< Network adapter.
	peekReplayAdapter,		///< Replay adapter (a file adapter).
	peekPluginAdapter,		///< Adapter managed by a Distributed Analysis Module.
	peekRemoteAdapter		///< Remote adapter (adapter managed by omni-capture daemon).
};

/// \internal
/// \enum PeekChannelType
/// \ingroup Adapter
/// \brief Channel type used in PeekChannelData.
/// \see PeekChannelData
enum PeekChannelType
{
	peekChannelWireless,	///< Wireless channel set.
	peekChannelEnum,		///< Enumerable channel set.
	peekChannelMax			///< No channels.
};

/// \internal
/// \struct PeekChannelData
/// \ingroup Adapter
/// \brief Channel type, count, and data in one contiguous package.
#include "hepushpack.h"
struct PeekChannelData
{
	PeekChannelType	type;
	UInt32			count;
	UInt8			data[1];
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \interface IChannelManager
/// \ingroup Adapter
/// \brief Channel info.
#define IChannelManager_IID \
{ 0x831D4A3A, 0x0891, 0x4909, {0x98, 0xE7, 0x2F, 0x79, 0x08, 0x38, 0x6D, 0x24} }

class HE_NO_VTABLE IChannelManager : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IChannelManager_IID);

	/// \todo
	HE_IMETHOD GetChannelType(/*out*/PeekChannelType* pType) = 0;
	/// \todo
	HE_IMETHOD SetChannelType(/*in*/ PeekChannelType type) = 0;
	/// \todo
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	/// \todo
	HE_IMETHOD GetChannels(/*out*/ void** ppChannels, /*out*/ SInt32* plCount) = 0;
	/// \todo
	HE_IMETHOD SetChannels(/*in*/ const void* pChannels, /*in*/ SInt32 lCount) = 0;
	/// \todo
	HE_IMETHOD GetChannels2(/*out*/ PeekChannelData** ppChannelData) = 0;
	/// \todo
	HE_IMETHOD SetChannels2(/*in*/ const PeekChannelData* pChannelData) = 0;
	/// \todo
	HE_IMETHOD AddChannel(/*in*/ const void* pChannel) = 0; 
};

/// \internal
/// \interface IAdapterInfo
/// \ingroup Adapter
/// \brief Adapter info.
/// \see IAdapterInfoList, IAdapter
#define IAdapterInfo_IID \
{ 0x9A8E3BF3, 0xF44A, 0x495B, {0x9B, 0x8F, 0x7E, 0xB8, 0x97, 0x67, 0xC7, 0xC9} }

class HE_NO_VTABLE IAdapterInfo : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IAdapterInfo_IID);

	/// Get the adapter identifier.
	HE_IMETHOD GetIdentifier(/*out*/ Helium::HEBSTR* pbstrIdentifier) = 0;
	/// Is the adapter persistent? For example, network adapters are not.
	HE_IMETHOD GetPersistent(/*out*/ BOOL* pbPersistent) = 0;
	/// Is the adapter deletable? For example, network adapters are not, but
	/// replay adapters are.
	HE_IMETHOD GetDeletable(/*out*/ BOOL* pbDeletable) = 0;
	/// Get the adapter type.
	HE_IMETHOD GetAdapterType(/*out*/ PeekAdapterType* pAdapterType) = 0;
	/// Set the adapter description.
	HE_IMETHOD SetDescription(/*out*/ Helium::HEBSTR pbstrDescription) = 0;
	/// Get the adapter description.
	HE_IMETHOD GetDescription(/*out*/ Helium::HEBSTR* pbstrDescription) = 0;
	/// Get the media type.
	HE_IMETHOD GetMediaType(/*out*/ TMediaType* pMediaType) = 0;
	/// Get the media sub type.
	HE_IMETHOD GetMediaSubType(/*out*/ TMediaSubType* pMediaSubType) = 0;
	/// Get the link speed in bits/second units.
	HE_IMETHOD GetLinkSpeed(/*out*/ UInt64* pnLinkSpeed) = 0;
	/// Get the adapter physical address.
	HE_IMETHOD GetAddress(/*out*/ UInt8 pAddress[6]) = 0;
	/// Get the channels that the adapter supports.
	HE_IMETHOD GetChannelManager(/*out*/ IChannelManager** ppChannelManager) = 0;
	/// Get the media info.
	HE_IMETHOD GetMediaInfo(/*out*/ IMediaInfo** ppMediaInfo) = 0;
	/// Compare an address to the adapter physical address.
	HE_IMETHOD CompareAddress(/*in*/ const UInt8 pAddress[6], /*out*/ BOOL* pbResult) = 0;
	/// Create an adapter.
	HE_IMETHOD CreateAdapter(/*out*/ IAdapter** ppAdapter) = 0;
	/// Get the adapter type.
	HE_IMETHOD GetAdapterFeatures(/*out*/ UInt32* pAdapterFeatures) = 0;
};

/// \internal
/// \interface IAdapterInfoList
/// \ingroup Adapter
/// \brief List of adapter info.
/// \see IAdapterInfo, coclass AdapterInfoList
#define IAdapterInfoList_IID \
{ 0xD92F2756, 0xCBAC, 0x4D8A, {0xAD, 0x1A, 0x65, 0x4E, 0x68, 0xBA, 0xF1, 0xCB} }

class HE_NO_VTABLE IAdapterInfoList : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IAdapterInfoList_IID);

	/// Get adapter info by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ IAdapterInfo** ppAdapterInfo) = 0;
	/// Get the count of adapter info items in the list.
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	/// Add adapter info.
	HE_IMETHOD Add(/*in*/ IAdapterInfo* pAdapterInfo) = 0;
	/// Remove adapter info.
	HE_IMETHOD Remove(/*in*/ IAdapterInfo* pAdapterInfo) = 0;
	/// Remove all adapter info.
	HE_IMETHOD RemoveAll() = 0;
	/// Find adapter info given adapter type and identifier.
	HE_IMETHOD Find(/*in*/ PeekAdapterType type, /*in*/ Helium::HEBSTR bstrIdentifier,
		/*out*/ IAdapterInfo** ppAdapterInfo) = 0;
};

/// \internal
/// \interface IWirelessAdapterInfo
/// \ingroup Adapter
/// \brief Adapter info.
/// \see IAdapterInfo
#define IWirelessAdapterInfo_IID \
{ 0xED7C693C, 0x9CF1, 0x45BA, {0x8F, 0xD3, 0x4C, 0x6B, 0x7D, 0x9C, 0xEF, 0x57} }

class HE_NO_VTABLE IWirelessAdapterInfo : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IWirelessAdapterInfo_IID);

	/// Get the wireless domain.
	HE_IMETHOD GetMediaDomain(/*out*/ TMediaDomain* pMediaDomain) = 0;
	/// Get the wireless data rates.
	HE_IMETHOD GetWirelessDataRates(/*out*/ UInt32** ppDataRates, /*out*/ UInt32* pnDataRatesCount) = 0;
};

/// \internal
/// \interface IWANAdapterInfo
/// \ingroup Adapter
/// \brief Adapter info.
/// \see IAdapterInfo
#define IWANAdapterInfo_IID \
{ 0x2DD66998, 0xE21E, 0x46de, {0x88, 0x6D, 0xD9, 0xFB, 0x83, 0xE4, 0x75, 0xE7} }

class HE_NO_VTABLE IWANAdapterInfo : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IWANAdapterInfo_IID);

	/// Get the WAN protocol.
	HE_IMETHOD GetProtocol(/*out*/ UInt32* pnProtocol) = 0;
	/// Set the WAN protocol.
	HE_IMETHOD SetProtocol(/*in*/ UInt32 nProtocol) = 0;
};

/// \internal
/// \interface IWANHardwareOptions
/// \ingroup Adapter
/// \brief Configurations for WAN hardware options.
/// \see IWANHardwareOptions
#define IWANHardwareOptions_IID \
{ 0xB2457312, 0x96B5, 0x4703, {0x9F, 0x14, 0x88, 0x32, 0xD3, 0x3C, 0xCA, 0x5F} }

class HE_NO_VTABLE IWANHardwareOptions : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IWANHardwareOptions_IID);

	/// Get the WAN protocol.
	HE_IMETHOD GetProtocol(/*out*/ UInt32* pnProtocol) = 0;
	/// Set the WAN protocol.
	HE_IMETHOD SetProtocol(/*in*/ UInt32 nProtocol) = 0;
};

/// \internal
/// \interface INdisAdapterInfoInitialize
/// \ingroup Adapter
/// \brief Initialize adapter info.
/// \see IAdapterInfo, IAdapterInfoList, coclass NdisAdapterInfo
#define INdisAdapterInfoInitialize_IID \
{ 0x07299221, 0x0527, 0x4DCD, {0xAF, 0x68, 0x2D, 0xD1, 0x37, 0x6E, 0x1F, 0x38} }

class HE_NO_VTABLE INdisAdapterInfoInitialize : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(INdisAdapterInfoInitialize_IID);

	/// Initialize the adapter info given an adapter enumerator.
	HE_IMETHOD Initialize(/*in*/ const wchar_t* pszEnumerator) = 0;
};

/// \internal
/// \interface INdisAdapterInfo
/// \ingroup Adapter
/// \brief Adapter info for an NDIS network adapter.
/// \see IAdapterInfo, IAdapterInfoList, coclass NdisAdapterInfo
#define INdisAdapterInfo_IID \
{ 0x8C5C80CD, 0xDC1F, 0x412C, {0xA2, 0x17, 0x72, 0x49, 0xAF, 0x1C, 0xA3, 0xD5} }

class HE_NO_VTABLE INdisAdapterInfo : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(INdisAdapterInfo_IID);

	/// Create the adapter by querying the OS for info.
	HE_IMETHOD Create() = 0;
	/// Create "advanced" info for the adapter by loading a special driver.
	HE_IMETHOD CreateAdvanced() = 0;
	/// Is the info valid (has Create() been called)?
	HE_IMETHOD GetValid(/*out*/ BOOL* pbValid) = 0;
	/// Is the advanced info valid (has CreateAdvanced() been called)?
	HE_IMETHOD GetValidAdvanced(/*out*/ BOOL* pbValidAdvanced) = 0;
	/// Is the adapter hidden by the OS?
	HE_IMETHOD GetHidden(/*out*/ BOOL* pbHidden) = 0;
	/// Get the adapter enumerator.
	HE_IMETHOD GetEnumerator(/*out*/ Helium::HEBSTR* pbstrEnumerator) = 0;
	/// Get the adapter title.
	HE_IMETHOD GetTitle(/*out*/ Helium::HEBSTR* pbstrTitle) = 0;
	/// Get the adapter product name.
	HE_IMETHOD GetProductName(/*out*/ Helium::HEBSTR* pbstrProductName) = 0;
	/// Get the adapter service name (NT/2K/XP only).
	HE_IMETHOD GetServiceName(/*out*/ Helium::HEBSTR* pbstrServiceName) = 0;
	/// Get the adapter service symbolic link (NT/2K/XP only).
	HE_IMETHOD GetSymbolicLink(/*out*/ Helium::HEBSTR* pbstrSymbolicLink) = 0;
	/// Get the NDIS adapter characteristics.
	HE_IMETHOD GetNdisCharacteristics(/*out*/ UInt32* pnCharacteristics) = 0;
	/// Get the NDIS media type.
	HE_IMETHOD GetNdisMediaType(/*out*/ UInt32* pnNdisMediaType) = 0;
	/// Get the NDIS physical medium.
	HE_IMETHOD GetNdisPhysMedium(/*out*/ UInt32* pnNdisPhysMedium) = 0;
	/// Get the PEEK custom driver interface version.
	HE_IMETHOD GetCustomInterfaceVersion(/*out*/ UInt32* pnCustomInterfaceVersion) = 0;
	/// Get the PEEK custom driver interface features.
	HE_IMETHOD GetCustomInterfaceFeatures(/*out*/ UInt32* pnCustomInterfaceFeatures) = 0;
	/// Get the custom adapter features.
	HE_IMETHOD GetCustomAdapterFeatures(/*out*/ UInt32* pnCustomAdapterFeatures) = 0;
	/// Check if a given OID is supported by the underlying device driver.
	HE_IMETHOD IsOidSupported(/*in*/ UInt32 oid, /*out*/ BOOL* pbResult) = 0;
	/// Get the adapter version.
	HE_IMETHOD GetVersions(/*out*/ Helium::HEBSTR* pbstrVersions) = 0;
};

/// \internal
/// \enum Topology
/// \ingroup Adapter
/// \brief Adapter topologies.
/// \see ITopologySupportConfig
enum Topology
{
	topologyEthernet,
	topologyWireless,
	topologyGigabit,
	topologyWan
};

/// \internal
/// \enum TopologySupportLevel
/// \ingroup Adapter
/// \brief Adapter topology support levels.
/// \see ITopologySupportConfig
enum TopologySupportLevel
{
	topologySupportLevelNone,		///< Don't support capture for the topology.
	topologySupportLevelMinimal,	///< Support limited capture features for the topology (Ethernet fall back mode).
	topologySupportLevelFull		///< Fully support capture features for the topology.
};

/// \internal
/// \interface ITopologySupportConfig
/// \ingroup Adapter
/// \brief Holds the topology support configuration.
/// \see coclass TopologySupportConfig
#define ITopologySupportConfig_IID \
{ 0xD99B52A9, 0x94D9, 0x4510, {0x90, 0x33, 0x8D, 0x3F, 0x8D, 0x60, 0xF1, 0xDA} }

class HE_NO_VTABLE ITopologySupportConfig : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ITopologySupportConfig_IID);

	/// Get the topology configuration.
	HE_IMETHOD GetTopologySupportLevel(/*in*/ Topology topology,
		/*out*/ TopologySupportLevel* pLevel) = 0;
	/// Set the topology configuration.
	HE_IMETHOD SetTopologySupportLevel(/*in*/ Topology topology,
		/*in*/ TopologySupportLevel level) = 0;
};

/// \internal
/// \interface IReplayAdapterInfo
/// \ingroup Adapter
/// \brief Adapter info for a replay adapter.
/// \see IAdapterInfo, IAdapterInfoList, coclass ReplayAdapterInfo
#define IReplayAdapterInfo_IID \
{ 0xE76DCC3D, 0xDFB3, 0x4FBB, {0x84, 0x93, 0x95, 0x65, 0xD2, 0x78, 0x1F, 0xD2} }

class HE_NO_VTABLE IReplayAdapterInfo : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IReplayAdapterInfo_IID);

	/// Get the file path.
	HE_IMETHOD GetFilePath(/*out*/ Helium::HEBSTR* pbstrFilePath) = 0;
	/// Set the file path.
	HE_IMETHOD SetFilePath(/*in*/ Helium::HEBSTR bstrFilePath) = 0;
};

/// \enum PeekPluginAdapterType
/// \ingroup Adapter
/// \brief Distributed Analysis Module adapter types.
/// \see IPluginAdapterInfo
enum PeekPluginAdapterType
{
	peekNullPluginAdapter,		///< Null Distributed Analysis Module adapter type, none.
	peekRootPluginAdapter,		///< TODO
	peekGroupPluginAdapter,		///< TODO
	peekAdapterPluginAdapter,	///< TODO
	peekPrivatePluginAdapter	///< Private
};

/// \interface IPluginAdapterInfo
/// \ingroup Adapter
/// \brief Adapter info for a Distributed Analysis Module adapter.
/// \see IAdapterInfo, IAdapterInfoList, coclass PluginAdapterInfo
#define IPluginAdapterInfo_IID \
{ 0x140FFB94, 0xE6A8, 0x490C, {0xB9, 0xAA, 0xF7, 0x1B, 0x60, 0x6C, 0x34, 0xA0} }

class HE_NO_VTABLE IPluginAdapterInfo : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPluginAdapterInfo_IID);

	/// Get the Distributed Analysis Module adapter type.
	HE_IMETHOD GetPluginAdapterType(/*out*/ PeekPluginAdapterType* pType) = 0;
	/// Get the name of the plugin that created this.
	HE_IMETHOD GetPluginName(/*out, retval*/ Helium::HEBSTR* pbstrPlugin) = 0;
	/// Get the title of the adapter.
	HE_IMETHOD GetTitle(/*out, retval*/ Helium::HEBSTR* pbstrTitle) = 0;
	// Plugin Adapter Info allows setting all of the things that IAdapterInfo requests
	/// Set the name of the plugin that created this.
	HE_IMETHOD SetPluginName(/*in*/ Helium::HEBSTR bstrPlugin) = 0;
	/// Set the title of the adapter.
	HE_IMETHOD SetTitle(/*in*/ Helium::HEBSTR bstrTitle) = 0;
	/// the adapter identifier.
	HE_IMETHOD SetIdentifier(/*in*/ Helium::HEBSTR bstrIdentifier) = 0;
	/// Is the adapter persistent? For example, network adapters are not.
	HE_IMETHOD SetPersistent(/*in*/ BOOL bPersistent) = 0;
	/// Is the adapter deletable? For example, network adapters are not, but
	/// replay adapters are.
	HE_IMETHOD SetDeletable(/*in*/ BOOL bDeletable) = 0;
	/// the adapter description.
	HE_IMETHOD SetDescription(/*in*/ Helium::HEBSTR bstrDescription) = 0;
	/// the PEEK media type.
	HE_IMETHOD SetMediaType(/*in*/ TMediaType mt) = 0;
	/// Set the PEEK media sub type.
	HE_IMETHOD SetMediaSubType(/*in*/ TMediaSubType mst) = 0;
	/// Get the link speed in bits/second units.
	HE_IMETHOD SetLinkSpeed(/*in*/ UInt64 nLinkSpeed) = 0;
	/// Get the adapter physical address.
	HE_IMETHOD SetAddress(/*in*/ UInt8 pAddress[6]) = 0;
	/// Get the channels that the adapter supports.
	HE_IMETHOD SetChannelManager(/*in*/ IChannelManager* pChannelManager) = 0;
	/// Get the media info.
	HE_IMETHOD SetMediaInfo(/*in*/ IMediaInfo* pMediaInfo) = 0;
	/// Set the Distributed Analysis Module adapter type.
	HE_IMETHOD SetPluginAdapterType(/*in*/ PeekPluginAdapterType type) = 0;
	/// Get the id of the plugin that created this.
	HE_IMETHOD GetPluginID(/*out*/ Helium::HeID* pid) = 0;
	/// Set the id of the plugin that created this.
	HE_IMETHOD SetPluginID(/*in*/ const Helium::HeID& id) = 0;
	/// Set the adapter features.
	HE_IMETHOD SetAdapterFeatures(/*in*/ UInt32 nAdapterFeatures) = 0;
};

/// \interface IAdapterConfig
/// \ingroup Adapter
/// \brief Configuration for an adapter including hardware options.
/// \see coclass AdapterConfig
#define IAdapterConfig_IID \
{ 0xB49DDABE, 0xAB43, 0x4C51, {0xBC, 0x32, 0x2E, 0xEE, 0x29, 0x40, 0xDD, 0x08} }

class HE_NO_VTABLE IAdapterConfig : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IAdapterConfig_IID);

	/// Get the link speed in bits/second.
	HE_IMETHOD GetLinkSpeed(/*out*/ UInt64* pnLinkSpeed) = 0;
	/// Set the link speed in bits/second.
	HE_IMETHOD SetLinkSpeed(/*in*/ UInt64 nLinkSpeed) = 0;
	/// Get the default link speed in bits/second.
	HE_IMETHOD GetDefaultLinkSpeed(/*out*/ UInt64* pnLinkSpeed) = 0;
	/// Set the default link speed in bits/second.
	HE_IMETHOD SetDefaultLinkSpeed(/*in*/ UInt64 nLinkSpeed) = 0;
	/// Get the ring buffer size in bytes.
	HE_IMETHOD GetRingBufferSize(/*out*/ UInt32* pcbRingBufferSize) = 0;
	/// Set the ring buffer size in bytes.
	HE_IMETHOD SetRingBufferSize(/*in*/ UInt32 cbRingBufferSize) = 0;
	/// Get a hardware options ID by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ Helium::HeID* pID) = 0;
	/// Get the count of hardware options IDs.
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	/// Add a hardware options ID.
	HE_IMETHOD Add(/*in*/ const Helium::HeID& id) = 0;
	/// Remove a hardware options ID.
	HE_IMETHOD Remove(/*in*/ const Helium::HeID& id) = 0;
	/// Remove all hardware options IDs.
	HE_IMETHOD RemoveAll() = 0;
};

/// \interface IAdapter
/// \ingroup Adapter
/// \brief Basic adapter interface.
/// \see ICapturePackets, ISendPackets, coclass NdisAdapter, coclass ReplayAdapter
#define IAdapter_IID \
{ 0x71A07F5D, 0x10E4, 0x4DF2, {0xB1, 0x26, 0x06, 0xE1, 0x47, 0x27, 0x0E, 0xDA} }

class HE_NO_VTABLE IAdapter : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IAdapter_IID);

	/// Get the adapter info.
	HE_IMETHOD GetAdapterInfo(/*out*/ IAdapterInfo** ppAdapterInfo) = 0;
	/// Get the adapter hardware options.
	HE_IMETHOD GetHardwareOptions(/*out*/ Helium::IHeUnknown** ppHardwareOptions) = 0;
	/// Set the adapter hardware options.
	HE_IMETHOD SetHardwareOptions(/*in*/ Helium::IHeUnknown* pHardwareOptions) = 0;
	/// Get the adapter type.
	HE_IMETHOD GetAdapterType(/*out*/ PeekAdapterType* pAdapterType) = 0;
	/// Get the adapter name.
	HE_IMETHOD GetName(/*out*/ Helium::HEBSTR* pbstrName) = 0;
	/// Get the adapter media type.
	HE_IMETHOD GetMediaType(/*out*/ TMediaType* pMediaType) = 0;
	/// Get the adapter media sub type.
	HE_IMETHOD GetMediaSubType(/*out*/ TMediaSubType* pMediaSubType) = 0;
	/// Get the adapter link speed in bits/second units.
	HE_IMETHOD GetLinkSpeed(/*out*/ UInt64* pnLinkSpeed) = 0;
	/// Get the adapter physical address.
	HE_IMETHOD GetAddress(/*out*/ UInt8 pAddress[6]) = 0;
	/// Is networking supported during capture?
	HE_IMETHOD GetNetSupportDuringCapture(/*out*/ BOOL* pbNetSupp) = 0;
	/// Open the adapter.
	HE_IMETHOD Open() = 0;
	/// Close the adapter.
	HE_IMETHOD Close() = 0;
};

/// \interface IAdapterTime
/// \ingroup Adapter
/// \brief Get current time from the same time source used by the adapter.
#define IAdapterTime_IID \
{ 0xCFC99C73, 0xFBD5, 0x44DF, {0x88, 0x43, 0xB3, 0x81, 0xEA, 0xF8, 0x48, 0xA4} }

class HE_NO_VTABLE IAdapterTime : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IAdapterTime_IID);

	/// Get the current time.
	HE_IMETHOD GetTimeStamp(/*out*/ UInt64* pTimeStamp) = 0;
};

/// \internal
/// \interface IGigUtil
/// \ingroup Adapter
/// \brief Basic Gigabit card utility interface.
/// \see coclass NapatechUtil
#define IGigUtil_IID \
{ 0x03A3E9BE, 0x1857, 0x4b06, {0xAE, 0xFA, 0x6E, 0xC5, 0xB8, 0xFF, 0x62, 0x42} }

class HE_NO_VTABLE IGigUtil : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGigUtil_IID);

	/// Open the card.
	HE_IMETHOD Open(/*in*/ UInt32 nIdx) = 0;
	/// Close the card.
	HE_IMETHOD Close() = 0;
	/// Get a packet.
	HE_IMETHOD GetNextPacket(/*out*/ UInt8** ppData, /*out*/ PeekPacket* pPeekPacket) = 0;
	/// Release last used packet.
	HE_IMETHOD ReleaseLastPacket() = 0;
	/// Create hardware filters.
	HE_IMETHOD CreateHardwareFilters(/*in*/ void* pFilterData, /*in*/ UInt32 nDataLength) = 0;
	/// Check if the card index exists.
	HE_IMETHOD IsAdapterIdxValid(/*in*/ UInt32 nIdx, /*out*/ BOOL* pbValid) = 0;
	/// Create a capture feed.
	HE_IMETHOD CreateFeed() = 0;
	/// Hook method that is called first in Run() method of a feed thread.
	HE_IMETHOD PreRun() = 0;
	/// Stop capture feed.
	HE_IMETHOD StopFeed() = 0;
	/// Close a capture feed.
	HE_IMETHOD DestroyFeed() = 0;
	/// What the card calls itself.
	HE_IMETHOD GetCardName(/*out*/ Helium::HEBSTR* pbstrName) = 0;
	/// Get features supported.
	HE_IMETHOD GetAdapterFeatures(/*out*/ UInt32* pFeatures) = 0;
	/// Get number of channels on card.
	HE_IMETHOD GetNumChannels(/*out*/ UInt32* pnChannels) = 0;
	/// Get number of dropped packets.
	HE_IMETHOD GetNumDroppedPackets(/*out*/ UInt64 *nDropped) = 0;
	/// Get maximum port link speed.
	HE_IMETHOD GetMaxLinkSpeed(/*out*/ UInt64* pnLinkSpeed) = 0;
	/// Get card version info.
	HE_IMETHOD GetVersionInfo(/*out*/ Helium::HEBSTR* pbstrVersion) = 0;
	/// Get card version info.
	HE_IMETHOD GetMACAddress(/*out*/ UInt8* payAddress) = 0;
	/// Get the count of packet streams.
	HE_IMETHOD GetStreamCount(SInt32* pCount) = 0;
	/// Get the next packet from a stream.
	HE_IMETHOD GetNextStreamPacket(/*in*/ UInt32 nStreamIndex, 
		/*out*/ UInt8** ppData, /*out*/ PeekPacket* pPeekPacket) = 0;
	/// Release the last packet from a stream.
	HE_IMETHOD ReleaseLastStreamPacket(/*in*/ UInt32 nStreamIndex) = 0;
	/// Get the current time.
	HE_IMETHOD GetTimeStamp(/*out*/ UInt64* pTimeStamp) = 0;
};

/// \interface ICapturePackets
/// \ingroup Adapter
/// \brief Start, stop, pause, resume packet capture.
/// \see IAdapter, ISendPackets, coclass NdisAdapter, coclass ReplayAdapter
#define ICapturePackets_IID \
{ 0x38FF17B6, 0xBF36, 0x467C, {0x8B, 0xA9, 0x55, 0x23, 0xBA, 0x43, 0x8C, 0x4C} }

class HE_NO_VTABLE ICapturePackets : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICapturePackets_IID);

	/// Is capturing on?
	HE_IMETHOD IsCapturing(/*out*/ BOOL* pbCapturing) = 0;
	/// Start capturing packets.
	HE_IMETHOD StartCapture() = 0;
	/// Stop capturing packets.
	HE_IMETHOD StopCapture() = 0;
	/// Pause packet capture.
	HE_IMETHOD PauseCapture() = 0;
	/// Resume packet capture.
	HE_IMETHOD ResumeCapture() = 0;
};

/// \interface ISendPackets
/// \ingroup Adapter
/// \brief Start and stop sending packets; send individual packets.
/// \see IAdapter, ICapturePackets, coclass NdisAdapter, coclass ReplayAdapter
#define ISendPackets_IID \
{ 0xC0A22522, 0xEE2B, 0x41AB, {0x93, 0x84, 0x00, 0xA7, 0xD4, 0x91, 0x6E, 0x21} }

class HE_NO_VTABLE ISendPackets : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISendPackets_IID);

	/// Is sending on?
	HE_IMETHOD IsSending(/*out*/ BOOL* pbSending) = 0;
	/// Start sending packets.
	HE_IMETHOD StartSend() = 0;
	/// Stop sending packets.
	HE_IMETHOD StopSend() = 0;
	/// Send a packet.
	HE_IMETHOD Send(/*in*/ const UInt8* pData, /*in*/ UInt32 cbData, /*in*/ UInt32 nCount) = 0;
	/// Reset the count of packets and bytes sent.
	HE_IMETHOD ResetSendCounts() = 0;
	/// Set the send data for continuous send.
	HE_IMETHOD SetSendData(/*in*/ const UInt8* pData, /*in*/ UInt32 cbData) = 0;
	/// Get the send data for continuous send.
	HE_IMETHOD GetSendData(/*out*/ UInt8** ppData, /*out*/ UInt32* pcbData) = 0;
	/// Get the send interval for continuous send in milliseconds.
	HE_IMETHOD GetSendInterval(UInt32* pnSendInterval) = 0;
	/// Set the send interval for continuous send in milliseconds.
	HE_IMETHOD SetSendInterval(/*in*/ UInt32 nSendInterval) = 0;
	/// Get the burst count for continuous send.
	HE_IMETHOD GetSendBurstCount(UInt32* pnSendBurstCount) = 0;
	/// Set the burst count for continuous send.
	HE_IMETHOD SetSendBurstCount(/*in*/ UInt32 nSendBurstCount) = 0;
};

/// \interface IAdapterInitialize
/// \ingroup Adapter
/// \brief Initialize an adapter with adapter info.
/// \see coclass NdisAdapter
#define IAdapterInitialize_IID \
{ 0x6E277BEC, 0x735D, 0x4CD7, {0xA3, 0x0D, 0x60, 0x92, 0x9D, 0x3C, 0xF2, 0x05} }

class HE_NO_VTABLE IAdapterInitialize : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IAdapterInitialize_IID);

	/// Initialize an adapter with adapter info.
	HE_IMETHOD Initialize(/*in*/ IAdapterInfo* pAdapterInfo) = 0;
};

/// \internal
/// \interface INdisAdapter
/// \ingroup Adapter
/// \brief Configure NDIS adapter with ring buffer size, etc.
/// \see coclass NdisAdapter
#define INdisAdapter_IID \
{ 0x24C7FDEC, 0x9A58, 0x4284, {0x99, 0xBE, 0xC0, 0x7F, 0x05, 0xC9, 0x95, 0xDD} }

class HE_NO_VTABLE INdisAdapter : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(INdisAdapter_IID);

	/// Get the ring buffer size.
	HE_IMETHOD GetRingBufferSize(/*out*/ UInt32* pcbRingBufferSize) = 0;
	/// Set the ring buffer size.
	HE_IMETHOD SetRingBufferSize(/*in*/ UInt32 cbRingBufferSize) = 0;
	/// Get the dropped packet count.
	HE_IMETHOD GetPacketsDropped(/*out*/ UInt64* pnPacketsDropped) = 0;
};

/// \internal
/// \interface IReplayAdapter
/// \ingroup Adapter
/// \brief Configure replay count, etc.
/// \see coclass ReplayAdapter
#define IReplayAdapter_IID \
{ 0x03DBE424, 0x20D7, 0x440A, {0xA1, 0x32, 0xE9, 0x55, 0x53, 0xF4, 0x10, 0x3B} }

class HE_NO_VTABLE IReplayAdapter : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IReplayAdapter_IID);

	/// Get the replay limit.
	HE_IMETHOD GetReplayLimit(/*out*/ UInt32* pnReplayLimit) = 0;
	/// Set the replay limit (zero means replay indefinitely).
	HE_IMETHOD SetReplayLimit(/*in*/ UInt32 nReplayLimit) = 0;
	/// Get the replay count.
	HE_IMETHOD GetReplayCount(/*out*/ UInt32* pnReplayCount) = 0;
	/// Get the replay speed.
	HE_IMETHOD GetReplaySpeed(/*out*/ double* pnReplaySpeed) = 0;
	/// Set the replay speed (1.0 is normal, 0.5 is double speed, 2 is half speed).
	HE_IMETHOD SetReplaySpeed(/*in*/ double nReplaySpeed) = 0;
};

/// \internal
/// \enum ReplayAdapterTimeStampMode
/// \ingroup Adapter
/// \brief Settings for replay adapter timestamp behavior.
/// \see IReplayAdapter2
enum ReplayAdapterTimeStampMode
{
	replayAdapterTimeStampsOS,				///< Use the current OS time.
	replayAdapterTimeStampsOriginal,		///< Use the original packet time.
	replayAdapterTimeStampsOriginalOffset	///< Use the original packed time offset from the capture start time.
};

/// \internal
/// \interface IReplayAdapter2
/// \ingroup Adapter
/// \brief Configure replay options.
/// \see coclass ReplayAdapter
#define IReplayAdapter2_IID \
{ 0x86A10AA2, 0xB516, 0x431E, {0xB0, 0x8B, 0x86, 0xD8, 0xE5, 0x3D, 0x9F, 0x24} }

class HE_NO_VTABLE IReplayAdapter2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IReplayAdapter2_IID);

	/// Get the timestamp mode. \see ReplayAdapterTimeStampMode
	HE_IMETHOD GetReplayTimeStampMode(/*out*/ ReplayAdapterTimeStampMode* pMode) = 0;
	/// Set the timestamp mode. \see ReplayAdapterTimeStampMode
	HE_IMETHOD SetReplayTimeStampMode(/*in*/ ReplayAdapterTimeStampMode mode) = 0;
};

/// \internal
/// \interface IAdapterManager
/// \ingroup Adapter
/// \brief Full service adapter management.
/// \see coclass AdapterManager
#define IAdapterManager_IID \
{ 0xD4E387C9, 0xCC3B, 0x46BC, {0xB6, 0x22, 0xB3, 0xB8, 0x0F, 0x83, 0x04, 0x57} }

class HE_NO_VTABLE IAdapterManager : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IAdapterManager_IID);

	/// Get the adapter count.
	HE_IMETHOD GetAdapterCount(/*out*/ SInt32* pnAdapters) = 0;
	/// Get the adapter info list.
	HE_IMETHOD GetAdapterInfoList(/*out*/ IAdapterInfoList** ppAdapterInfoList) = 0;
	/// Get an adapter.
	HE_IMETHOD GetAdapter(/*in*/ PeekAdapterType type,
		/*in*/ Helium::HEBSTR bstrIdentifier, /*in*/ Helium::HEBSTR bstrDescription,
		/*out*/ IAdapter** ppAdapter) = 0;
	/// Get adapter configuration.
	HE_IMETHOD GetAdapterConfig(/*in*/ PeekAdapterType type,
		/*in*/ Helium::HEBSTR bstrIdentifier,
		/*out*/ IAdapterConfig** ppAdapterConfig) = 0;
	/// Set adapter configuration.
	HE_IMETHOD SetAdapterConfig(/*in*/ PeekAdapterType type,
		/*in*/ Helium::HEBSTR bstrIdentifier,
		/*in*/ IAdapterConfig* pAdapterConfig) = 0;
	// Add an adapter.
	HE_IMETHOD AddAdapter(/*in*/ IAdapter* pAdapter, /*in*/ IAdapterInfo* pAdapterInfo,
		/*in*/ IAdapterConfig* pAdapterConfig) = 0;
	// Remove an adapter.
	HE_IMETHOD RemoveAdapter(/*in*/ IAdapter* pAdapter) = 0;
	// Refresh the adapter info list.
	HE_IMETHOD RefreshAdapterInfo() = 0;
};

/// \struct PeekCaptureStats
/// \brief Capture statistics.
/// \see ICaptureStats
#include "hepushpack.h"
struct PeekCaptureStats
{
	UInt64		TotalPackets;				///< Total packets.
	UInt64		TotalBytes;					///< Total bytes.
	double		AverageUtilization;			///< Average bits per second.
	double		CurrentUtilization;			///< Current bits per second.
	double		MaxUtilization;				///< Max bits per second.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \interface ICaptureStats
/// \ingroup Adapter
/// \brief Get adapter stats
/// \see struct PeekAdapterStats
#define ICaptureStats_IID \
{ 0xB2238DC3, 0x2F02, 0x4327, {0x88, 0x10, 0xA9, 0x81, 0x79, 0x89, 0x4C, 0x4D} }

class HE_NO_VTABLE ICaptureStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICaptureStats_IID);

	/// Get the capture stats.
	HE_IMETHOD GetCaptureStats(/*out*/ PeekCaptureStats* pStats) = 0;
};

/// \internal
/// \interface IHardwareOptions
/// \ingroup Adapter
/// \brief Hardware options
#define IHardwareOptions_IID \
{ 0xFCE74160, 0x4819, 0x44C4, {0x91, 0x19, 0xE1, 0xEB, 0xC4, 0xA1, 0x61, 0x77} }

class HE_NO_VTABLE IHardwareOptions : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IHardwareOptions_IID);

	/// Get the hardware options ID.
	HE_IMETHOD GetID(/*out*/ Helium::HeID* pID) = 0;
	/// Set the hardware options ID.
	HE_IMETHOD SetID(/*in*/ const Helium::HeID& id) = 0;
	/// Get the hardware options name.
	HE_IMETHOD GetName(/*out*/ Helium::HEBSTR* pbstrName) = 0;
	/// Set the hardware options name.
	HE_IMETHOD SetName(/*in*/ Helium::HEBSTR bstrName) = 0;
	/// Get the hardware options comment.
	HE_IMETHOD GetComment(/*out*/ Helium::HEBSTR* pbstrComment) = 0;
	/// Set the hardware options comment.
	HE_IMETHOD SetComment(/*in*/ Helium::HEBSTR bstrComment) = 0;
	/// Get the hardware options color.
	HE_IMETHOD GetColor(/*out*/ UInt32* pColor) = 0;
	/// Set the hardware options color.
	HE_IMETHOD SetColor(/*in*/ UInt32 color) = 0;
	/// Get the time the hardware options was created (PEEK format).
	HE_IMETHOD GetCreated(/*out*/ UInt64* pnTimeStamp) = 0;
	/// Set the time the hardware options was created (PEEK format).
	HE_IMETHOD SetCreated(/*in*/ UInt64 nTimeStamp) = 0;
	/// Get the time the hardware options was last modified (PEEK format).
	HE_IMETHOD GetModified(/*out*/ UInt64* pnTimeStamp) = 0;
	/// Set the time the hardware options was last modified (PEEK format).
	HE_IMETHOD SetModified(/*in*/ UInt64 nTimeStamp) = 0;
};

/// \internal
/// \interface IHardwareOptionsCollection
/// \ingroup Adapter
/// \brief A collection of hardware options.
/// \see coclass HardwareOptionsCollection
#define IHardwareOptionsCollection_IID \
{ 0xF8EE4B28, 0xE14E, 0x4DFD, {0xA1, 0x49, 0x67, 0xCE, 0x89, 0x7F, 0x44, 0x44} }

class HE_NO_VTABLE IHardwareOptionsCollection : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IHardwareOptionsCollection_IID);

	/// Get the count of hardware options in the list.
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	/// Get a hardware options by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ IHardwareOptions** ppHardwareOptions) = 0;
	/// Get a hardware options by ID.
	HE_IMETHOD ItemFromID(/*in*/ const Helium::HeID& id,
		/*out*/ IHardwareOptions** ppHardwareOptions) = 0;
	/// Add a hardware option.
	HE_IMETHOD Add(/*in*/ IHardwareOptions* pHardwareOptions) = 0;
	/// Remove a hardware option.
	HE_IMETHOD Remove(/*in*/ IHardwareOptions* pHardwareOptions) = 0;
	/// Remove all hardware options.
	HE_IMETHOD RemoveAll() = 0;
};

/// \internal
/// \interface IGigHardwareOptions
/// \ingroup Adapter
/// \brief Specific Gigabit hardware options.
/// \see coclass GigHardwareOptions
#define IGigHardwareOptions_IID \
{ 0x1DBB544F, 0x584F, 0x4AB0, {0x82, 0xEA, 0xEA, 0x93, 0xAD, 0x32, 0x69, 0x0F} }

class HE_NO_VTABLE IGigHardwareOptions : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGigHardwareOptions_IID);

	/// Is slicing enabled?
	HE_IMETHOD GetSlicingEnabled(/*out*/ BOOL* pbSlicingEnabled) = 0;
	/// Enable slicing.
	HE_IMETHOD SetSlicingEnabled(/*in*/ BOOL bSlicingEnabled) = 0;
	/// Get the slice length.
	HE_IMETHOD GetSliceLength(/*out*/ UInt16* pnSliceLength) = 0;
	/// Set slice length per packet.
	HE_IMETHOD SetSliceLength(/*in*/ UInt16 nSliceLength) = 0;
	/// Is the filter enabled?
	HE_IMETHOD GetFilterEnabled(/*out*/ BOOL* pbFilterEnabled) = 0;
	/// Enable the filter.
	HE_IMETHOD SetFilterEnabled(/*in*/ BOOL bFilterEnabled) = 0;
	/// Get the filter.
	HE_IMETHOD GetFilter(/*out*/ Helium::IHeUnknown** ppFilter) = 0;
	/// Set the filter.
	HE_IMETHOD SetFilter(/*in*/ Helium::IHeUnknown* pFilter) = 0;
};

/// \internal
/// \interface IGigHardwareChannelFilter
/// \ingroup Adapter
/// \brief Specific Gigabit hardware channel filter.
/// \see coclass NapatechHardwareFilter
#define IGigHardwareChannelFilter_IID \
{ 0xF94D6CD5, 0xBDED, 0x426C, {0x82, 0x22, 0x35, 0x8F, 0xBF, 0x3E, 0x6C, 0xDC} }

class HE_NO_VTABLE IGigHardwareChannelFilter : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGigHardwareChannelFilter_IID);

	/// Get the channel (numbered from one, zero = invalid).
	HE_IMETHOD GetChannel(/*out*/ UInt32* pnChannel) = 0;
	/// Set the channel (numbered from one, zero = invalid).
	HE_IMETHOD SetChannel(/*in*/ UInt32 nChannel) = 0;
	/// Discard errors?
	HE_IMETHOD GetDiscardErrors(/*out*/ BOOL* pbDiscaredErrors) = 0;
	/// Set whether to discard errors.
	HE_IMETHOD SetDiscardErrors(/*in*/ BOOL bDiscardErrors) = 0;
	/// Is this an exclusive filter?
	HE_IMETHOD GetExclusiveFilter(/*out*/ BOOL* pbExclusiveFilter) = 0;
	/// Set whether this is an exclusive filter.
	HE_IMETHOD SetExclusiveFilter(/*in*/ BOOL bExclusiveFilter) = 0;
	/// Is this an address filter?
	HE_IMETHOD GetAddressFilterEnabled(/*out*/ BOOL* pbAddressFilter) = 0;
	/// Set whether to filter for addresses.
	HE_IMETHOD SetAddressFilterEnabled(/*in*/ BOOL bAddressFilter) = 0;
	/// Get the address filter.
	HE_IMETHOD GetAddressFilter(/*out*/ TMediaSpec* pAddr1, /*out*/ TMediaSpec* pAddr2,
		/*out*/ BOOL* pb8023IP, /*out*/ BOOL* pbEType2IP,
		/*out*/ BOOL* pbAccept1To2, /*out*/ BOOL* pbAccept2To1) = 0;
	/// Set the address filter.
	HE_IMETHOD SetAddressFilter(/*in*/ TMediaSpec* pAddr1, /*in*/ TMediaSpec* pAddr2,
		/*in*/ BOOL b8023IP, /*in*/ BOOL bEType2IP,
		/*in*/ BOOL bAccept1To2, /*in*/ BOOL bAccept2To1) = 0;
	/// Is this a port filter?
	HE_IMETHOD GetPortFilterEnabled(/*out*/ BOOL* pbPortFilter) = 0;
	/// Set whether to filter for ports.
	HE_IMETHOD SetPortFilterEnabled(/*in*/ BOOL bPortFilter) = 0;
	/// Get the port filter.
	HE_IMETHOD GetPortFilter(/*out*/ TMediaSpec* pPort1, /*out*/ TMediaSpec* pPort2,
		/*out*/ BOOL* pbTCP, /*out*/ BOOL* pbUDP,
		/*out*/ BOOL* pbAccept1To2, /*out*/ BOOL* pbAccept2To1) = 0;
	/// Set the address filter.
	HE_IMETHOD SetPortFilter(/*in*/ TMediaSpec* pPort1, /*in*/ TMediaSpec* pPort2,
		/*in*/ BOOL bTCP, /*in*/ BOOL bUDP,
		/*in*/ BOOL bAccept1To2, /*in*/ BOOL bAccept2To1) = 0;
};

/// \internal
/// \interface IGigHardwareFilter
/// \ingroup Adapter
/// \brief Specific Gigabit hardware filter.
///
/// A collection of IGigHardwareChannelFilters plus other properties.
///
/// \see coclass NapatechHardwareFilter
#define IGigHardwareFilter_IID \
{ 0x1676DB94, 0x55C5, 0x49F6, {0xB2, 0xB8, 0xF8, 0x5A, 0xFA, 0x98, 0xBA, 0x69} }

class HE_NO_VTABLE IGigHardwareFilter : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGigHardwareFilter_IID);

	/// Is the filter active?
	HE_IMETHOD GetActive(/*out*/ BOOL* pbActive) = 0;
	/// Is this a one channel filter?
	HE_IMETHOD GetOneChannelFilter(/*out*/ BOOL* pbOneChannelFilter) = 0;
	/// Set whether this is a one channel filter.
	HE_IMETHOD SetOneChannelFilter(/*in*/ BOOL bOneChannelFilter) = 0;
	/// Get a channel filter by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ IGigHardwareChannelFilter** ppChannelFilter) = 0;
	/// Get the count of channel filters in the list.
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	/// Add a channel filter.
	HE_IMETHOD Add(/*in*/ IGigHardwareChannelFilter* pChannelFilter) = 0;
	/// Remove a channel filter.
	HE_IMETHOD Remove(/*in*/ IGigHardwareChannelFilter* pChannelFilter) = 0;
	/// Remove all channel filters.
	HE_IMETHOD RemoveAll() = 0;
};

/// \internal
/// \interface IBuildHardwareFilter
/// \ingroup Adapter
/// \brief Build a hardware filter into a stream.
#define IBuildHardwareFilter_IID \
{ 0x15113788, 0x35BA, 0x47D9, {0xA6, 0x22, 0x86, 0x8A, 0x09, 0x97, 0x09, 0xA6} }

class HE_NO_VTABLE IBuildHardwareFilter : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IBuildHardwareFilter_IID);

	/// Build a hardware filter into a stream.
	HE_IMETHOD BuildHardwareFilter(/*in*/ Helium::IHeStream* pStream, /*in*/ BOOL bGenericFilter) = 0;
};

/// \struct PeekWEPKey
/// \ingroup Adapter
#include "hepushpack.h"
struct PeekWEPKey
{
	UInt32		KeyBitLen;		///< \todo
	UInt8		Key[256];		///< \todo
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \enum PeekWEPAlgorithm
/// \ingroup Adapter
enum PeekWEPAlgorithm
{
	peekWEPAlgorithm40bSharedKey,			///< \todo
	peekWEPAlgorithm128bSharedKey,			///< \todo
	peekWEPAlgorithm152bSharedKey,			///< \todo
	peekWEPAlgorithmUserDefinedSharedKey,	///< \todo
	peekWEPAlgorithmWPA512bSharedKey,		///< \todo
	peekWEPAlgorithmWPAPassPhraseSharedKey	///< \todo
};

/// \internal
/// \interface IWEPKeySet
/// \ingroup Adapter
/// \brief A WEP key set for wireless hardware options.
/// \see coclass WEPKeySet
#define IWEPKeySet_IID \
{ 0x8D9588B8, 0x8EE1, 0x4947, {0xA3, 0x7D, 0x09, 0x8A, 0x22, 0xF4, 0xAB, 0xDA} }

class HE_NO_VTABLE IWEPKeySet : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IWEPKeySet_IID);

	/// Get the key set name.
	HE_IMETHOD GetName(/*out*/ Helium::HEBSTR* pstrName) = 0;
	/// Set the key set name.
	HE_IMETHOD SetName(/*in*/ Helium::HEBSTR bstrName) = 0;
	/// Get the WEP algorithm \see PeekWEPAlgorithm.
	HE_IMETHOD GetAlgorithm(/*out*/ PeekWEPAlgorithm* pAlgorithm) = 0;
	/// Set the WEP algorithm.
	HE_IMETHOD SetAlgorithm(/*in*/ PeekWEPAlgorithm algorithm) = 0;
	/// Get the number of user bits when using a user-defined shared key.
	HE_IMETHOD GetUserBits(/*out*/ UInt32* pnUserBits) = 0;
	/// Set the number of user bits when using a user-defined shared key.
	HE_IMETHOD SetUserBits(/*in*/ UInt32 nUserBits) = 0;
	/// Get key 1.
	HE_IMETHOD GetKey1(/*out*/ PeekWEPKey* pKey) = 0;
	/// Set key 1.
	HE_IMETHOD SetKey1(/*in*/ const PeekWEPKey* pKey) = 0;
	/// Get key 2.
	HE_IMETHOD GetKey2(/*out*/ PeekWEPKey* pKey) = 0;
	/// Set key 2.
	HE_IMETHOD SetKey2(/*in*/ const PeekWEPKey* pKey) = 0;
	/// Get key 3.
	HE_IMETHOD GetKey3(/*out*/ PeekWEPKey* pKey) = 0;
	/// Set key 3.
	HE_IMETHOD SetKey3(/*in*/ const PeekWEPKey* pKey) = 0;
	/// Get key 4.
	HE_IMETHOD GetKey4(/*out*/ PeekWEPKey* pKey) = 0;
	/// Set key 4.
	HE_IMETHOD SetKey4(/*in*/ const PeekWEPKey* pKey) = 0;
};

/// \struct PeekWirelessChannelScanningSetting
/// \ingroup Adapter
/// \brief TODO
#include "hepushpack.h"
struct PeekWirelessChannelScanningSetting
{
	WirelessChannel		Channel;		///< Channel to scan.
	UInt32				Duration;		///< Duration in milliseconds.
	BOOL				Enabled;		///< Is this setting enabled?
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \enum PeekWirelessHardwareConfig
/// \ingroup Adapter
/// \brief Configurations for wireless hardware options.
/// \see IWirelessHardwareOptions
enum PeekWirelessHardwareConfig
{
	peekWirelessHardwareConfigChannel,		///< TODO
	peekWirelessHardwareConfigBSSID,		///< TODO
	peekWirelessHardwareConfigESSID,		///< TODO
	peekWirelessHardwareConfigScan			///< TODO
};

/// \internal
/// \interface IWirelessHardwareOptions
/// \ingroup Adapter
/// \brief Specific wireless hardware options.
/// \see coclass WirelessHardwareOptions
#define IWirelessHardwareOptions_IID \
{ 0x952EF12F, 0x2338, 0x4774, {0xB6, 0x83, 0x39, 0x53, 0xC7, 0xAD, 0xE7, 0x77} }

class HE_NO_VTABLE IWirelessHardwareOptions : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IWirelessHardwareOptions_IID);

	/// Get the configuration choice.
	HE_IMETHOD GetConfig(/*out*/ PeekWirelessHardwareConfig* pConfig) = 0;
	/// Set the configuration choice.
	HE_IMETHOD SetConfig(/*in*/ PeekWirelessHardwareConfig config) = 0;
	/// Get the channel.
	HE_IMETHOD GetChannel(/*out*/ WirelessChannel* pChannel) = 0;
	/// Set the channel.
	HE_IMETHOD SetChannel(/*in*/ WirelessChannel channel) = 0;
	/// Get the ESSID.
	HE_IMETHOD GetESSID(/*out*/ Helium::HEBSTR* pbstrESSID) = 0;
	/// Set the ESSID.
	HE_IMETHOD SetESSID(/*in*/ Helium::HEBSTR bstrESSID) = 0;
	/// Get the BSSID.
	HE_IMETHOD GetBSSID(/*out*/ UInt8 bssid[6]) = 0;
	/// Set the BSSID.
	HE_IMETHOD SetBSSID(/*in*/ const UInt8 bssid[6]) = 0;
	/// Get the key set.
	HE_IMETHOD GetKeySet(/*out*/ Helium::IHeUnknown** ppKeySet) = 0;
	/// Set the key set.
	HE_IMETHOD SetKeySet(/*in*/ Helium::IHeUnknown* pKeySet) = 0;
	/// Get the channel scanning settings.
	HE_IMETHOD GetChannelScanningSettings(/*out*/ PeekWirelessChannelScanningSetting** ppScanningSettings, /*out*/ UInt32* pnSettings) = 0;
	/// Set the channel scanning settings.
	HE_IMETHOD SetChannelScanningSettings(/*in*/ const PeekWirelessChannelScanningSetting* pScanningSettings, /*in*/ UInt32 nSettings) = 0;
};

/// \defgroup Filter Packet filtering interfaces and classes implemented by Omni.
/// \brief Packet filtering-related interfaces and classes.

/// \internal
/// \enum PeekFilterMode
/// \ingroup Filter
/// \brief Modes for filtering packets (accept all, accept matching, reject matching).
/// \see IPeekCapture
enum PeekFilterMode
{
	peekFilterModeAcceptAll,			///< Accept all packets.
	peekFilterModeAcceptMatchingAny,	///< Accept only packets matching any filters.
	peekFilterModeRejectAll,			///< Reject all packets.
	peekFilterModeRejectMatchingAny,	///< Reject packets matching any filters.
	peekFilterModeAcceptMatchingAll,	///< Accept only packets matching all filters.
	peekFilterModeRejectMatchingAll		///< Reject packets matching all filters.
};

/// \interface IFilterPacket
/// \ingroup Filter
/// \brief Filter and/or slice a packet.
///
/// Optional interface implemented by Distributed Analysis Modules and used by the OmniPeek console
/// and OmniEngine. This interface is exposed by a Distributed Analysis Module that needs to filter
/// and/or slice packets captured by the engine or to filter and/or slice packets that are selected
/// for filtering by Analysis Module in the console packet list.
#define IFilterPacket_IID \
{ 0x5F9062AD, 0x95DB, 0x4A5C, {0x9C, 0x9E, 0x80, 0x51, 0x67, 0xAE, 0xE1, 0xB3} }

class HE_NO_VTABLE IFilterPacket : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IFilterPacket_IID);

	/// Allow a Distributed Analysis Module to filter and/or slice packets.
	/// \param pPacket Pointer to an IPacket interface that allows the
	/// Distributed Analysis Module to get information about the packet.
	/// \param pcbBytesToAccept Pointer to a variable that receives the sliced length
	/// of the packet.
	/// \param pFilterID Pointer to the filter's globally unique identifier.
	/// \retval HE_S_OK Packet is accepted by the Distributed Analysis Module
	/// \retval HE_S_FALSE Packet is rejected by the Distributed Analysis Module
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid
	/// \remarks This interface should not be implemented if the Distributed Analysis Module
	/// does not need to filter or slice packets.
	/// The slice length of the packet should be zero if the Distributed Analysis Module is not slicing the packet.
	/// If the capture running on the engine is also slicing packets, the packet will be sliced
	/// at the lesser of the capture slice length and the Distributed Analysis Module slice length.
	HE_IMETHOD FilterPacket(/*in*/ IPacket* pPacket, /*out*/ UInt16* pcbBytesToAccept, /*out*/ Helium::HeID* pFilterID) = 0;
};

/// \interface IFilterFile
/// \ingroup Filter
/// \brief Filter a file.
///
/// Optional interface used by the OmniEngine. This interface needs to filter files captured by the engine.
#define IFilterFile_IID \
{ 0xF50E968E, 0xCDF3, 0x4CE5, {0x90, 0x83, 0x2B, 0x20, 0x10, 0x7D, 0xEF, 0xB6} }

class HE_NO_VTABLE IFilterFile : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IFilterFile_IID);

	/// Allow the OmniEngine to filter files.
	/// \param bstrPath Path to the file to be filtered.
	/// \param pFilterID Pointer to the filter's globally unique identifier.
	/// \retval HE_S_OK File is accepted
	/// \retval HE_S_FALSE File is rejected
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid
	/// \remarks This interface should not be implemented if the OmniEngine does not need to filter files.
	HE_IMETHOD FilterFile(/*in*/ Helium::HEBSTR bstrPath, /*out*/ Helium::HeID* pFilterID) = 0;
};

/// \internal
/// \enum PeekSelectRelatedParam
/// \ingroup Filter
/// \brief Params for selecting related packets (by source, by dest, by protocol, etc.)
enum PeekSelectRelatedParam
{
	peekSelectRelatedBySource,			///< Select by source address.
	peekSelectRelatedByDest,			///< Select by destination address.
	peekSelectRelatedBySourceAndDest,	///< Select by source and destination addresses.
	peekSelectRelatedByProtocol,		///< Select by protocol.
	peekSelectRelatedByPort,			///< Select by port.
	peekSelectRelatedByConversation,	///< Select by conversation (address and port or protocol).
	peekSelectRelatedByVlan,			///< Select by VLAN ID.
	peekSelectRelatedByApplication		///< Select by application.
};

/// \defgroup Packet Packet interfaces and classes implemented by Omni.
/// \brief Packet-related interfaces and classes.
///
/// These packet-related interfaces and classes are implemented by Omni for
/// use by Distributed Analysis Modules.

/// \defgroup PacketBuffer Packet buffer interfaces and classes implemented by Omni.
/// \brief Packet buffer-related interfaces and classes.

/// \struct PacketLayerInfo
/// \brief ProtoSpec, header, and payload information for a layer in a packet.
/// \see IPacketLayers
#include "hepushpack.h"
struct PacketLayerInfo
{
	UInt32			ulProtoSpec;	///< The ProtoSpec instance ID that identifies the layer.
	const UInt8*	pHeader;		///< Pointer to the start of the layer's header.
	const UInt8*	pPayload;		///< Pointer to the start of the layer's payload.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \enum PeekEncryptionType
/// \ingroup Packet
enum PeekEncryptionType
{
	peekEncryptionTypeNone,
	peekEncryptionTypeWEP,
	peekEncryptionTypeCKIP,
	peekEncryptionTypeTKIP
};

/// \enum PeekAuthenticationType
/// \ingroup Packet
enum PeekAuthenticationType
{
	peekAuthenticationTypeNone,
	peekAuthenticationTypeEAP,
	peekAuthenticationTypeLEAP,
	peekAuthenticationTypePEAP,
	peekAuthenticationTypeEAPTLS
};

/// \interface IPacket
/// \ingroup Packet
/// \brief Allows access to a variety of information about the packet.
#define IPacket_IID \
{ 0xC39C34DA, 0xF741, 0x4BCF, {0xBB, 0x7B, 0x1B, 0xDE, 0x01, 0xA8, 0x57, 0xD9} }

class HE_NO_VTABLE IPacket : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacket_IID);

	/// \internal
	/// Initialize a packet.
	HE_IMETHOD Initialize(/*in*/ IPacketBuffer* pPacketBuffer, /*in*/ UInt32 nPacketBufferIndex,
		/*in*/ PeekPacket* pPacketHeader, /*in*/ const UInt8* pPacketData,
		/*in*/ TMediaType mt, /*in*/ TMediaSubType mst) = 0;
	/// \internal
	/// Get direct access to the packet header.
	HE_IMETHOD GetPacketHeader(/*out*/ const PeekPacket** ppPeekPacket) = 0;
	/// Get direct access to the packet data.
	/// \param ppPacketData Pointer to a variable that receives a pointer to the start of the packet data.
	/// \retval HE_S_OK Success
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetPacketData(/*out*/ const UInt8** ppPacketData) = 0;
	/// Get direct access to one past the last valid byte of data, not including FCS bytes.
	/// \param ppPacketDataEnd Pointer to a variable that receives a pointer to one 
	/// past the end of the packet data. This is the first FCS byte if packet is 
	/// unsliced, or one past the end of the world if sliced.
	/// \retval HE_S_OK Success
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetPacketDataEnd(/*out*/ const UInt8** ppPacketDataEnd) = 0;
	/// \internal
	/// Get direct access to the packet media specific info.
	HE_IMETHOD GetPacketMediaSpecificInfo(/*out*/ const UInt8** ppMediaSpecificInfo,
		/*out*/ UInt32* pcbMediaSpecificInfoLength) = 0;
	/// \internal
	/// Get the packet buffer.
	HE_IMETHOD GetPacketBuffer(/*out*/ IPacketBuffer** ppPacketBuffer) = 0;
	/// \internal
	/// Get the packet buffer index.
	HE_IMETHOD GetPacketBufferIndex(/*out*/ UInt32* pIndex) = 0;
	/// \internal
	/// Get the packet number.
	HE_IMETHOD GetPacketNumber(/*out*/ UInt64* pPacketNumber) = 0;
	/// Get the packet media type.
	/// \param pMediaType Pointer to a variable that receives the media type.
	/// \retval HE_S_OK Success
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetMediaType(/*out*/ TMediaType* pMediaType) = 0;
	/// Get the packet media sub type.
	/// \param pMediaSubType Pointer to a variable that receives the media sub type.
	/// \retval HE_S_OK Success
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetMediaSubType(/*out*/ TMediaSubType* pMediaSubType) = 0;
	/// Get the packet length in bytes.
	/// \param pcbPacketLength Pointer to a variable that receives the packet length.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This is the length of the packet on the network.
	HE_IMETHOD GetPacketLength(/*out*/ UInt16* pcbPacketLength) = 0;
	/// Get the packet slice length in bytes.
	/// \param pcbSliceLength Pointer to a variable that receives the slice length.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This length is zero if the packet is not sliced.
	HE_IMETHOD GetSliceLength(/*out*/ UInt16* pcbSliceLength) = 0;
	/// Get the actual length of packet data in bytes.
	/// \param pcbActualLength Pointer to a variable that receives the actual length.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This is the length of the packet that is available for use by the OmniEngine. This
	/// length may be shorter than the length of the original packet on the network.
	HE_IMETHOD GetActualLength(/*out*/ UInt16* pcbActualLength) = 0;
	/// Get the packet flags bits.
	/// \param pFlags Pointer to a variable that receives the flags bits.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	/// \see PeekPacketFlags
	HE_IMETHOD GetFlags(/*out*/ UInt32* pFlags) = 0;
	/// Get the packet status bits.
	/// \param pStatus Pointer to a variable that receives the status bits.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	/// \see PeekPacketStatus
	HE_IMETHOD GetStatus(/*out*/ UInt32* pStatus) = 0;
	/// Get the packet timestamp.
	/// \param pnTimeStamp Pointer to a variable that receives the timestamp.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks The timestamp is nanoseconds since midnight 1/1/1601 UTC.
	HE_IMETHOD GetTimeStamp(/*out*/ UInt64* pnTimeStamp) = 0;
	/// Get the packet ProtoSpec instance ID.
	/// \param pProtoSpec Pointer to a variable that receives the ProtoSpec instance ID.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetProtoSpec(/*out*/ UInt32* pProtoSpec) = 0;
	/// Get the packet ProtoSpec ID.
	/// \param pProtoSpecID Pointer to a variable that receives the ProtoSpec ID.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetProtoSpecID(/*out*/ UInt16* pProtoSpecID) = 0;
	/// Set packet flags bits.
	/// \param f Flags bits to set.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	/// \see PeekPacketFlags
	HE_IMETHOD SetFlag(/*in*/ UInt32 f) = 0;
	/// Clear packet flags bits.
	/// \param f Flags bits to clear.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	/// \see PeekPacketFlags
	HE_IMETHOD ClearFlag(/*in*/ UInt32 f) = 0;
	/// Test packet flags bits.
	/// \param f Bits to be checked.
	/// \param pbResult Pointer to a boolean that is true if the bits being checked are set in the packet flags.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	/// \see PeekPacketFlags
	HE_IMETHOD TestFlag(/*in*/ UInt32 f,/*out*/ BOOL* pbResult) = 0;
	/// Set packet status bits.
	/// \param s Bits to be set.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	/// \see PeekPacketStatus
	HE_IMETHOD SetStatus(/*in*/ UInt32 s) = 0;
	/// Clear packet status bits.
	/// \param s Bits to be cleared.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	/// \see PeekPacketStatus
	HE_IMETHOD ClearStatus(/*in*/ UInt32 s) = 0;
	/// Test packet status bits.
	/// \param s Bits to be checked.
	/// \param pbResult Pointer to a boolean that is true if the bits being checked are set in the packet status.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	/// \see PeekPacketStatus
	HE_IMETHOD TestStatus(/*in*/ UInt32 s, /*out*/ BOOL* pbResult) = 0;
	/// Is this a wireless packet?
	/// \param pbWirelessPacket Pointer to a boolean that is true if the packet is a wireless packet.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD IsWirelessPacket(/*out*/ BOOL* pbWirelessPacket) = 0;
	/// Is this a full duplex packet?
	/// \param pbFullDuplexPacket Pointer to a boolean that is true if the packet is a full duplex packet.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD IsFullDuplexPacket(/*out*/ BOOL* pbFullDuplexPacket) = 0;
	/// Is this a WAN packet?
	/// \param pbWanPacket Pointer to a boolean that is true if the packet is a WAN packet.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD IsWanPacket(/*out*/ BOOL* pbWanPacket) = 0;
	/// Is this an error packet?
	/// \param pbError Pointer to a boolean that is true if the packet is an error packet.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD IsError(/*out*/ BOOL* pbError) = 0;
	/// Is this a multicast packet?
	/// \param pbMulticast Pointer to a boolean that is true if the packet is a multicast packet.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD IsMulticast(/*out*/ BOOL* pbMulticast) = 0;
	/// Is this a broadcast packet?
	/// \param pbBroadcast Pointer to a boolean that is true if the packet is a broadcast packet.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD IsBroadcast(/*out*/ BOOL* pbBroadcast) = 0;
	/// Check if the packet protocol is a subprotocol of a number of possible parent protocols.
	/// \param pParentArray An array of ProtoSpec IDs for the parent protocols.
	/// \param lCount The number of parent protocols.
	/// \param pulMatchInstanceID Pointer to a variable that receives the ProtoSpec instance ID if
	/// the protocol is a subprotocol of one of the parent protocols.
	/// This parameter may be NULL if this information is not needed.
	/// \param plMatchIndex Pointer to a variable the receives the index of the parent protocol if
	/// the protocol is a subprotocol of one of the parent protocols.
	/// This parameter may be NULL if this information is not needed.
	/// \param pbResult Pointer to a boolean that is true if the protocol is a subprotocol of one of the
	/// parent protocols.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_ARG One of the non-pointer parameters is invalid.
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks None
	HE_IMETHOD IsDescendentOf(/*in*/ const UInt16 pParentArray[],
		/*in*/ SInt32 lCount, /*out*/ UInt32* pulMatchInstanceID,
		/*out*/ SInt32* plMatchIndex, /*out*/ BOOL* pbResult) = 0;
	/// Is this a wireless or IPv4 fragment?
	HE_IMETHOD IsFragment(/*out*/ BOOL* pbFragment) = 0;
	/// Get the IP fragment offset value.
	/// \retval HE_S_OK if this is a fragment and outputting a valid fragment offset
	/// \retval HE_S_FALSE if this is not a fragment (outputs fragment offset of 0)
	HE_IMETHOD GetIPFragmentOffset(/*out*/ UInt16* pFragmentOffset) = 0;
	/// Get the IP fragment flags.
	/// Mask with 0xE000 to get the actual flags.
	/// 0x8000 = reserved
	/// 0x4000 = may fragment
	/// 0x2000 = more fragments
	///
	/// Lower bits (0x1FFF mask) is the fragment offset, although you need to multiply
	/// by 8 (or shift <<3) to get the actual offset.
	///
	/// \retval HE_S_OK if this is a fragment and outputting a valid fragment bits
	/// \retval HE_S_FALSE if this is not a fragment (outputs fragment offset of 0)
	///			or unable to find IP fragment bits (not an IPv4 packet?)
	HE_IMETHOD GetIPFragmentBits(/*out*/ UInt16* pFragmentBits) = 0;
	/// Get the IPID of this packet. Return HE_E_FAIL if not an IPv4 packet.
	HE_IMETHOD GetIPID(/*out*/ UInt16* pIPID) = 0;
	/// Get the IP Length field (length of packet from start of IP header to 
	/// end of IP data, not including MAC header or FCS suffix)
	HE_IMETHOD GetIPLength(/*out*/ UInt16* pIPLength) = 0;
	/// Get application-specific packet parsing information.
	HE_IMETHOD GetAppLayer(/*out*/ Helium::IHeUnknown** ppAppLayer) = 0;
	/// Set application-specific packet parsing information.
	HE_IMETHOD SetAppLayer(/*in*/ Helium::IHeUnknown* pAppLayer) = 0;
	/// Get the packet flow ID.
	HE_IMETHOD GetFlowID(/*out*/ UInt32* pFlowID) = 0;
	/// Set the packet flow ID.
	HE_IMETHOD SetFlowID(/*in*/ UInt32 nFlowID) = 0;
	/// Get the packet VoIP call ID.
	HE_IMETHOD GetVoIPCallID(/*out*/ UInt64* pCallID) = 0;
	/// Set the packet VoIP call ID.
	HE_IMETHOD SetVoIPCallID(/*in*/ UInt64 nCallID) = 0;
	/// Get the packet VoIP call flow index.
	HE_IMETHOD GetVoIPCallFlowIndex(/*out*/ UInt32* pCallFlowIndex) = 0;
	/// Set the packet VoIP call flow index.
	HE_IMETHOD SetVoIPCallFlowIndex(/*in*/ UInt32 nCallFlowIndex) = 0;

	/// Get all packet layers.
	/// \param pulLayerCount On input, pointer to a variable that specifies the number of layers that
	/// can be handled in the PacketLayerInfo array. On output, this variable receives the
	/// actual number of layers in the packet.
	/// \param pLayerInfo Pointer to an array of PacketLayerInfo structures.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_ARG The layer count passed as input is not large enough for the actual number
	/// of layers in the packet.
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks None
	HE_IMETHOD GetPacketLayers(/*in,out*/ UInt32* pulLayerCount,
		/*out*/ PacketLayerInfo pLayerInfo[]) = 0;
	/// Get the header layer associated with a ProtoSpec.
	/// \param usProtoSpecID ProtoSpec ID
	/// \param pulSourceProtoSpec Pointer to a variable that receives the ProtoSpec instance ID.
	/// This parameter may be NULL if this information is not needed.
	/// \param ppData Pointer to a variable that receives a pointer to the header layer of the packet that
	/// corresponds to the ProtoSpec.
	/// \param pcbBytesRemaining Pointer to a variable that receives the number of bytes remaining in the packet.
	/// This parameter may be NULL if this information is not needed.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks None
	HE_IMETHOD GetHeaderLayer(/*in*/ UInt16 usProtoSpecID, /*out*/ UInt32* pulSourceProtoSpec,
		/*out*/ const UInt8** ppData, /*out*/ UInt16* pcbBytesRemaining) = 0;
	/// Get the data layer associated with a ProtoSpec.
	/// \param usProtoSpecID ProtoSpec ID
	/// \param pulSourceProtoSpec Pointer to a variable that receives the ProtoSpec instance ID.
	/// This parameter may be NULL if this information is not needed.
	/// \param ppData Pointer to a variable that receives a pointer to the packet data that
	/// corresponds to layer specified by the ProtoSpec ID.
	/// \param pcbBytesRemaining Pointer to a variable that receives the number of bytes remaining in the packet.
	/// This parameter may be NULL if this information is not needed.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks None
	HE_IMETHOD GetDataLayer(/*in*/ UInt16 usProtoSpecID, /*out*/ UInt32* pulSourceProtoSpec,
		/*out*/ const UInt8** ppData, /*out*/ UInt16* pcbBytesRemaining) = 0;

	/// Get the packet protocol in ProtoSpec format.
	/// \param pSpec Pointer to a TMediaSpec that is filled with protocol information.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks Use GetRawProtocol to get the "raw" protocol.
	HE_IMETHOD GetProtocol(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the "raw" packet protocol (e.g., SNAP instead of ProtoSpec).
	/// \param pSpec Pointer to a TMediaSpec that is filled with "raw" protocol information.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks Use GetProtocol to get the ProtoSpec corresponding to the protocol.
	HE_IMETHOD GetRawProtocol(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet source physical address.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the source physical address.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetSrcPhysical(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet destination physical address.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the destination physical address.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetDestPhysical(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet source logical address.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the source logical address.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetSrcLogical(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet destination logical address.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the destination logical address.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetDestLogical(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet source port.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the source port.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetSrcPort(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet destination port.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the destination port.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetDestPort(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet BSSID.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the BSSID.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This method should only be used on wireless packets.
	HE_IMETHOD GetBSSID(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet receiver.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the packet receiver.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This method should only be used on wireless packets.
	HE_IMETHOD GetReceiver(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet transmitter.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the packet transmitter.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This method should only be used on wireless packets.
	HE_IMETHOD GetTransmitter(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet address 1.
	/// \param pSpec Pointer to a TMediaSpec that is filled with address 1.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This method should only be used on wireless packets.
	HE_IMETHOD GetAddress1(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet address 2.
	/// \param pSpec Pointer to a TMediaSpec that is filled with address 2.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This method should only be used on wireless packets.
	HE_IMETHOD GetAddress2(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet address 3.
	/// \param pSpec Pointer to a TMediaSpec that is filled with address 3.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This method should only be used on wireless packets.
	HE_IMETHOD GetAddress3(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet address 4.
	/// \param pSpec Pointer to a TMediaSpec that is filled with address 4.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This method should only be used on wireless packets.
	HE_IMETHOD GetAddress4(/*out*/ TMediaSpec* pSpec) = 0;
	/// Determine if the transmitter is implied.
	///
	/// 802.11 control packets of type ACK and CTS only carry the receiver address.
	/// For ACK packets, the transmitter is implied from the "immediately previous
	/// directed data, management, or PS-POLL control frame".
	/// For CTS packets, the transmitter is implied from the receiver in the
	/// immediately preceding RTS frame.
	/// \param pbImplied Pointer to bool that indicates if the transmitter is implied.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This method should only be used on wireless packets.
	/// GetTransmitter or GetSrcPhysical must be called first.
	HE_IMETHOD IsTransmitterImplied(/*out*/ BOOL* pbImplied) = 0;

	/// Reset cached entity table index.
	HE_IMETHOD ResetEntityIndexes() = 0;
	/// Get the protocol entity table index.
	HE_IMETHOD GetProtocolIndex(/*out*/ UInt32* pIndex) = 0;
	/// Set the protocol entity table index.
	HE_IMETHOD SetProtocolIndex(/*in*/ UInt32 index) = 0;
	/// Get the source physical address entity table index.
	HE_IMETHOD GetSrcPhysicalIndex(/*out*/ UInt32* pIndex) = 0;
	/// Set the source physical address entity table index.
	HE_IMETHOD SetSrcPhysicalIndex(/*in*/ UInt32 index) = 0;
	/// Get the destination physical address table index.
	HE_IMETHOD GetDestPhysicalIndex(/*out*/ UInt32* pIndex) = 0;
	/// Set the destination physical address entity table index.
	HE_IMETHOD SetDestPhysicalIndex(/*in*/ UInt32 index) = 0;
	/// Get the source logical address entity table index.
	HE_IMETHOD GetSrcLogicalIndex(/*out*/ UInt32* pIndex) = 0;
	/// Set the source logical address entity table index.
	HE_IMETHOD SetSrcLogicalIndex(/*in*/ UInt32 index) = 0;
	/// Get the destination logical address entity table index.
	HE_IMETHOD GetDestLogicalIndex(/*out*/ UInt32* pIndex) = 0;
	/// Set the destination logical address entity table index.
	HE_IMETHOD SetDestLogicalIndex(/*in*/ UInt32 index) = 0;

	/// Get the full duplex channel.
	/// \param pnChannel Pointer to a variable that receives the full duplex channel number.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks The channel number starts at one.
	HE_IMETHOD GetFullDuplexChannel(/*out*/ UInt32* pnChannel) = 0;

	/// Get the wireless channel.
	/// \param pChannel Pointer to a variable that receives the channel info
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetWirelessChannel(/*out*/ WirelessChannel* pChannel) = 0;
	/// Get the wireless data rate.
	/// \param pDataRate Pointer to a variable that receives the data rate.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetDataRate(/*out*/ UInt16* pDataRate) = 0;
	/// Get the wireless N flags.
	/// \param pFlags Pointer to a variable that receives the flags.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetFlagsN(/*out*/ UInt32* pFlags) = 0;
	/// Get the wireless signal level.
	/// \param pSignalLevel Pointer to a variable that receives the signal level.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetSignalLevel(/*out*/ UInt8* pSignalLevel) = 0;
	/// Get the wireless signal level in dBm.
	/// \param pSignaldBm Pointer to a variable that receives the signal level in dBm.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetSignaldBm(/*out*/ SInt8* pSignaldBm) = 0;
	/// Get the wireless N signal level in dBm.
	/// \param nStream 802.11N channel stream
	/// \param pSignaldBm Pointer to a variable that receives the signal level in dBm.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetSignaldBmN(/*in*/ UInt8 nStream, /*out*/ SInt8* pSignaldBm) = 0;
	/// Get the wireless noise level.
	/// \param pNoiseLevel Pointer to a variable that receives the noise level.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetNoiseLevel(/*out*/ UInt8* pNoiseLevel) = 0;
	/// Get the wireless noise level in dBm.
	/// \param pNoisedBm Pointer to a variable that receives the noise level in dBm.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetNoisedBm(/*out*/ SInt8* pNoisedBm) = 0;
	/// Get the wireless N noise level in dBm.
	/// \param nStream 802.11N channel stream
	/// \param pNoisedBm Pointer to a variable that receives the noise level in dBm.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetNoisedBmN(/*in*/ UInt8 nStream, /*out*/ SInt8* pNoisedBm) = 0;
	/// Get the transmitter encryption type.
	/// \param pEncryptionType Pointer to an EncryptionType enum that receives the encryption type.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetTransmitterEncryptionType(/*out*/ PeekEncryptionType* pEncryptionType) = 0;
	/// Get the receiver encryption type.
	/// \param pEncryptionType Pointer to an EncryptionType enum that receives the encryption type.
	/// receives the authentication type.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetReceiverEncryptionType(/*out*/ PeekEncryptionType* pEncryptionType) = 0;
	/// Get the transmitter authentication type.
	/// \param pAuthenticationType Pointer to an AuthenticationType enum that
	/// receives the authentication type.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetTransmitterAuthenticationType(/*out*/ PeekAuthenticationType* pAuthenticationType) = 0;
	/// Get the WEP key index for a WEP encrypted packet.
	/// \param pWEPKeyIndex Pointer to a variable that receives the WEP key index.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks Returns HE_S_OK and index -1 if there is no WEP key index.
	HE_IMETHOD GetWEPKeyIndex(/*out*/ SInt32* pWEPKeyIndex) = 0;
	/// Return the 8-bit wireless control flags
	/// \param pFlags Pointer to a variable that receives the 8-bit 802.11 control flags.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetWirelessControlFlags(/*out*/ UInt8* pFlags) = 0;
	/// Return the 4-bit wireless fragment number for this packet. Zero if not a fragment, or if first fragment.
	/// \param pFragmentNumber Pointer to a variable that receives the 4-bit 802.11 control flags
	/// \retval HE_S_OK Success
	/// \retval HE_S_FALSE Success, but this isn't a fragment (the fragment number is zero and the More Fragments control flag is clear).
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetWirelessFragmentNumber(/*out*/ UInt8* pFragmentNumber) = 0;

	/// Get the WAN direction.
	/// \param pWanDirection Pointer to a variable that receives the WAN direction.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetWanDirection(/*out*/ UInt8* pWanDirection) = 0;
	/// Get the WAN protocol.
	/// \param pWanProtocol Pointer to a variable that receives the WAN protocol.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetWanProtocol(/*out*/ UInt8* pWanProtocol) = 0;

	/// 32-bit TCP sequence number.
	HE_IMETHOD GetTCPSequence(/*out*/ UInt32* pVal) = 0;
	/// 32-bit TCP ACK number.
	HE_IMETHOD GetTCPAck(/*out*/ UInt32* pVal) = 0;
	/// 6-bit TCP flags. FIN=0x01, SYN=0x02, RST=0x04, PSH=0x08, ACK=0x10, URG=0x20.
	HE_IMETHOD GetTCPFlags(/*out*/ UInt8* pVal) = 0;
	/// The 16-bit TCP Window value in the packet, does not factor in any TCP Window Scaling
	HE_IMETHOD GetTCPWindow(/*out*/ UInt16* pVal) = 0;
	/// The 16-bit TCP Checksum.
	HE_IMETHOD GetTCPChecksum(/*out*/ UInt16* pVal) = 0;
	/// The 16-bit TCP Urgent Pointer.
	HE_IMETHOD GetTCPUrgentPointer(/*out*/ UInt16* pVal) = 0;
	/// The first TCP option of the requested option type, HE_E_FAIL if option not present.
	HE_IMETHOD GetTCPOption(/*in*/ UInt8 optionType, /*out, size_is(,*pcbData)*/ const UInt8** ppData, /*out*/ UInt32* pcbData) = 0;
	/// 16-bit TCP Maximum Segment Size option, HE_E_FAIL if not present.
	HE_IMETHOD GetTCPOptionMSS(/*out*/ UInt16* pVal) = 0;
	/// 8-bit TCP Window Scale option, HE_E_FAIL if not present.
	/// How many right-shifts to apply to all TCP Window values to
	/// future packets from this source address on this TCP flow.
	HE_IMETHOD GetTCPOptionWindowScale(/*out*/ UInt8* pVal) = 0;
	/// 64-bit TCP Selective ACK option, HE_E_FAIL if not present.
	/// Split into 32-bit begin and end values.
	HE_IMETHOD GetTCPOptionSACK(/*out*/ UInt32* pBegin, /*out*/ UInt32* pEnd) = 0;
	/// 64-bit TCP Timestamp option, HE_E_FAIL if not present.
	/// Split into 32-bit timestamp and 32-bit echo.
	HE_IMETHOD GetTCPOptionTimestamp(/*out*/ UInt32* pVal, /*out*/ UInt32* pEcho) = 0;

	/// Get the packet frame check sequence.
	HE_IMETHOD GetFrameCheckSequence(/*out*/ UInt32* pFrameCheckSequence) = 0;

	/// Get the wireless data rate. If MCS index is received, calculate corresponding data rate.
	/// \param pDataRate Pointer to a variable that receives the calculated data rate.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetCalculatedDataRate(/*out*/ double* pDataRate) = 0;
	/// Get the MCS index value.
	/// \param pMCS Pointer to a variable that receives the MCS index.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetMCSIndex(/*out*/ UInt16* pMCS) = 0;
	/// Get the number of spatial streams.
	/// \param pSpatialStreams Pointer to a variable that receives the number of spatial streams.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetSpatialStreams(/*out*/ UInt32* pSpatialStreams) = 0;

	/// Get all packet layers.
	/// \param pulLayerCount On output, this variable receives the
	/// actual number of layers in the packet.
	/// \param ppLayerInfo On output, pointer to an array of PacketLayerInfo structures.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// of layers in the packet.
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks None
	HE_IMETHOD GetPacketLayers2(/*out*/ UInt32* pulLayerCount,
		/*out*/ const PacketLayerInfo** ppLayerInfo) = 0;

	/// Get the VLAN IDs in reverse order.
	/// \param ppIDs On output, this variable receives the array of VLAN IDs.
	/// \param pnIDs On output, this variable receives the number of
	/// VLAN IDs returned in ppIDs.
	HE_IMETHOD GetVLANIDs(/*out*/ UInt16** ppIDs, /*out*/ UInt32* pnIDs) = 0;
	/// Get the MPLS Labels in reverse order.
	/// \param ppLabels On output, this variable receives the array of MPLS Labels.
	/// \param pnLabels On output, this variable receives the number of
	/// MPLS Labels returned in ppLabels.
	HE_IMETHOD GetMPLSLabels(/*out*/ UInt32** ppLabels, /*out*/ UInt32* pnLabels) = 0;

	/// Get the application id as a TMediaSpec.
	/// \param pSpec Pointer to a TMediaSpec that is filled with application information.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	HE_IMETHOD GetApplication(/*out*/ TMediaSpec* pSpec) = 0;

	/// Get the ESSID.
	/// \param ppESSID Pointer to a variable that receives a pointer to the ESSID of the packet.
	/// \param pcbESSIDLength Pointer to a variable that receives the length of the ESSID in bytes.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	HE_IMETHOD GetESSID(/*out*/ const UInt8** ppESSID, /*out*/ UInt8* pcbESSIDLength) = 0;
};

/// \interface IFullDuplexPacket
/// \ingroup Packet
/// \brief Information about a packet received on a full duplex network.
/// \deprecated Use IPacket.
///
/// Provides access to full duplex information related to a packet.
#define IFullDuplexPacket_IID \
{ 0x6BEF432C, 0x41C9, 0x463A, {0xB2, 0x2D, 0x3A, 0x34, 0xF0, 0x8F, 0xA4, 0x39} }

class HE_NO_VTABLE IFullDuplexPacket : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IFullDuplexPacket_IID);

	/// Get the full duplex channel.
	/// \param pnChannel Pointer to a variable that receives the full duplex channel number.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks The channel number starts at one.
	HE_IMETHOD GetFullDuplexChannel(/*out*/ UInt32* pnChannel) = 0;
};

/// \interface IWirelessPacket
/// \ingroup Packet
/// \brief Information about a wireless packet.
/// \deprecated Use IPacket.
///
/// Provides access to wireless information related to a packet.
#define IWirelessPacket_IID \
{ 0x3C5E607D, 0x758B, 0x4F37, {0x9F, 0xA0, 0x8E, 0x28, 0xC5, 0xFE, 0x6C, 0x0A} }

class HE_NO_VTABLE IWirelessPacket : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IWirelessPacket_IID);

	/// Get the wireless channel.
	/// \param pChannel Pointer to a variable that receives the channel info
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetWirelessChannel(/*out*/ WirelessChannel* pChannel) = 0;
	/// Get the wireless data rate.
	/// \param pDataRate Pointer to a variable that receives the data rate.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetDataRate(/*out*/ UInt16* pDataRate) = 0;
	/// Get the wireless N flags.
	/// \param pFlags Pointer to a variable that receives the flags.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetFlagsN(/*out*/ UInt32* pFlags) = 0;
	/// Get the wireless signal level.
	/// \param pSignalLevel Pointer to a variable that receives the signal level.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetSignalLevel(/*out*/ UInt8* pSignalLevel) = 0;
	/// Get the wireless signal level in dBm.
	/// \param pSignaldBm Pointer to a variable that receives the signal level in dBm.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetSignaldBm(/*out*/ SInt8* pSignaldBm) = 0;
	/// Get the wireless N signal level in dBm.
	/// \param nStream 802.11N channel stream
	/// \param pSignaldBm Pointer to a variable that receives the signal level in dBm.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetSignaldBmN(/*in*/ UInt8 nStream, /*out*/ SInt8* pSignaldBm) = 0;
	/// Get the wireless noise level.
	/// \param pNoiseLevel Pointer to a variable that receives the noise level.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetNoiseLevel(/*out*/ UInt8* pNoiseLevel) = 0;
	/// Get the wireless noise level in dBm.
	/// \param pNoisedBm Pointer to a variable that receives the noise level in dBm.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetNoisedBm(/*out*/ SInt8* pNoisedBm) = 0;
	/// Get the wireless N noise level in dBm.
	/// \param nStream 802.11N channel stream
	/// \param pNoisedBm Pointer to a variable that receives the noise level in dBm.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetNoisedBmN(/*in*/ UInt8 nStream, /*out*/ SInt8* pNoisedBm) = 0;
	/// Get the transmitter encryption type.
	/// \param pEncryptionType Pointer to an EncryptionType enum that receives the encryption type.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetTransmitterEncryptionType(/*out*/ PeekEncryptionType* pEncryptionType) = 0;
	/// Get the receiver encryption type.
	/// \param pEncryptionType Pointer to an EncryptionType enum that receives the encryption type.
	/// receives the authentication type.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetReceiverEncryptionType(/*out*/ PeekEncryptionType* pEncryptionType) = 0;
	/// Get the transmitter authentication type.
	/// \param pAuthenticationType Pointer to an AuthenticationType enum that
	/// receives the authentication type.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetTransmitterAuthenticationType(/*out*/ PeekAuthenticationType* pAuthenticationType) = 0;
	/// Get the WEP key index for a WEP encrypted packet.
	/// \param pWEPKeyIndex Pointer to a variable that receives the WEP key index.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks Returns HE_S_OK and index -1 if there is no WEP key index.
	HE_IMETHOD GetWEPKeyIndex(/*out*/ SInt32* pWEPKeyIndex) = 0;
	/// Return the 8-bit wireless control flags
	/// \param pFlags Pointer to a variable that receives the 8-bit 802.11 control flags.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetWirelessControlFlags(/*out*/ UInt8* pFlags) = 0;
	/// Return the 4-bit wireless fragment number for this packet. Zero if not a fragment, or if first fragment.
	/// \param pFragmentNumber Pointer to a variable that receives the 4-bit 802.11 control flags
	/// \retval HE_S_OK Success
	/// \retval HE_S_FALSE Success, but this isn't a fragment (the fragment number is zero and the More Fragments control flag is clear).
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetWirelessFragmentNumber(/*out*/ UInt8* pFragmentNumber) = 0;
	/// Get the wireless data rate. If MCS index is received, calculate corresponding data rate.
	/// \param pDataRate Pointer to a variable that receives the calculated data rate.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetCalculatedDataRate(/*out*/ double* pDataRate) = 0;
	/// Get the MCS index value.
	/// \param pMCS Pointer to a variable that receives the MCS index.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetMCSIndex(/*out*/ UInt16* pMCS) = 0;
	/// Get the number of spatial streams.
	/// \param pSpatialStreams Pointer to a variable that receives the number of spatial streams.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetSpatialStreams(/*out*/ UInt32* pSpatialStreams) = 0;
};

/// \interface IWanPacket
/// \ingroup Packet
/// \brief Information about a WAN packet.
/// \deprecated Use IPacket.
///
/// Provides access to WAN information related to a packet.
#define IWanPacket_IID \
{ 0x67AFFB12, 0xDC17, 0x4958, {0xA7, 0xFE, 0x57, 0x6C, 0x3A, 0x77, 0x23, 0xE2} }

class HE_NO_VTABLE IWanPacket : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IWanPacket_IID);

	/// Get the WAN direction.
	/// \param pWanDirection Pointer to a variable that receives the WAN direction.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetWanDirection(/*out*/ UInt8* pWanDirection) = 0;
	/// Get the WAN protocol.
	/// \param pWanProtocol Pointer to a variable that receives the WAN protocol.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetWanProtocol(/*out*/ UInt8* pWanProtocol) = 0;
};

/// \interface ITCPPacket
/// \ingroup Packet
/// \brief Access to common TCP fields like sequence, flags, ACK.
/// \deprecated Use IPacket.
#define ITCPPacket_IID \
{ 0x64CB9FDD, 0x5A8B, 0x48B1, {0x80, 0x07, 0x8C, 0xEB, 0x8D, 0x11, 0x2D, 0x39} }

class HE_NO_VTABLE ITCPPacket : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ITCPPacket_IID);

	/// 32-bit TCP sequence number.
	HE_IMETHOD GetTCPSequence(/*out*/ UInt32* pVal) = 0;
	/// 32-bit TCP ACK number.
	HE_IMETHOD GetTCPAck(/*out*/ UInt32* pVal) = 0;
	/// 6-bit TCP flags. FIN=0x01, SYN=0x02, RST=0x04, PSH=0x08, ACK=0x10, URG=0x20.
	HE_IMETHOD GetTCPFlags(/*out*/ UInt8* pVal) = 0;
	/// The 16-bit TCP Window value in the packet, does not factor in any TCP Window Scaling
	HE_IMETHOD GetTCPWindow(/*out*/ UInt16* pVal) = 0;
	/// The 16-bit TCP Checksum.
	HE_IMETHOD GetTCPChecksum(/*out*/ UInt16* pVal) = 0;
	/// The 16-bit TCP Urgent Pointer.
	HE_IMETHOD GetTCPUrgentPointer(/*out*/ UInt16* pVal) = 0;
	/// The first TCP option of the requested option type, HE_E_FAIL if option not present.
	HE_IMETHOD GetTCPOption(/*in*/ UInt8 optionType, /*out, size_is(,*pcbData)*/ const UInt8** ppData, /*out*/ UInt32* pcbData) = 0;
	/// 16-bit TCP Maximum Segment Size option, HE_E_FAIL if not present.
	HE_IMETHOD GetTCPOptionMSS(/*out*/ UInt16* pVal) = 0;
	/// 8-bit TCP Window Scale option, HE_E_FAIL if not present.
	/// How many right-shifts to apply to all TCP Window values to
	/// future packets from this source address on this TCP flow.
	HE_IMETHOD GetTCPOptionWindowScale(/*out*/ UInt8* pVal) = 0;
	/// 64-bit TCP Selective ACK option, HE_E_FAIL if not present.
	/// Split into 32-bit begin and end values.
	HE_IMETHOD GetTCPOptionSACK(/*out*/ UInt32* pBegin, /*out*/ UInt32* pEnd) = 0;
	/// 64-bit TCP Timestamp option, HE_E_FAIL if not present.
	/// Split into 32-bit timestamp and 32-bit echo.
	HE_IMETHOD GetTCPOptionTimestamp(/*out*/ UInt32* pVal, /*out*/ UInt32* pEcho) = 0;
};

/// \interface IPacketLayers
/// \ingroup Packet
/// \brief Access protocol layers in a packet.
/// \deprecated Use IPacket.
///
/// This interface is implemented by the OmniEngine and provides access to the
/// protocol layers found in a packet.
#define IPacketLayers_IID \
{ 0xBE66DD79, 0x1ED7, 0x4F91, {0xB2, 0xC8, 0x68, 0x5D, 0x2B, 0x06, 0xFA, 0xF3} }

class HE_NO_VTABLE IPacketLayers : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketLayers_IID);

	/// Get all packet layers.
	/// \param pulLayerCount On input, pointer to a variable that specifies the number of layers that
	/// can be handled in the LayerInfo array. On output, this variable receives the
	/// actual number of layers in the packet.
	/// \param pLayerInfo Pointer to an array of PacketLayerInfo structures.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_ARG The layer count passed as input is not large enough for the actual number
	/// of layers in the packet.
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks None
	HE_IMETHOD GetPacketLayers(/*in,out*/ UInt32* pulLayerCount,
		/*out*/ PacketLayerInfo pLayerInfo[]) = 0;
	/// Get the header layer associated with a ProtoSpec.
	/// \param usProtoSpecID ProtoSpec ID
	/// \param pulSourceProtoSpec Pointer to a variable that receives the ProtoSpec instance ID.
	/// This parameter may be NULL if this information is not needed.
	/// \param ppData Pointer to a variable that receives a pointer to the header layer of the packet that
	/// corresponds to the ProtoSpec.
	/// \param pcbBytesRemaining Pointer to a variable that receives the number of bytes remaining in the packet.
	/// This parameter may be NULL if this information is not needed.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks None
	HE_IMETHOD GetHeaderLayer(/*in*/ UInt16 usProtoSpecID, /*out*/ UInt32* pulSourceProtoSpec,
		/*out*/ const UInt8** ppData, /*out*/ UInt16* pcbBytesRemaining) = 0;
	/// Get the data layer associated with a ProtoSpec.
	/// \param usProtoSpecID ProtoSpec ID
	/// \param pulSourceProtoSpec Pointer to a variable that receives the ProtoSpec instance ID.
	/// This parameter may be NULL if this information is not needed.
	/// \param ppData Pointer to a variable that receives a pointer to the packet data that
	/// corresponds to layer specified by the ProtoSpec ID.
	/// \param pcbBytesRemaining Pointer to a variable that receives the number of bytes remaining in the packet.
	/// This parameter may be NULL if this information is not needed.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks None
	HE_IMETHOD GetDataLayer(/*in*/ UInt16 usProtoSpecID, /*out*/ UInt32* pulSourceProtoSpec,
		/*out*/ const UInt8** ppData, /*out*/ UInt16* pcbBytesRemaining) = 0;
	/// Get all packet layers.
	/// \param pulLayerCount On output, this variable receives the
	/// actual number of layers in the packet.
	/// \param ppLayerInfo On output, pointer to an array of PacketLayerInfo structures.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// of layers in the packet.
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks None
	HE_IMETHOD GetPacketLayers2(/*out*/ UInt32* pulLayerCount,
		/*out*/ const PacketLayerInfo** ppLayerInfo) = 0;
};

/// \interface IPacketSpecs
/// \ingroup Packet
/// \brief Get addresses and protocol information from a packet.
/// \deprecated Use IPacket.
///
/// This interface is implemented by the OmniEngine and provides access to address
/// and protocol information in a packet.
#define IPacketSpecs_IID \
{ 0x068E6EBE, 0xD67A, 0x4B6D, {0x93, 0x3C, 0x65, 0x8A, 0x09, 0xD0, 0x4E, 0x5D} }

class HE_NO_VTABLE IPacketSpecs : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketSpecs_IID);

	/// Get the packet protocol in ProtoSpec format.
	/// \param pSpec Pointer to a TMediaSpec that is filled with protocol information.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks Use GetRawProtocol to get the "raw" protocol.
	HE_IMETHOD GetProtocol(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the "raw" packet protocol (e.g., SNAP instead of ProtoSpec).
	/// \param pSpec Pointer to a TMediaSpec that is filled with "raw" protocol information.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks Use GetProtocol to get the ProtoSpec corresponding to the protocol.
	HE_IMETHOD GetRawProtocol(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet source physical address.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the source physical address.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetSrcPhysical(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet destination physical address.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the destination physical address.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetDestPhysical(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet source logical address.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the source logical address.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetSrcLogical(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet destination logical address.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the destination logical address.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetDestLogical(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet source port.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the source port.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetSrcPort(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet destination port.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the destination port.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetDestPort(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet BSSID.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the BSSID.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This method should only be used on wireless packets.
	HE_IMETHOD GetBSSID(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet receiver.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the packet receiver.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This method should only be used on wireless packets.
	HE_IMETHOD GetReceiver(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet transmitter.
	/// \param pSpec Pointer to a TMediaSpec that is filled with the packet transmitter.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This method should only be used on wireless packets.
	HE_IMETHOD GetTransmitter(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet address 1.
	/// \param pSpec Pointer to a TMediaSpec that is filled with address 1.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This method should only be used on wireless packets.
	HE_IMETHOD GetAddress1(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet address 2.
	/// \param pSpec Pointer to a TMediaSpec that is filled with address 2.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This method should only be used on wireless packets.
	HE_IMETHOD GetAddress2(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet address 3.
	/// \param pSpec Pointer to a TMediaSpec that is filled with address 3.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This method should only be used on wireless packets.
	HE_IMETHOD GetAddress3(/*out*/ TMediaSpec* pSpec) = 0;
	/// Get the packet address 4.
	/// \param pSpec Pointer to a TMediaSpec that is filled with address 4.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This method should only be used on wireless packets.
	HE_IMETHOD GetAddress4(/*out*/ TMediaSpec* pSpec) = 0;
	/// Determine if the transmitter is implied.
	///
	/// 802.11 control packets of type ACK and CTS only carry the receiver address.
	/// For ACK packets, the transmitter is implied from the "immediately previous
	/// directed data, management, or PS-POLL control frame".
	/// For CTS packets, the transmitter is implied from the receiver in the
	/// immediately preceding RTS frame.
	/// \param pbImplied Pointer to bool that indicates if the transmitter is implied.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks This method should only be used on wireless packets.
	/// GetTransmitter or GetSrcPhysical must be called first.
	HE_IMETHOD IsTransmitterImplied(/*out*/ BOOL* pbImplied) = 0;
};

/// \internal
/// \interface IPacketEntities
/// \ingroup Packet
/// \brief Get and set cached entity table indices in a packet.
#define IPacketEntities_IID \
{ 0x9AE59C9A, 0x1721, 0x4BDD, { 0x97, 0x95, 0x0A, 0x31, 0xCF, 0x50, 0x6E, 0x00 } }

class HE_NO_VTABLE IPacketEntities : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketEntities_IID);

	/// Reset cached entity table index.
	HE_IMETHOD ResetEntityIndexes() = 0;
	/// Get the protocol entity table index.
	HE_IMETHOD GetProtocolIndex(/*out*/ UInt32* pIndex) = 0;
	/// Set the protocol entity table index.
	HE_IMETHOD SetProtocolIndex(/*in*/ UInt32 index) = 0;
	/// Get the source physical address entity table index.
	HE_IMETHOD GetSrcPhysicalIndex(/*out*/ UInt32* pIndex) = 0;
	/// Set the source physical address entity table index.
	HE_IMETHOD SetSrcPhysicalIndex(/*in*/ UInt32 index) = 0;
	/// Get the destination physical address table index.
	HE_IMETHOD GetDestPhysicalIndex(/*out*/ UInt32* pIndex) = 0;
	/// Set the destination physical address entity table index.
	HE_IMETHOD SetDestPhysicalIndex(/*in*/ UInt32 index) = 0;
	/// Get the source logical address entity table index.
	HE_IMETHOD GetSrcLogicalIndex(/*out*/ UInt32* pIndex) = 0;
	/// Set the source logical address entity table index.
	HE_IMETHOD SetSrcLogicalIndex(/*in*/ UInt32 index) = 0;
	/// Get the destination logical address entity table index.
	HE_IMETHOD GetDestLogicalIndex(/*out*/ UInt32* pIndex) = 0;
	/// Set the destination logical address entity table index.
	HE_IMETHOD SetDestLogicalIndex(/*in*/ UInt32 index) = 0;
};

/// \interface IPacketBuffer
/// \ingroup PacketBuffer
/// \brief Basic packet buffer interface.
/// \see coclass MemPacketBuffer, FilePacketBuffer
#define IPacketBuffer_IID \
{ 0x5134605B, 0x5963, 0x45D4, {0x81, 0x9B, 0x6B, 0x16, 0x54, 0xFC, 0x53, 0x05} }

class HE_NO_VTABLE IPacketBuffer : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketBuffer_IID);

	/// Get the buffer media type.
	HE_IMETHOD GetMediaType(/*out*/ TMediaType* pMediaType) = 0;
	/// Set the buffer media type.
	HE_IMETHOD SetMediaType(/*in*/ TMediaType mt) = 0;
	/// Get the buffer media sub type.
	HE_IMETHOD GetMediaSubType(/*out*/ TMediaSubType* pMediaSubType) = 0;
	/// Set the buffer media sub type.
	HE_IMETHOD SetMediaSubType(/*in*/ TMediaSubType mst) = 0;
	/// Get the link speed in bits/second units.
	HE_IMETHOD GetLinkSpeed(/*out*/ UInt64* pnLinkSpeed) = 0;
	/// Set the link speed in bits/second units.
	HE_IMETHOD SetLinkSpeed(/*in*/ UInt64 nLinkSpeed) = 0;
	/// Get the start date-time.
	HE_IMETHOD GetStartDateTime(/*out*/ UInt64* pnStartDateTime) = 0;
	/// Set the start date-time.
	HE_IMETHOD SetStartDateTime(/*in*/ UInt64 nStartDateTime) = 0;
	/// Get the stop date-time.
	HE_IMETHOD GetStopDateTime(/*out*/ UInt64* pnStopDateTime) = 0;
	/// Set the stop date-time.
	HE_IMETHOD SetStopDateTime(/*in*/ UInt64 nStopDateTime) = 0;
	/// Get the duration.
	HE_IMETHOD GetDuration(/*out*/ SInt64* pnDuration) = 0;
	/// Get the first packet number.
	HE_IMETHOD GetFirstPacketNumber(/*out*/ UInt64* pnFirstPacketNumber) = 0;
	/// Get the first packet index in the buffer (support for ring buffers).
	HE_IMETHOD GetFirstPacketIndex(/*out*/ UInt32* pulFirstPacketIndex) = 0;
	/// Get the number of packets in the buffer.
	HE_IMETHOD GetPacketCount(/*out*/ UInt32* pnPacketCount) = 0;
	/// Get the packet buffer size in bytes.
	/// \retval HE_E_OVERFLOW The value is too large to fit in the param.
	/// \remarks Use GetSizeEx.
	HE_IMETHOD GetSize(/*out*/ UInt32* pcbSize) = 0;
	/// Is this a valid packet index?
	HE_IMETHOD IsValidPacketIndex(/*in*/ UInt32 nIndex, /*out*/ BOOL* pbResult) = 0;
	/// Get direct access to a packet's header. This adds a reference to the packet and 
	/// requires ReleasePacket() to be called when no longer using it.
	HE_IMETHOD GetPacketHeader(/*in*/ UInt32 nIndex, /*out*/ const PeekPacket** ppPacketHeader) = 0;
	/// Get direct access to a packet's data. This adds a reference to the packet and 
	/// requires ReleasePacket() to be called when no longer using it.
	HE_IMETHOD GetPacketData(/*in*/ UInt32 nIndex, /*out*/ const UInt8** ppPacketData) = 0;
	/// Get direct access to a packet's meta-data.
	HE_IMETHOD GetPacketMetaData(/*in*/ UInt32 nIndex, /*out*/ const void** ppMetaData) = 0;
	/// Get a packet from the buffer. This adds a reference to the packet and 
	/// requires ReleasePacket() to be called when no longer using it.
	HE_IMETHOD GetPacket(/*in*/ IPacket* pPacket, /*in*/ UInt32 nIndex) = 0;
	/// Add a reference to a packet in the buffer.
	HE_IMETHOD AddRefPacket(/*in*/ UInt32 nIndex) = 0;
	/// Release a packet in the buffer.
	HE_IMETHOD ReleasePacket(/*in*/ UInt32 nIndex) = 0;
	/// Convert a packet buffer index to a packet number.
	HE_IMETHOD PacketIndexToPacketNumber(/*in*/ UInt32 nIndex, /*out*/ UInt64* pnNumber) = 0;
	/// Convert a packet number to a packet buffer index.
	HE_IMETHOD PacketNumberToPacketIndex(/*in*/ UInt64 nPacketNumber, /*out*/ UInt32* pnIndex) = 0;
	/// Reset the packet buffer (throw out all packets).
	HE_IMETHOD Reset() = 0;
	/// Save a selection of packets.
	HE_IMETHOD SaveSelected(/*in*/ Helium::HEBSTR bstrPath, /*in*/ UInt32 ulRangeCount, /*in*/ UInt64* pullRanges) = 0;
	/// Add a packet consumer, used to initialize packet RefCount.
	HE_IMETHOD AddPacketConsumer() = 0;
	/// Remove a packet consumer.
	HE_IMETHOD RemovePacketConsumer() = 0;
	/// Get the packet consumer count.
	HE_IMETHOD GetPacketConsumerCount(/*out*/ UInt32* pnConsumers) = 0;
	/// Get the media info.
	HE_IMETHOD GetMediaInfo(/*out*/ IMediaInfo** ppMediaInfo) = 0;
	/// Set the media info.
	HE_IMETHOD SetMediaInfo(/*in*/ IMediaInfo* pMediaInfo) = 0;
	/// Get the packet buffer size in bytes.
	HE_IMETHOD GetSizeEx(/*out*/ UInt64* pcbSize) = 0;
};

/// \internal
/// \interface IPacketBufferEvents
/// \ingroup PacketBuffer
/// \brief Packet buffer events callback interface.
/// \see IDynamicPacketBuffer, coclass RingPacketBuffer
#define IPacketBufferEvents_IID \
{ 0x22489AC2, 0x1283, 0x4401, {0xB7, 0x53, 0x0E, 0x91, 0x88, 0x2D, 0x92, 0xE7} }

class HE_NO_VTABLE IPacketBufferEvents : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketBufferEvents_IID);

	/// Notification that the buffer now has space available.
	HE_IMETHOD OnBufferSpaceAvailable() = 0;
};

/// \interface IDynamicPacketBuffer
/// \ingroup PacketBuffer
/// \brief A packet buffer whose size is adjustable and can receive packets.
/// \see IPacketBuffer, IPacket, coclass MemPacketBuffer
#define IDynamicPacketBuffer_IID \
{ 0x91C95D24, 0x325B, 0x440E, {0xB0, 0x3F, 0x63, 0xAF, 0x08, 0x80, 0x9A, 0x0F} }

class HE_NO_VTABLE IDynamicPacketBuffer : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IDynamicPacketBuffer_IID);

	/// Set the packet buffer events receiver callback interface.
	HE_IMETHOD SetEventsReceiver(/*in*/ IPacketBufferEvents* pEventsReceiver) = 0;
	/// Get the capacity of the buffer in bytes.
	HE_IMETHOD GetCapacity(/*out*/ UInt32* pcbCapacity) = 0;
	/// Set the capacity of the buffer in bytes.
	HE_IMETHOD SetCapacity(/*in*/ UInt32 cbCapacity) = 0;
	/// Get the capacity of the buffer being used to store packets in bytes.
	HE_IMETHOD GetCapacityUsed(/*out*/ UInt32* pcbCapacity) = 0;
	/// Get the capacity of the the buffer available to store packets in bytes.
	HE_IMETHOD GetCapacityAvailable(/*out*/ UInt32* pcbCapacity) = 0;
	/// Is the buffer full?
	HE_IMETHOD IsFull(/*out*/ BOOL* pbResult) = 0;
	/// Put a packet in the buffer.
	HE_IMETHOD PutPacket(/*in*/ IPacket* pPacket, /*in*/ UInt16 cbDataSize) = 0;
};

/// \interface IDynamicPacketBuffer2
/// \ingroup PacketBuffer
/// \brief A packet buffer whose size is adjustable and can receive packets.
/// \see IPacketBuffer, IPacket, coclass MemPacketBuffer
#define IDynamicPacketBuffer2_IID \
{ 0x17DAC5D7, 0x11AE, 0x461B, {0xBB, 0x1E, 0x09, 0x47, 0x67, 0x0B, 0x2F, 0x91} }

class HE_NO_VTABLE IDynamicPacketBuffer2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IDynamicPacketBuffer2_IID);

	/// Set the packet buffer events receiver callback interface.
	HE_IMETHOD SetEventsReceiver(/*in*/ IPacketBufferEvents* pEventsReceiver) = 0;
	/// Get the capacity of the buffer in bytes.
	HE_IMETHOD GetCapacity(/*out*/ UInt64* pcbCapacity) = 0;
	/// Set the capacity of the buffer in bytes.
	HE_IMETHOD SetCapacity(/*in*/ UInt64 cbCapacity) = 0;
	/// Get the capacity of the buffer being used to store packets in bytes.
	HE_IMETHOD GetCapacityUsed(/*out*/ UInt64* pcbCapacity) = 0;
	/// Get the capacity of the the buffer available to store packets in bytes.
	HE_IMETHOD GetCapacityAvailable(/*out*/ UInt64* pcbCapacity) = 0;
	/// Is the buffer full?
	HE_IMETHOD IsFull(/*out*/ BOOL* pbResult) = 0;
	/// Put a packet in the buffer.
	HE_IMETHOD PutPacket(/*in*/ IPacket* pPacket, /*in*/ UInt16 cbDataSize) = 0;
};

/// \interface IDynamicPacketBuffer3
/// \ingroup PacketBuffer
/// \brief A packet buffer whose size is adjustable and can receive packets.
/// \see IPacketBuffer, IPacket, coclass MemPacketBuffer
#define IDynamicPacketBuffer3_IID \
{ 0x5D5DBFB7, 0x6384, 0x4C19, {0x8D, 0xA0, 0x0E, 0x09, 0x8B, 0xB3, 0x58, 0x24} }

class HE_NO_VTABLE IDynamicPacketBuffer3 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IDynamicPacketBuffer3_IID);

	/// Set the packet buffer events receiver callback interface.
	HE_IMETHOD SetEventsReceiver(/*in*/ IPacketBufferEvents* pEventsReceiver) = 0;
	/// Get the capacity of the buffer in bytes.
	HE_IMETHOD GetCapacity(/*out*/ UInt64* pcbCapacity) = 0;
	/// Set the capacity of the buffer in bytes.
	HE_IMETHOD SetCapacity(/*in*/ UInt64 cbCapacity) = 0;
	/// Get the capacity of the buffer being used to store packets in bytes.
	HE_IMETHOD GetCapacityUsed(/*out*/ UInt64* pcbCapacity) = 0;
	/// Get the capacity of the the buffer available to store packets in bytes.
	HE_IMETHOD GetCapacityAvailable(/*out*/ UInt64* pcbCapacity) = 0;
	/// Is the buffer full?
	HE_IMETHOD IsFull(/*out*/ BOOL* pbResult) = 0;
	/// Put a packet in the buffer.
	HE_IMETHOD PutPacket(/*in*/ IPacket* pPacket, /*in*/ UInt16 cbDataSize,
		/*in*/ const Helium::HeID* pFilterID) = 0;
};

/// \interface IGetPacketFilterID
/// \ingroup PacketBuffer
/// \brief Get the filter ID for a packet in a packet buffer.
/// \see IPacketBuffer, coclass MemPacketBuffer
#define IGetPacketFilterID_IID \
{ 0x5FEC1E33, 0xB0D9, 0x4B02, {0x93, 0xE5, 0xC5, 0x31, 0x6E, 0x83, 0xDE, 0x42} }

class HE_NO_VTABLE IGetPacketFilterID : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGetPacketFilterID_IID);

	/// Get the filter ID for a packet.
	HE_IMETHOD GetPacketFilterID(/*in*/ UInt32 nIndex, 
		/*out*/ Helium::HeID* pFilterID) = 0;
};

/// \internal
/// \enum LimitMode
/// \ingroup PacketBuffer
/// \brief Limit types for searching packets.
/// \see IMultiFilePacketBuffer
enum LimitMode
{
	limitNone,				///< None.
	limitByPackets,			///< K packets.
	limitByBytes,			///< Bytes (packet data).
	limitRollingBytes		///< Static sized packet buffer, roll out packets.
};

/// \internal
/// \interface IMultiFilePacketBuffer
/// \ingroup PacketBuffer
/// \brief Packet buffer that aggregates multiple files.
/// \see coclass MultiFilePacketBuffer
#define IMultiFilePacketBuffer_IID \
{ 0xD8F9BD07, 0x132B, 0x4315, {0xB1, 0xCC, 0x0A, 0x24, 0xFD, 0xE7, 0xF8, 0xE6} }

class HE_NO_VTABLE IMultiFilePacketBuffer : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IMultiFilePacketBuffer_IID);

	HE_IMETHOD SetTimeFilter(/*in*/ UInt64 nStartTime, /*in*/ UInt64 nEndTime) = 0;
	HE_IMETHOD SetFilterMode(/*in*/ PeekFilterMode fm) = 0;
	HE_IMETHOD SetPacketFilter(/*in*/ IFilterPacket* pPacketFilter) = 0;
	HE_IMETHOD SetLimit(/*in*/ LimitMode mode, /*in*/ UInt64 nLimitCount) = 0;
	/// Open.
	/// \param pFiles is a DataTable.
	/// \param bIndex keep packets indexed
	HE_IMETHOD Open(/*in*/ Helium::IHeUnknown* pFiles, /*in*/ BOOL bIndex) = 0;
	/// Load - blocking call.
	HE_IMETHOD Load() = 0;
	/// Stop loading.
	HE_IMETHOD Stop() = 0;
	/// Suspend packet insertion.
	HE_IMETHOD Pause() = 0;
	/// Resume packet insertion.
	HE_IMETHOD Resume() = 0;
};

/// \defgroup PacketHandler Packet handling interfaces and classes implemented by Omni.
/// \brief Packet handling related interfaces and classes.

/// \interface IPacketHandler
/// \ingroup PacketHandler
/// \brief Handles packets from an IPacketProvider
/// \see coclass IPacketProvider
#define IPacketHandler_IID \
{ 0x5BA4760E, 0x9966, 0x499A, {0xBC, 0xB8, 0x86, 0x13, 0x6D, 0xEB, 0x57, 0xF2} }

class HE_NO_VTABLE IPacketHandler : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketHandler_IID);

	/// Handle a packet.
	HE_IMETHOD HandlePacket(/*in*/ IPacket* pPacket) = 0;
};

/// \interface IPacketProvider
/// \ingroup PacketHandler
/// \brief Provides packets to IPacketHandlers. Implemented by adapters.
/// \see interface IPacketHandler, IPacketProviderEvents
#define IPacketProvider_IID \
{ 0x74D7FD7E, 0x46E2, 0x49EB, {0xA7, 0x69, 0x84, 0xDA, 0x22, 0xCF, 0x57, 0x86} }

class HE_NO_VTABLE IPacketProvider : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketProvider_IID);

	/// Add a packet handler.
	HE_IMETHOD AddPacketHandler(/*in*/ IPacketHandler* pPacketHandler) = 0;
	/// Remove a packet handler.
	HE_IMETHOD RemovePacketHandler(/*in*/ IPacketHandler* pPacketHandler) = 0;
};

/// \interface IPacketProviderEvents
/// \ingroup PacketHandler
/// \brief Events broadcast by a packet provider to packet handlers.
/// Implemented by adapters.
/// \see IPacketProvider, IPacketHandler
#define IPacketProviderEvents_IID \
{ 0xBD086F67, 0xE9F8, 0x4CD8, {0xB0, 0x0C, 0x44, 0x88, 0xB9, 0x85, 0xD4, 0xC3} }

class HE_NO_VTABLE IPacketProviderEvents : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketProviderEvents_IID);

	/// Packet provider is started. Called on the provider's thread.
	HE_IMETHOD OnStartPacketProvider() = 0;
	/// Packet provider is stopping. Called on the provider's thread.
	HE_IMETHOD OnStopPacketProvider() = 0;
};

/// \interface IPacketStreamHandler
/// \ingroup PacketHandler
/// \brief Handles packets from an IPacketStreamProvider
#define IPacketStreamHandler_IID \
{ 0x8642E0A6, 0xEEFE, 0x467F, {0x9D, 0x62, 0x4D, 0xB0, 0xCE, 0x6B, 0x23, 0xF3} }

class HE_NO_VTABLE IPacketStreamHandler : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketStreamHandler_IID);

	/// Handle a packet.
	HE_IMETHOD HandleStreamPacket(/*in*/ UInt32 nStream, /*in*/ IPacket* pPacket) = 0;
	/// Handle a time (approximately once per second).
	HE_IMETHOD HandleStreamTime(/*in*/ UInt32 nStream, /*in*/ UInt64 nTimeStamp) = 0;
};

/// \interface IPacketStreamProvider
/// \ingroup PacketHandler
/// \brief Provides packets to IPacketStreamHandlers
#define IPacketStreamProvider_IID \
{ 0x6011599C, 0xABE6, 0x4F0F, {0xBE, 0x58, 0xF5, 0xA4, 0xE3, 0xA3, 0x97, 0x1C} }

class HE_NO_VTABLE IPacketStreamProvider : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketStreamProvider_IID);

	/// Get the count of packet handlers.
	HE_IMETHOD GetStreamCount(SInt32* pCount) = 0;
	/// Add a packet handler.
	HE_IMETHOD AddPacketStreamHandler(/*in*/ IPacketStreamHandler* pHandler) = 0;
	/// Remove a packet handler.
	HE_IMETHOD RemovePacketStreamHandler(/*in*/ IPacketStreamHandler* pHandler) = 0;
};

/// \interface IPacketStreamProviderEvents
/// \ingroup PacketHandler
/// \brief Events broadcast by a packet stream provider to packet handlers.
/// Implemented by adapters.
/// \see IPacketStreamProvider, IPacketStreamHandler
#define IPacketStreamProviderEvents_IID \
{ 0x9F332B07, 0xDA42, 0x4AB2, {0xB0, 0xF8, 0x23, 0x03, 0x95, 0x75, 0x1F, 0xEF} }

class HE_NO_VTABLE IPacketStreamProviderEvents : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketStreamProviderEvents_IID);

	/// Packet stream provider is started. Called on the provider's thread.
	HE_IMETHOD OnStartPacketStreamProvider(/*in*/ UInt32 nStream) = 0;
	/// Packet stream provider is stopping. Called on the provider's thread.
	HE_IMETHOD OnStopPacketStreamProvider(/*in*/ UInt32 nStream) = 0;
};

/// \internal
/// \interface IPacketSegment
/// \ingroup PacketHandler
/// \brief A segment containing a small number of packets
#define IPacketSegment_IID \
{ 0x8577E044, 0x4752, 0x4CD6, {0xBF, 0x08, 0xD5, 0x72, 0xF7, 0x91, 0xE3, 0xFF} }

class HE_NO_VTABLE IPacketSegment : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketSegment_IID)

	HE_IMETHOD GetCount(/*out*/ UInt32* pnCount) = 0;
	HE_IMETHOD GetPacket(/*in*/ UInt32 nIndex, /*out*/ IPacket** ppPacket) = 0;
};

/// \internal
/// \interface IPacketSegmentQueue
/// \ingroup PacketHandler
/// \brief A queue for packet segments
#define IPacketSegmentQueue_IID \
{ 0xCC10D5D4, 0x3311, 0x4CC6, {0xA8, 0x51, 0xDC, 0xD4, 0x79, 0x5E, 0x7B, 0x10} }

class HE_NO_VTABLE IPacketSegmentQueue : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketSegmentQueue_IID)

	HE_IMETHOD Enqueue(/*in*/ IPacketSegment* pPacketSegment) = 0;
	HE_IMETHOD Dequeue(/*out*/ IPacketSegment** ppPacketSegment) = 0;
};

/// \internal
/// \interface IPacketSegmentQueueHolder
/// \ingroup PacketHandler
/// \brief Gets the in and out packet segment queues
#define IPacketSegmentQueueHolder_IID \
{ 0xEF356168, 0xE4AC, 0x4C24, {0x81, 0x1E, 0x8B, 0x9A, 0x5C, 0x92, 0x31, 0xCD} }

class HE_NO_VTABLE IPacketSegmentQueueHolder : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketSegmentQueueHolder_IID)

	HE_IMETHOD GetQueues(/*out*/ IPacketSegmentQueue** ppInQueue, 
		/*out*/ IPacketSegmentQueue** ppOutQueue) = 0;
};

/// \internal
/// \interface IPacketSegmentPipelineStage
/// \ingroup PacketHandler
/// \brief An individual stage in a packet segment pipeline
#define IPacketSegmentPipelineStage_IID \
{ 0xE8AC7D27, 0x0649, 0x4E01, {0x93, 0x7C, 0x30, 0xF4, 0x7D, 0x7D, 0xED, 0x31} }

class HE_NO_VTABLE IPacketSegmentPipelineStage : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketSegmentPipelineStage_IID);

	HE_IMETHOD Activate(
		/*in*/ IPacketSegmentQueue* pInQueue, 
		/*in*/ IPacketSegmentQueue* pOutQueue) = 0;
	HE_IMETHOD Deactivate() = 0;
};

/// \internal
/// \interface IPacketSegmentPipeline
/// \ingroup PacketHandler
/// \brief A packet segment pipeline made up of individual stages
#define IPacketSegmentPipeline_IID \
{ 0xD9382167, 0xCEFD, 0x4190, {0xBF, 0x82, 0xBC, 0x62, 0xD8, 0x52, 0x2E, 0x7C} }

class HE_NO_VTABLE IPacketSegmentPipeline : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketSegmentPipeline_IID);

	HE_IMETHOD GetStageCount(/*out*/ SInt32* pnCount) = 0;
	HE_IMETHOD GetStage(/*in*/ SInt32 index, /*out*/ IPacketSegmentPipelineStage** ppStage) = 0;
	HE_IMETHOD AddStage(/*in*/ IPacketSegmentPipelineStage* pStage) = 0;
	HE_IMETHOD RemoveStage(/*in*/ IPacketSegmentPipelineStage* pStage) = 0;
	HE_IMETHOD RemoveAllStages() = 0;
};

/// \internal
/// \defgroup Trigger Trigger-related interfaces and classes implemented by Omni.
/// \brief Trigger-related interfaces and classes.

/// \internal
/// \interface ITriggerEvent
/// \ingroup Trigger
/// \brief An event that can trigger a trigger
/// \see ITrigger, ITriggerEvents, IPacket
#define ITriggerEvent_IID \
{ 0x025A49CD, 0x7168, 0x4D8C, {0xA7, 0xB8, 0x4D, 0xB1, 0x93, 0x6A, 0xA4, 0x84} }

class HE_NO_VTABLE ITriggerEvent : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ITriggerEvent_IID);

	/// Is the event enabled?
	HE_IMETHOD GetEnabled(/*out*/ BOOL* bEnabled) = 0;
	/// Set the event enabled state.
	HE_IMETHOD SetEnabled(/*in*/ BOOL bEnabled) = 0;
	/// Reset the event.
	HE_IMETHOD Reset(/*in*/ UInt64 ullStartDateTime) = 0;
	/// Check the event (pPacket may be NULL).
	HE_IMETHOD CheckTrigger(/*in*/ IPacket* pPacket, /*out*/ BOOL* pbPacketMatch) = 0;
};

/// \internal
/// \interface ITriggerEvents
/// \ingroup Trigger
/// \brief A collection of trigger events.
/// \see ITrigger, ITriggerEvent
#define ITriggerEvents_IID \
{ 0x8FB287F9, 0x13A9, 0x48ED, {0xB2, 0x88, 0xA3, 0xA1, 0x48, 0x39, 0x18, 0xF7} }

class HE_NO_VTABLE ITriggerEvents : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ITriggerEvents_IID);

	/// Get a trigger event by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ ITriggerEvent** ppTriggerEvent) = 0;
	/// Get the count of trigger events in the list.
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	/// Add a trigger event to the list.
	HE_IMETHOD Add(/*in*/ ITriggerEvent* pTriggerEvent) = 0;
	/// Remove a trigger event from the list.
	HE_IMETHOD Remove(/*in*/ ITriggerEvent* pTriggerEvent) = 0;
	/// Remove all trigger events from the list.
	HE_IMETHOD RemoveAll() = 0;
};

/// \internal
/// \interface ITrigger
/// \ingroup Trigger
/// \brief A trigger
/// \see ITriggerEvent, ITriggerEvents, IPacket
#define ITrigger_IID \
{ 0x96B2B9E3, 0x2BB1, 0x4EC7, {0xBC, 0xC2, 0x17, 0x2A, 0x77, 0x71, 0x60, 0x80} }

class HE_NO_VTABLE ITrigger : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ITrigger_IID);

	/// Get the list of trigger events.
	HE_IMETHOD GetTriggerEvents(/*out*/ ITriggerEvents** ppTriggerEvents) = 0;
	/// Is the trigger enabled?
	HE_IMETHOD GetEnabled(/*out*/ BOOL* pbEnabled) = 0;
	/// Set the enabled state of the trigger.
	HE_IMETHOD SetEnabled(/*in*/ BOOL bEnabled) = 0;
	/// Get the notify option.
	HE_IMETHOD GetNotify(/*out*/ BOOL* pbNotify) = 0;
	/// Set the notify option.
	HE_IMETHOD SetNotify(/*in*/ BOOL bNotify) = 0;
	/// Get the notification severity.
	HE_IMETHOD GetNotifySeverity(/*out*/ PeekSeverity* pSeverity) = 0;
	/// Set the notification severity.
	HE_IMETHOD SetNotifySeverity(/*in*/ PeekSeverity severity) = 0;
	/// Get the toggle capture option.
	HE_IMETHOD GetToggleCapture(/*out*/ BOOL* pbToggleCapture) = 0;
	/// Set the toggle capture option.
	HE_IMETHOD SetToggleCapture(/*in*/ BOOL bToggleCapture) = 0;
	/// Is the trigger active? (Enabled but not Triggered).
	HE_IMETHOD GetActive(/*out*/ BOOL* pbActive) = 0;
	/// Is the trigger triggered?
	HE_IMETHOD GetTriggered(/*out*/ BOOL* pbTriggered) = 0;
	/// Get the time in nanoseconds since midnight 1/1/1601 UTC that the trigger triggered.
	HE_IMETHOD GetTimeTriggered(/*out*/ UInt64* pnTimeTriggered) = 0;
	/// Reset the trigger (resets all trigger events).
	HE_IMETHOD Reset(/*in*/ UInt64 nStartDateTime) = 0;
	/// Check the trigger (checks all enabled trigger events).
	HE_IMETHOD CheckTrigger(/*in*/ IPacket* pPacket, /*out*/ BOOL* pbPacketMatch) = 0;
};

/// \internal
/// \interface IFilterTriggerEvent
/// \ingroup Trigger
/// \brief A trigger event based on filtering packets.
/// \see coclass FilterTriggerEvent
#define IFilterTriggerEvent_IID \
{ 0x1C038EA0, 0x21C3, 0x443A, {0xB5, 0xF8, 0xA5, 0x44, 0xA4, 0xC5, 0xD0, 0xEA} }

class HE_NO_VTABLE IFilterTriggerEvent : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IFilterTriggerEvent_IID);

	/// Get the filter mode.
	HE_IMETHOD GetFilterMode(/*out*/ PeekFilterMode* pMode) = 0;
	/// Set the filter mode.
	HE_IMETHOD SetFilterMode(/*in*/ PeekFilterMode mode) = 0;
	/// Get the packet filter used by the trigger event.
	HE_IMETHOD GetPacketFilter(/*out*/ IFilterPacket** ppPacketFilter) = 0;
	/// Set the packet filter used by the trigger event.
	HE_IMETHOD SetPacketFilter(/*in*/ IFilterPacket* pPacketFilter) = 0;
};

/// \internal
/// \interface ITimeTriggerEvent
/// \ingroup Trigger
/// \brief A trigger event based on time.
/// \see coclass TimeTriggerEvent
#define ITimeTriggerEvent_IID \
{ 0x383A4CDE, 0xDAA5, 0x44E7, {0xB5, 0x78, 0x3E, 0xC5, 0xCF, 0x0D, 0x6F, 0x27} }

class HE_NO_VTABLE ITimeTriggerEvent : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ITimeTriggerEvent_IID);

	/// Get the "use date" option.
	HE_IMETHOD GetUseDate(/*out*/ BOOL* pbUseDate) = 0;
	/// Set the "use date" option.
	HE_IMETHOD SetUseDate(/*in*/ BOOL bUseDate) = 0;
	/// Get the "use elapsed time" option.
	HE_IMETHOD GetUseElapsedTime(/*out*/ BOOL* pbUseElapsedTime) = 0;
	/// Set the "use elapsed time" option.
	HE_IMETHOD SetUseElapsedTime(/*in*/ BOOL bUseElapsedTime) = 0;
	/// Get the reference date-time in nanoseconds since midnight 1/1/1601 UTC.
	HE_IMETHOD GetDateTime(/*out*/ UInt64* pullDateTime) = 0;
	/// Set the reference date-time in nanoseconds since midnight 1/1/1601 UTC.
	HE_IMETHOD SetDateTime(/*in*/ UInt64 nDateTime) = 0;
};

/// \internal
/// \interface IBytesCapturedTriggerEvent
/// \ingroup Trigger
/// \brief A trigger event based on the count of bytes captured.
/// \see coclass BytesCapturedTriggerEvent
#define IBytesCapturedTriggerEvent_IID \
{ 0x9BE54F70, 0x3736, 0x4E8D, {0x99, 0xF0, 0xB9, 0x9D, 0x31, 0xF7, 0xBF, 0xD6} }

class HE_NO_VTABLE IBytesCapturedTriggerEvent : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IBytesCapturedTriggerEvent_IID);

	/// Get the bytes captured count that will trigger the event.
	HE_IMETHOD GetBytesCaptured(/*out*/ UInt64* pnBytesCaptured) = 0;
	/// Set the bytes captured count that will trigger the event.
	HE_IMETHOD SetBytesCaptured(/*in*/ UInt64 nBytesCaptured) = 0;
};

/// \defgroup PacketProcessing Packet processing interfaces and classes implemented by Distributed Analysis Modules.
/// \brief Packet processing-related interfaces and classes.
///
/// These interfaces and classes are provided by Distributed Analysis Modules that need to do packet processing.

/// \interface IProcessPacket
/// \ingroup PacketProcessing
/// \brief Process a packet.
///
/// Optional interface implemented by Distributed Analysis Modules and used by the OmniEngine. This
/// interface is exposed by a Distributed Analysis Module that needs to process packets captured by the engine.
#define IProcessPacket_IID \
{ 0x3CBF88C8, 0xC385, 0x40b5, {0x83, 0x94, 0x0A, 0x84, 0x80, 0xBB, 0x71, 0xC0} }

class HE_NO_VTABLE IProcessPacket : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IProcessPacket_IID);

	/// Allows a Distributed Analysis Module to process a packet captured by the engine.
	/// \param pPacket Pointer to an IPacket interface that allows the
	/// Distributed Analysis Module to get information about the packet.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_OUT_OF_MEMORY Out of memory
	/// \retval HE_E_INVALID_POINTER The pointer is invalid
	/// \remarks This interface should not be implemented if the Distributed Analysis Module
	/// does not need to process packets captured by the engine. This packet processing could include
	/// analyzing the packet or modifying statistics that are used by IUpdateSummaryStats.
	HE_IMETHOD ProcessPacket(/*in*/ IPacket* pPacket) = 0;
};

/// \interface IProcessTime
/// \ingroup PacketProcessing
/// \brief Do processing at regular time intervals.
///
/// Optional interface implemented by Distributed Analysis Modules and used by the OmniEngine. This
/// interface is exposed by a Distributed Analysis Module that needs to do processing at regular time intervals.
#define IProcessTime_IID \
{ 0x750138D1, 0xC07A, 0x4C06, {0x83, 0xF9, 0xB2, 0x75, 0xBB, 0xEA, 0xDC, 0xE8} }

class HE_NO_VTABLE IProcessTime : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IProcessTime_IID);

	/// Do some processing based on an elapsed time.
	/// \param nCurrentTime The current time in nanoseconds since midnight 1/1/1601 UTC.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks If implemented, this method will be called approximately once per second.
	HE_IMETHOD ProcessTime(/*in*/ UInt64 nCurrentTime) = 0;
};

/// \interface IResetProcessing
/// \ingroup PacketProcessing
/// \brief Reset processing, for example the clearing statistical counters.
///
/// Optional interface implemented by Distributed Analysis Modules and used by the OmniEngine.
/// This interface is exposed by a Distributed Analysis Module that needs the ability to perform
/// reset processing when a capture is started.
#define IResetProcessing_IID \
{ 0x0FA72D19, 0x31B9, 0x44F7, {0xAA, 0x38, 0x00, 0x41, 0x02, 0x42, 0xA9, 0x92} }

class HE_NO_VTABLE IResetProcessing : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IResetProcessing_IID);

	/// Reset processing.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks The type of processing performed during a reset could include the zeroing of all counters.
	HE_IMETHOD ResetProcessing() = 0;
};

/// \internal
/// \interface IPacketProcessor
/// \ingroup PacketProcessing
/// \brief Manages a list of IProcessPacket and processes packets from a packet buffer.
/// \see IProcessPacket, IPacketProcessorEvents, IPacketBuffer, IPacket
#define IPacketProcessor_IID \
{ 0x1998080E, 0x2294, 0x4C7E, {0xB5, 0x35, 0x06, 0xE8, 0xFB, 0x6F, 0xF1, 0x38} }

class HE_NO_VTABLE IPacketProcessor : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketProcessor_IID);

	/// Get a processor by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 index, /*out*/ Helium::IHeUnknown** ppUnk) = 0;
	/// Get the count of items.
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	/// Add a processor.
	HE_IMETHOD Add(/*in*/ Helium::IHeUnknown* pUnk) = 0;
	/// Remove a processor.
	HE_IMETHOD Remove(/*in*/ Helium::IHeUnknown* pUnk) = 0;
	/// Remove all processors.
	HE_IMETHOD RemoveAll() = 0;
	/// Get the packet buffer.
	HE_IMETHOD GetPacketBuffer(/*out*/ Helium::IHeUnknown** ppPacketBuffer) = 0;
	/// Set the packet buffer.
	HE_IMETHOD SetPacketBuffer(/*in*/ Helium::IHeUnknown* pPacketBuffer) = 0;
	/// Get the count of packets processed.
	HE_IMETHOD GetPacketsProcessed(/*out*/ UInt64* pnPacketsProcessed) = 0;
	/// Is it processing packets?
	HE_IMETHOD IsProcessing(/*out*/ BOOL* pbProcessing) = 0;
	/// Start processing packets in the packet buffer.
	HE_IMETHOD Start(/*in*/ UInt64 nStartTime) = 0;
	/// Stop processing packets in the packet buffer.
	HE_IMETHOD Stop(/*in*/ UInt64 nStopTime) = 0;
	/// Stop processing immediately.
	HE_IMETHOD Abort() = 0;
	/// Reset the packet processor.
	HE_IMETHOD Reset() = 0;
	/// Wait until processing is finished (i.e., packets processed == packet
	/// count in the buffer).
	/// \param nTimeOut Timeout in milliseconds. Zero for infinite.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_TIMEOUT Timeout.
	HE_IMETHOD WaitUntilDone(/*in*/ UInt32 nTimeOut) = 0;
	/// All-in-one utility: set buffer, start, wait, stop.
	HE_IMETHOD Process(/*in*/ Helium::IHeUnknown* pPacketBuffer) = 0;
	/// Is this a real-time packet processor?
	HE_IMETHOD SetRealTime(/*in*/ BOOL bRealTime) = 0;
};

/// \internal
/// \interface IPacketSegmentProcessor
/// \ingroup PacketProcessing
/// \brief Manages a list of IProcessPacket and processes packets from a packet segment.
/// \see IProcessPacket, IPacketProcessorEvents, IPacketSegment, IPacket
#define IPacketSegmentProcessor_IID \
{ 0x6AACE66F, 0x9217, 0x4CE7, {0xB8, 0x9B, 0x5D, 0x2B, 0xE6, 0x74, 0xB3, 0x81} }

class HE_NO_VTABLE IPacketSegmentProcessor : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketSegmentProcessor_IID);

	/// Get a processor by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 index, /*out*/ Helium::IHeUnknown** ppUnk) = 0;
	/// Get the count of items.
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	/// Add a processor.
	HE_IMETHOD Add(/*in*/ Helium::IHeUnknown* pUnk) = 0;
	/// Remove a processor.
	HE_IMETHOD Remove(/*in*/ Helium::IHeUnknown* pUnk) = 0;
	/// Remove all processors.
	HE_IMETHOD RemoveAll() = 0;
	/// Get the packet buffer.
	HE_IMETHOD GetPacketBuffer(/*out*/ Helium::IHeUnknown** ppPacketBuffer) = 0;
	/// Set the packet buffer.
	HE_IMETHOD SetPacketBuffer(/*in*/ Helium::IHeUnknown* pPacketBuffer) = 0;
	/// Get the count of packets processed.
	HE_IMETHOD GetPacketsProcessed(/*out*/ UInt64* pnPacketsProcessed) = 0;
	/// Is it processing packets?
	HE_IMETHOD IsProcessing(/*out*/ BOOL* pbProcessing) = 0;
	/// Start processing packets in the packet buffer.
	HE_IMETHOD Start(/*in*/ UInt64 nStartTime) = 0;
	/// Stop processing packets in the packet buffer.
	HE_IMETHOD Stop(/*in*/ UInt64 nStopTime) = 0;
	/// Reset the packet processor.
	HE_IMETHOD Reset() = 0;
	/// Set the packet hash range.
	HE_IMETHOD SetHashRange(/*in*/ UInt32 nHashLower, /*in*/ UInt32 nHashUpper) = 0;
	/// Get the packet hash range.
	HE_IMETHOD GetHashRange(/*out*/ UInt32* pnHashLower, /*out*/ UInt32* pnHashUpper) = 0;
};

/// \interface IPacketProcessorEvents
/// \ingroup PacketProcessing
/// \brief Events broadcast by a packet processor to contained items.
/// \see IPacketProcessor, IPacketSegmentProcessor
#define IPacketProcessorEvents_IID \
{ 0xAFB024B7, 0x4C4F, 0x4153, {0x99, 0x95, 0xD4, 0x8B, 0x04, 0x88, 0xBC, 0x6E} }

class HE_NO_VTABLE IPacketProcessorEvents : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketProcessorEvents_IID);

	/// Packet processing is starting.
	HE_IMETHOD OnStartPacketProcessing(/*in*/ UInt64 nStartTime) = 0;
	/// Packet processing is stopping.
	HE_IMETHOD OnStopPacketProcessing(/*in*/ UInt64 nStopTime) = 0;
	/// Packet processing has been reset.
	HE_IMETHOD OnResetPacketProcessing() = 0;
};

/// \internal
/// \defgroup MessageLog Message log interfaces and classes implemented by Omni.
/// \brief Message log-related interfaces and classes.

/// \internal
/// \interface IMessageLog
/// \ingroup MessageLog
/// \brief Properties interface for the message log.
/// \see coclass MessageLog
#define IMessageLog_IID \
{ 0x962400F9, 0xCCFD, 0x4DF7, {0xA7, 0xCB, 0x52, 0x8A, 0xB5, 0xB8, 0x9E, 0x96} }

class HE_NO_VTABLE IMessageLog : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IMessageLog_IID);

	/// Get the max record count.
	HE_IMETHOD GetMaxRecordCount(/*out*/ SInt32* pnMaxRecordCount) = 0;
	/// Set the max record count.
	HE_IMETHOD SetMaxRecordCount(/*in*/ SInt32 nMaxRecordCount) = 0;
	/// Get the max record count adjustment.
	HE_IMETHOD GetMaxRecordCountAdjustment(/*out*/ SInt32* pnMaxRecordCountAdjust) = 0;
	/// Set the max record count adjustment (i.e., delete this many records
	/// when the max record count is reached).
	HE_IMETHOD SetMaxRecordCountAdjustment(/*in*/ SInt32 nMaxRecordCountAdjust) = 0;
	/// Get the log file path.
	HE_IMETHOD GetLogFilePath(/*out*/ Helium::HEBSTR* pbstrPath) = 0;
	/// Set the log file path.
	HE_IMETHOD SetLogFilePath(/*in*/ Helium::HEBSTR bstrPath) = 0;
};

/// \internal
/// \interface IQueryLog
/// \ingroup MessageLog
/// \brief Query the message log with an SQL statement, receive XML.
/// \see coclass MessageLog
#define IQueryLog_IID \
{ 0x989877C7, 0x1F17, 0x4AD4, {0xB7, 0xE3, 0x07, 0xF8, 0x0C, 0xF0, 0x58, 0xE6} }

class HE_NO_VTABLE IQueryLog : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IQueryLog_IID);

	/// Query the message log with an SQL statement, receive XML.
	/// First page: positive page size and zero message id.
	/// Previous page: negative page size from the message id.
	/// Next page: positive page size from the message id.
	/// Last page: negative page size and zero message id.
	HE_IMETHOD QueryLog(/*in*/ SInt32 nPageSize, /*in*/ SInt32 nMessageID,
		/*in*/ Helium::HEBSTR bstrSQL, /*in*/ Helium::IHeUnknown* pXmlDoc, /*in*/ Helium::IHeUnknown* pXmlNode) = 0;
	/// Query the message log for a record count using an SQL "where" clause.
	/// The "where" class may be NULL to get the total count.
	HE_IMETHOD QueryCount(/*in*/ Helium::HEBSTR bstrWhere, /*out*/ SInt32* pnCount) = 0;
};

/// \internal
/// \interface IQueryLog2
/// \ingroup MessageLog
/// \brief Query the message log, receive XML.
/// \see coclass MessageLog
#define IQueryLog2_IID \
{ 0x393E52F3, 0x4AF6, 0x43F3, {0x96, 0x8F, 0x1B, 0x74, 0xD9, 0xBE, 0x28, 0x00} }

class HE_NO_VTABLE IQueryLog2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IQueryLog2_IID);

	/// Query the message log, receive XML.
	/// \param pQueryParams A property bag of query parameters.
	/// \param pXmlDoc XML document to receive results.
	/// \param pXmlNode XML node to append results.
	HE_IMETHOD QueryLog2(/*in*/ Helium::IHeUnknown* pQueryParams, 
		/*in*/ Helium::IHeUnknown* pXmlDoc, /*in*/ Helium::IHeUnknown* pXmlNode) = 0;
};

/// \internal
/// \interface IClearLog
/// \ingroup MessageLog
/// \brief Remove messages from the log.
/// \see coclass MessageLog
#define IClearLog_IID \
{ 0x76A1CA9C, 0x19F6, 0x4AC2, {0xA3, 0x14, 0x71, 0x47, 0x18, 0x88, 0xA2, 0x74} }

class HE_NO_VTABLE IClearLog : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IClearLog_IID);

	/// Remove messages from the log.
	HE_IMETHOD ClearLog(/*in*/ Helium::HEBSTR bstrSQL, /*in*/ BOOL bCompact) = 0;
};

/// \internal
/// \defgroup PeekSecurity Security interfaces and classes implemented by Omni.
/// \brief Security-related interfaces and classes.

/// \internal
/// \interface IPeekSecurity
/// \ingroup PeekSecurity
/// \brief Properties interface for the security information.
/// \see coclass PeekSecurity
#define IPeekSecurity_IID \
{ 0xD4B26888, 0xA622, 0x4FA4, {0xB4, 0xAD, 0xB6, 0x83, 0x1D, 0xC3, 0xD6, 0xFD} }

class HE_NO_VTABLE IPeekSecurity : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekSecurity_IID);

	enum PeekSecurityType
	{
		peekSecurityAlert,			///< Security alerts.
		peekSecurityEvent,			///< Security events.
		peekSecurityInclude,		///< Security includes.
		peekSecurityExclude,		///< Security excludes.
		peekSecuritySourceConfig,	///< Security source configuration.
		peekSecurityConfiguration	///< Security configuration.
	};

	/// Get the security file path.
	HE_IMETHOD GetSecurityFilePath(/*out*/ Helium::HEBSTR* pbstrPath) = 0;
	/// Set the security file path.
	HE_IMETHOD SetSecurityFilePath(/*in*/ Helium::HEBSTR bstrPath) = 0;

	/// Get the max security record count.
	HE_IMETHOD GetMaxSecurityRecordCount(/*in*/ PeekSecurityType type, /*out*/ SInt64* pnMaxRecordCount) = 0;
	/// Set the max security record count.
	HE_IMETHOD SetMaxSecurityRecordCount(/*in*/ PeekSecurityType type, /*in*/ SInt64 nMaxRecordCount) = 0;
	/// Get the max security record count adjustment.
	HE_IMETHOD GetMaxSecurityRecordCountAdjustment(/*in*/ PeekSecurityType type, /*out*/ SInt64* pnMaxRecordCountAdjust) = 0;
	/// Set the max security record count adjustment (i.e., delete this many records when the max security record count is reached).
	HE_IMETHOD SetMaxSecurityRecordCountAdjustment(/*in*/ PeekSecurityType type, /*in*/ SInt64 nMaxRecordCountAdjust) = 0;

	/// Remove security information.
	HE_IMETHOD ClearSecurity(/*in*/ PeekSecurityType type, /*in*/ Helium::HEBSTR bstrSQL, /*in*/ BOOL bCompact) = 0;

	// Inserts a security alert into the security database.
	HE_IMETHOD LogSecurityAlert(/*in*/ UInt64 nTimestamp, /*in*/ const wchar_t* pszAlert) = 0;
	// Inserts a security event into the security database.
	HE_IMETHOD LogSecurityEvent(/*in*/ UInt64 nTimestamp, /*in*/ UInt64 nReceivedTimestamp, /*in*/ UInt64 nIDSID, /*in*/ const wchar_t* pszIDSName, /*in*/ const wchar_t* pszCVEID,
		/*in*/ UInt64 nGlobalID, /*in*/ UInt64 nSignatureID, /*in*/ const wchar_t* pszSignature, /*in*/ const wchar_t* pszCategory, 
		/*in*/ UInt32 nSeverity, /*in*/ bool bIPv6, /*in*/ UInt32 nSourceAddress1, /*in*/ UInt32 nSourceAddress2, /*in*/ UInt64 nSourceAddress3,
		/*in*/ UInt16 nSourcePort, /*in*/ UInt32 nDestinationAddress1, /*in*/ UInt32 nDestinationAddress2, /*in*/ UInt64 nDestinationAddress3,
		/*in*/ UInt16 nDestinationPort, /*in*/ const wchar_t* pszProtocol, /*in*/ UInt64 nBytes, /*in*/ UInt64 nPackets) = 0;
	// Inserts a security include entry into the security database.
	HE_IMETHOD LogSecurityInclude(/*in*/ bool bIPv6, /*in*/ UInt32 nAddress1, /*in*/ UInt32 nAddress2, /*in*/ UInt64 nAddress3) = 0;

	/// Query the security information, receive XML.
	/// \param pRspXmlDoc XML document containing request parameters.
	/// \param pRspXmlNode XML node containing request parameters.
	/// \param pRspXmlDoc XML document to receive results.
	/// \param pRspXmlNode XML node to append results.
	HE_IMETHOD QuerySecurity(/*in*/ Helium::IHeUnknown* pReqXmlDoc, /*in*/ Helium::IHeUnknown* pReqXmlNode,
		/*in*/ Helium::IHeUnknown* pRspXmlDoc, /*in*/ Helium::IHeUnknown* pRspXmlNode) = 0;

	/// Query the security information for a record count using an SQL "where" clause.
	/// The "where" class may be NULL to get the total count.
	HE_IMETHOD QuerySecurityCount(/*in*/ PeekSecurityType type, /*in*/ Helium::HEBSTR bstrWhere, /*out*/ SInt64* pnCount) = 0;

	/// Query/Save the security configuration information (in vigil 2.0, these are usually IDS's)
	/// by type configuration info in JSON format
	HE_IMETHOD QuerySecurityConfiguration(/*in*/ IPeekSecurity::PeekSecurityType type,  /*out*/ Helium::HEBSTR* pbstrJsonConfig) = 0;
	HE_IMETHOD SaveSecurityConfiguration(/*in*/ IPeekSecurity::PeekSecurityType type, /*in*/ Helium::HEBSTR bstrJsonConfig) = 0;
	HE_IMETHOD DisableAlertSource(/*in*/ UInt64 alert_source_id) = 0;

	/// Query for updated security configuration 
	/// return changed configuration info in JSON format; empty if unchanged
	HE_IMETHOD QuerySecurityUpdates(/*out*/ Helium::HEBSTR* pbstrJsonConfig, /*out*/ Helium::HEBSTR* pbstrUserAddedConfig) = 0;
};

/// \internal
/// \interface ICaptureSecurityEvents
/// \ingroup SecurityEvents
/// \brief Capture the security events.
#define ICaptureSecurityEvents_IID \
{ 0x7BC1A7E8, 0x692D, 0x4A5F, {0x87, 0x54, 0x9D, 0x72, 0xF5, 0xB8, 0xA7, 0x9C} }

class HE_NO_VTABLE ICaptureSecurityEvents : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICaptureSecurityEvents_IID);

	// Get the time offset used for capturing packets from security events.
	HE_IMETHOD GetTimeOffset(/*out*/ UInt32* pnTimeOffset) = 0;
};

/// \defgroup ProtoSpecs ProtoSpec interfaces and classes implemented by Omni.
/// \brief ProtoSpec-related interfaces and classes.
///
/// These interfaces and classes are provided by Omni to provide access to ProtoSpecs.

/// \def GET_PSID
/// \ingroup ProtoSpecs
/// \brief Extract the "ID" portion of a ProtoSpec, e.g., 1000 is IP.
#ifndef GET_PSID
#define GET_PSID(inst)			((UInt16)((inst) & 0xffff))
#endif

/// \def GET_PSINSTANCE
/// \ingroup ProtoSpecs
/// \brief Extract the "instance" portion of a ProtoSpec.
#ifndef GET_PSINSTANCE
#define GET_PSINSTANCE(inst)	((UInt16)(((inst) & 0xffff0000) >> 16))
#endif

/// \enum ProtospecsReserved
/// \ingroup ProtoSpecs
/// \brief Reserved PSIDs that consumer need to use
enum ProtospecsReserved
{
	kProtospecsReserved_Switch = 0xffff			///< A switch node.
};

/// \interface IProtoSpecs
/// \ingroup ProtoSpecs
/// \brief ProtoSpec access routines.
///
/// This interface is implemented by the OmniPeek console and OmniEngine and 
/// provides access to ProtoSpecs. IProtoSpecs2 is preferred.
#define IProtoSpecs_IID \
{ 0xB91DAA66, 0x1EF8, 0x4F3D, {0xAF, 0xD5, 0xBB, 0xCC, 0x95, 0xB0, 0x14, 0xFC} }

class HE_NO_VTABLE IProtoSpecs : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IProtoSpecs_IID);

	/// Load ProtoSpecs.
	/// \param pszXmlFile Pointer to the name of the ProtoSpec XML file.
	/// This parameter may be NULL to force the loading of the ProtoSpec binary file.
	/// \param pszBinFile Pointer to the name of the ProtoSpec binary file.
	/// This parameter may be NULL to force the loading of the ProtoSpec XML file.
	/// \param ulProductCode Do not load the ProtoSpecs related to this product. If 0,
	/// load the ProtoSpecs for all products.
	/// \param bShowHidden Reserved, must be true.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks If the ProtoSpec XML file exists and if the ProtoSpec binary file exists, and
	/// if both names are passed to this method, the method loads the "newer" file. The binary
	/// file is generated if it does not exist when this method is called.
	HE_IMETHOD Load(/*in*/ const wchar_t* pszXmlFile, /*in*/ const wchar_t* pszBinFile,
		/*in*/ UInt32 ulProductCode, /*in*/ BOOL bShowHidden) = 0;
	/// Unload ProtoSpecs.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD Unload() = 0;
	/// Convert a ProtoSpec instance ID to a ProtoSpec ID.
	/// \param ulInstanceID The ProtoSpec instance ID.
	/// \param pusID Pointer to a variable that receives the ProtoSpec ID.
	/// \retval HE_S_OK Success
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD InstanceToID(/*in*/ UInt32 ulInstanceID, /*out*/ UInt16* pusID) = 0;
	/// Is this ProtoSpec a "root" ID?
	/// \param ulID The ProtoSpec instance ID.
	/// \retval HE_S_OK This is a "root" ID.
	/// \retval HE_S_FALSE This is not a "root" ID.
	/// \remarks None
	HE_IMETHOD IsRootID(/*in*/ UInt32 ulID) = 0;
	/// Is this ProtoSpec a LLC ID?
	/// \param ulID The ProtoSpec instance ID.
	/// \retval HE_S_OK This is a LLC ID.
	/// \retval HE_S_FALSE This is not a LLC ID.
	/// \remarks None
	HE_IMETHOD IsLlcID(/*in*/ UInt32 ulID) = 0;
	/// Get the parent instance ID for a ProtoSpec.
	/// \param ulID The ProtoSpec instance ID.
	/// \param pulParent Pointer to a variable that receives the ProtoSpec instance ID of the parent.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks Unsupported with "native" ProtoSpecs enabled.
	HE_IMETHOD GetParent(/*in*/ UInt32 ulID, /*out*/ UInt32* pulParent) = 0;
	/// Get the hierarchical name of the protocol corresponding to the ProtoSpec.
	/// \param ulID The ProtoSpec instance ID.
	/// \param pbstrName Pointer to a Helium::HEBSTR that holds the hierarchical name of the ProtoSpec.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetHierarchicalName(/*in*/ UInt32 ulID, /*out*/ Helium::HEBSTR* pbstrName) = 0;
	/// Get the short name of the protocol corresponding to a ProtoSpec.
	/// \param ulID The ProtoSpec instance ID.
	/// \param pbstrName Pointer to a Helium::HEBSTR that holds the short name of the ProtoSpec.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetShortName(/*in*/ UInt32 ulID, /*out*/ Helium::HEBSTR* pbstrName) = 0;
	/// Get the long name of the protocol corresponding to a ProtoSpec.
	/// \param ulID The ProtoSpec instance ID.
	/// \param pbstrName Pointer to a Helium::HEBSTR that holds the long name of the ProtoSpec.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetLongName(/*in*/ UInt32 ulID, /*out*/ Helium::HEBSTR* pbstrName) = 0;
	/// Get the description of a ProtoSpec.
	/// \param ulID The ProtoSpec instance ID.
	/// \param pbstrDescription Pointer to a Helium::HEBSTR that holds the description of the ProtoSpec.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetDescription(/*in*/ UInt32 ulID, /*out*/ Helium::HEBSTR* pbstrDescription) = 0;
	/// Get the color assigned to the protocol corresponding to a ProtoSpec instance ID (see OleTranslateColor).
	/// \param ulID The ProtoSpec instance ID.
	/// \param pColor Pointer to a variable that receives the color assigned to the ProtoSpec.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetColor(/*in*/ UInt32 ulID, /*out*/ UInt32* pColor) = 0;
	/// Get the depth of the instance ID in the ProtoSpec.
	/// \param ulID The ProtoSpec instance ID.
	/// \param pulDepth Pointer to a variable that receives the depth of the ProtoSpec.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks Unsupported with "native" ProtoSpecs enabled.
	HE_IMETHOD GetDepth(/*in*/ UInt32 ulID, /*out*/ UInt32* pulDepth) = 0;
	/// Calculate the ProtoSpec instance ID for the highest layer of a packet.
	/// \param mt The media type.
	/// \param mst The media sub type.
	/// \param pData Pointer to the packet.
	/// \param cbDataBytes The number of bytes in the packet.
	/// \param pulID Pointer to a variable that receives the ProtoSpec instance ID.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks None
	HE_IMETHOD GetPacketProtoSpec(/*in*/ TMediaType mt, /*in*/ TMediaSubType mst,
		/*in*/ const UInt8* pData, /*in*/ UInt32 cbDataBytes, /*out*/ UInt32* pulID) = 0;
	/// Check if a protocol is a subprotocol of a number of possible parent protocols.
	/// \param ulID The ProtoSpec instance ID of the protocol.
	/// \param pParentArray An array of ProtoSpec instance IDs for the parent protocols.
	/// \param lCount The number of parent protocols.
	/// \param pulMatchInstanceID Pointer to a variable that receives the ProtoSpec instance ID if
	/// the protocol is a subprotocol of one of the parent protocols.
	/// This parameter may be NULL if this information is not needed.
	/// \param plMatchIndex Pointer to a variable the receives the index of the parent protocol if
	/// the protocol is a subprotocol of one of the parent protocols.
	/// This parameter may be NULL if this information is not needed.
	/// \retval HE_S_OK The protocol is a subprotocol of one of the specified parents.
	/// \retval HE_S_FALSE The protocol is not a subprotocol of any of the specified parents.
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_ARG The lCount parameter is invalid.
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks Unsupported with "native" ProtoSpecs enabled.
	HE_IMETHOD IsDescendentOf(/*in*/ UInt32 ulID, /*in*/ const UInt32 pParentArray[],
		/*in*/ SInt32 lCount, /*out*/ UInt32* pulMatchInstanceID, /*out*/ SInt32* plMatchIndex) = 0;
	/// Find a ProtoSpec ID by hierarchical name.
	/// \param bstrName A Helium::HEBSTR containing the name of the ProtoSpec.
	/// \param pulID Pointer to a variable that receives the ProtoSpec ID.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD Find(/*in*/ Helium::HEBSTR bstrName, /*out*/ UInt32* pulID) = 0;
	/// Get the header layer for a protocol in a packet.
	/// \param mt The media type.
	/// \param mst The media sub type.
	/// \param pData Pointer to the packet.
	/// \param cbDataBytes The number of bytes in the packet.
	/// \param ulPSpecInstID The ProtoSpec instance ID for the protocol.
	/// \param pulSourceID Pointer to a variable that receives the ProtoSpec instance ID
	/// corresponding to the header layer.
	/// \param ppData Pointer to a variable that receives a pointer to the location of the header layer
	/// in the packet.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks Unsupported with "native" ProtoSpecs enabled.
	HE_IMETHOD GetHeaderLayer(/*in*/ TMediaType mt, /*in*/ TMediaSubType mst,
		/*in*/ const UInt8* pData, /*in*/ UInt32 cbDataBytes,
		/*in*/ UInt32 ulPSpecInstID, /*out*/ UInt32* pulSourceID, /*out*/ UInt8** ppData) = 0;
	/// Get the data layer for a protocol in a packet.
	/// \param mt The media type.
	/// \param mst The media sub type.
	/// \param pData Pointer to the packet.
	/// \param cbDataBytes The number of bytes in the packet.
	/// \param ulID The ProtoSpec instance ID for the protocol.
	/// \param pulSourceID Pointer to a variable that receives the ProtoSpec instance ID
	/// corresponding to the data layer.
	/// \param ppData Pointer to a variable that receives a pointer to the location of the data layer
	/// in the packet.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks Unsupported with "native" ProtoSpecs enabled.
	HE_IMETHOD GetDataLayer(/*in*/ TMediaType mt, /*in*/ TMediaSubType mst,
		/*in*/ const UInt8* pData, /*in*/ UInt32 cbDataBytes, /*in*/ UInt32 ulID,
		/*out*/ UInt32* pulSourceID, /*out*/ UInt8** ppData) = 0;
	/// Build an array of parents for a ProtoSpec instance ID.
	/// \param ulID ProtoSpec instance ID.
	/// \param pParentArray Pointer to an array of parent instance IDs.
	/// \param pulMaxCount On input, pointer to a variable that specifies how many parents can
	/// be returned. On output, the variable receives the actual number of parent instance IDs returned.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_OUT_OF_MEMORY The value for max count is not large enough to hold the number of parents.
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks Unsupported with "native" ProtoSpecs enabled.
	HE_IMETHOD BuildParentArray(/*in*/ UInt32 ulID,
		/*in,out*/ UInt32* pParentArray, /*in,out*/ UInt32* pulMaxCount) = 0;
	/// Find a specific ProtoSpec Instance ID based on a parent ID array
	/// \param pParentArray Pointer to an array of parent instance IDs.
	/// \param nLength Number of items in pParentArray
	/// \param pulID If successful, contains the found Instance ID
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks Unsupported with "native" ProtoSpecs enabled.
	HE_IMETHOD FindProtospecInstanceByID(/*in*/ UInt16* pParentArray, 
		/*in*/ int nLength, /*out*/ UInt32* pulID) = 0;
	/// Translate a ProtoSpec ID to handle the case where an ID has be moved
	/// or removed.
	/// \param nFromID ProtoSpec ID to translate.
	/// \param pToID ProtoSpec ID result. Equal to nFromID if the ProtoSpec
	/// exists with no translation.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks None
	HE_IMETHOD TranslateID(/*in*/ UInt16 nFromID, /*out*/ UInt16* pToID) = 0;
	/// Return a reverse path to the root item in the array, with all header and data pointers.
	/// \param mt The media type.
	/// \param mst The media sub type.
	/// \param pData Pointer to the packet.
	/// \param cbDataBytes The number of bytes in the packet.
	/// \param ulID The ProtoSpec instance ID.
	/// \param payInfo Pointer to array of infos to be filled in (SProtoSpecInfo*).
	/// \param nInfoCount In: number of infos pointed to by payInfo
	///                   Out: number of infos returned.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks Unsupported with "native" ProtoSpecs enabled.
	HE_IMETHOD BuildParentInfo(/*in*/ TMediaType mt, /*in*/ TMediaSubType mst,
		/*in*/ const UInt8* pData, /*in*/ UInt32 cbDataBytes, /*in*/ UInt32 ulID,
		/*in/out*/ PacketLayerInfo* payLayers, /*in/out*/ UInt32& nLayerCount) = 0;
};

/// \interface IProtoSpecs2
/// \ingroup ProtoSpecs
/// \brief ProtoSpec access routines.
///
/// This interface is implemented by the OmniPeek console and OmniEngine and 
/// provides access to ProtoSpecs.
#define IProtoSpecs2_IID \
{ 0x1B2DC649, 0xF985, 0x486F, {0xAF, 0xAB, 0x5C, 0x53, 0xEF, 0xED, 0x27, 0xFE} }

class HE_NO_VTABLE IProtoSpecs2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IProtoSpecs2_IID);

	/// Set the ProtoSpecs mode: XML, native, or automatic.
	/// \param mode: 0=XML, 1=native, 2=automatic.
	/// \retval HE_S_OK Success
	/// \retval HE_E_INVALID_ARG Unsupported mode
	/// \remarks Call before loading ProtoSpecs to enable XML or native mode, or
	/// to determine the mode automatically by comparing the XML file with a
	/// hash file.
	HE_IMETHOD SetProtoSpecsMode(/*in*/ SInt32 mode) = 0;
	/// Load ProtoSpecs.
	/// \param pszXmlFile Pointer to the name of the ProtoSpec XML file.
	/// This parameter may be NULL to force the loading of the ProtoSpec binary file.
	/// \param pszBinFile Pointer to the name of the ProtoSpec binary file.
	/// This parameter may be NULL to force the loading of the ProtoSpec XML file.
	/// \param ulProductCode Do not load the ProtoSpecs related to this product. If 0,
	/// load the ProtoSpecs for all products.
	/// \param bShowHidden Reserved, must be true.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks If the ProtoSpec XML file exists and if the ProtoSpec binary file exists, and
	/// if both names are passed to this method, the method loads the "newer" file. The binary
	/// file is generated if it does not exist when this method is called.
	HE_IMETHOD Load(/*in*/ const wchar_t* pszXmlFile, /*in*/ const wchar_t* pszBinFile,
		/*in*/ UInt32 ulProductCode, /*in*/ BOOL bShowHidden) = 0;
	/// Unload ProtoSpecs.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD Unload() = 0;
	/// Get the ProtoSpecs file version.
	/// \param pbstrVersion Pointer to a Helium::HEBSTR that receives the version string.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetVersion(/*out*/ Helium::HEBSTR* pbstrVersion) = 0;
	/// Determine whether "native" ProtoSpecs is enabled.
	/// \param pbEnabled BOOL.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetNativeProtoSpecsEnabled(/*out*/ BOOL* pbEnabled) = 0;
	/// Convert a ProtoSpec instance ID to a ProtoSpec ID.
	/// \param ulInstanceID The ProtoSpec instance ID.
	/// \param pusID Pointer to a variable that receives the ProtoSpec ID.
	/// \retval HE_S_OK Success
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD InstanceToID(/*in*/ UInt32 ulInstanceID, /*out*/ UInt16* pusID) = 0;
	/// Is this ProtoSpec a "root" ID?
	/// \param ulID The ProtoSpec instance ID.
	/// \retval HE_S_OK This is a "root" ID.
	/// \retval HE_S_FALSE This is not a "root" ID.
	/// \remarks None
	HE_IMETHOD IsRootID(/*in*/ UInt32 ulID) = 0;
	/// Is this ProtoSpec a LLC ID?
	/// \param ulID The ProtoSpec instance ID.
	/// \retval HE_S_OK This is a LLC ID.
	/// \retval HE_S_FALSE This is not a LLC ID.
	/// \remarks None
	HE_IMETHOD IsLlcID(/*in*/ UInt32 ulID) = 0;
	/// Translate a ProtoSpec ID to handle the case where an ID has be moved
	/// or removed.
	/// \param nFromID ProtoSpec ID to translate.
	/// \param pToID ProtoSpec ID result. Equal to nFromID if the ProtoSpec
	/// exists with no translation.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks None
	HE_IMETHOD TranslateID(/*in*/ UInt16 nFromID, /*out*/ UInt16* pToID) = 0;
	/// Find a ProtoSpec ID by hierarchical name.
	/// \param bstrName A Helium::HEBSTR containing the name of the ProtoSpec.
	/// \param pulID Pointer to a variable that receives the ProtoSpec ID.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD Find(/*in*/ Helium::HEBSTR bstrName, /*out*/ UInt32* pulID) = 0;
	/// Get the hierarchical name of the protocol corresponding to the ProtoSpec.
	/// \param ulID The ProtoSpec instance ID.
	/// \param pbstrName Pointer to a Helium::HEBSTR that holds the hierarchical name of the ProtoSpec.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetHierarchicalName(/*in*/ UInt32 ulID, /*out*/ Helium::HEBSTR* pbstrName) = 0;
	/// Get the short name of the protocol corresponding to a ProtoSpec.
	/// \param ulID The ProtoSpec instance ID.
	/// \param pbstrName Pointer to a Helium::HEBSTR that holds the short name of the ProtoSpec.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetShortName(/*in*/ UInt32 ulID, /*out*/ Helium::HEBSTR* pbstrName) = 0;
	/// Get the long name of the protocol corresponding to a ProtoSpec.
	/// \param ulID The ProtoSpec instance ID.
	/// \param pbstrName Pointer to a Helium::HEBSTR that holds the long name of the ProtoSpec.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetLongName(/*in*/ UInt32 ulID, /*out*/ Helium::HEBSTR* pbstrName) = 0;
	/// Get the description of a ProtoSpec.
	/// \param ulID The ProtoSpec instance ID.
	/// \param pbstrDescription Pointer to a Helium::HEBSTR that holds the description of the ProtoSpec.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetDescription(/*in*/ UInt32 ulID, /*out*/ Helium::HEBSTR* pbstrDescription) = 0;
	/// Get the color assigned to the protocol corresponding to a ProtoSpec instance ID (see OleTranslateColor).
	/// \param ulID The ProtoSpec instance ID.
	/// \param pColor Pointer to a variable that receives the color assigned to the ProtoSpec.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetColor(/*in*/ UInt32 ulID, /*out*/ UInt32* pColor) = 0;
	/// Calculate the ProtoSpec ID and protocol layers for a packet.
	/// \param mt The media type.
	/// \param mst The media sub type.
	/// \param pData Pointer to the packet.
	/// \param cbDataBytes The number of bytes in the packet.
	/// \param pulID Pointer to a variable that receives the ProtoSpec ID.
	/// \param payLayers Pointer to array of layer infos to be filled in.
	/// \param pnLayers In: number of layer infos pointed to by payLayers
	///                 Out: number of layer infos returned.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks None
	HE_IMETHOD GetPacketProtoSpec(/*in*/ TMediaType mt, /*in*/ TMediaSubType mst,
		/*in*/ const UInt8* pData, /*in*/ UInt32 cbDataBytes, /*out*/ UInt32* pulID,
		/*out*/ PacketLayerInfo* payLayers, /*in,out*/ UInt32* pnLayers) = 0;
};

/// \defgroup Plugins Management interfaces and classes implemented by Distributed Analysis Modules.
/// \brief Management interfaces and classes for Distributed Analysis Modules.
///
/// These interfaces and classes are implemented by Distributed Analysis Modules
/// so that Omni can manage the Distributed Analysis Modules.

/// Plugins implement this category.
#define Category_PeekPlugin \
{ 0xD8A51BAC, 0x0297, 0x4247, {0xBF, 0x9A, 0x9C, 0x95, 0x62, 0x62, 0xAB, 0xD5} }

/// \interface IPeekPlugin
/// \ingroup Plugins
/// \brief Exposes information about the Distributed Analysis Module.
///
/// Required interface implemented by Distributed Analysis Modules and used by the OmniPeek console
/// and OmniEngine. This interface exposes information about the Distributed Analysis Module.
#define IPeekPlugin_IID \
{ 0xA0D478D3, 0x062A, 0x482E, {0x85, 0x2E, 0x4C, 0xD6, 0xAE, 0x8E, 0x90, 0x34} }

class HE_NO_VTABLE IPeekPlugin : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekPlugin_IID);

	/// Get the name of the Distributed Analysis Module.
	/// \param pbstrName Pointer to the Distributed Analysis Module's name.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_OUT_OF_MEMORY Out of memory
	/// \retval HE_E_INVALID_POINTER The pointer is invalid
	/// \remarks None
	HE_IMETHOD GetName(/*out*/ Helium::HEBSTR* pbstrName) = 0;
	/// Get the class identifier of the Distributed Analysis Module.
	/// \param pClassID Pointer to the Distributed Analysis Module's class identifier.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_OUT_OF_MEMORY Out of memory
	/// \retval HE_E_INVALID_POINTER The pointer is invalid
	/// \remarks None
	HE_IMETHOD GetClassID(/*out*/ Helium::HeCID* pClassID) = 0;
};

/// \interface IQueryMediaSupport
/// \ingroup Plugins
/// \brief Determine whether a media type is supported by a Distributed Analysis Module.
///
/// Optional interface implemented by Distributed Analysis Modules and used by
/// the OmniPeek console and OmniEngine. This interface is used to determine
/// whether a media type is supported by a Distributed Analysis Module.
#define IQueryMediaSupport_IID \
{ 0xCAB18993, 0xD18E, 0x4327, {0x81, 0xA2, 0x90, 0x91, 0xCE, 0xC1, 0xDA, 0xBB} }

class HE_NO_VTABLE IQueryMediaSupport : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IQueryMediaSupport_IID);

	/// Check if the media type is supported.
	/// \param mt The media type.
	/// \param mst The media sub type.
	/// \param pbSupported true if the media type is supported.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid
	/// \remarks None
	HE_IMETHOD IsMediaSupported(/*in*/ TMediaType mt, /*in*/ TMediaSubType mst, /*out*/ BOOL* pbSupported) = 0;
};

/// \interface IPeekPluginLoad
/// \ingroup Plugins
/// \brief Load/unload a Distributed Analysis Module.
///
/// Optional interface implemented by Distributed Analysis Modules and used by the OmniPeek console and OmniEngine.
/// This interface is exposed by a Distributed Analysis Module that needs to perform initialization after being
/// instantiated or to perform cleanup prior to being released.
#define IPeekPluginLoad_IID \
{ 0xAF1BB3E3, 0x6B51, 0x41FF, {0x82, 0xFB, 0x55, 0x00, 0xE1, 0x9E, 0x41, 0xFC} }

class HE_NO_VTABLE IPeekPluginLoad : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekPluginLoad_IID);

	/// Load the Distributed Analysis Module.
	/// \param guidContext The globally unique identifier assigned to the capture using this
	/// Distributed Analysis Module.
	/// The console and engine may use different identifiers to represent the same capture.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_OUT_OF_MEMORY Out of memory
	/// \remarks This method is called after the object has been instantiated.
	HE_IMETHOD Load(/*in*/ const Helium::HeID& guidContext) = 0;
	/// Unload the Distributed Analysis Module.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks This method is called prior to the object being released.
	HE_IMETHOD Unload() = 0;
};

/// \interface IPeekPluginAbout
/// \ingroup Plugins
/// \brief Show information about a Distributed Analysis Module.
///
/// Optional interface implemented by Distributed Analysis Modules and used by the OmniPeek console.
/// This interface is exposed by a Distributed Analysis Module in order to show information about
/// the module. This information usually consists of the module's name, copyright and description.
#define IPeekPluginAbout_IID \
{ 0x304C82AF, 0x2C41, 0x4C94, {0xB0, 0xFC, 0xE9, 0xE0, 0xBC, 0x41, 0x5E, 0xD3} }

class HE_NO_VTABLE IPeekPluginAbout : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekPluginAbout_IID);

	/// Show information about a Distributed Analysis Module.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks This interface should not be implemented if the Distributed Analysis Module
	/// does not have any information to show.
	HE_IMETHOD ShowAbout() = 0;
};

/// \interface IPeekPluginOptions
/// \ingroup Plugins
/// \brief Manage Distributed Analysis Module options.
///
/// Optional interface implemented by Distributed Analysis Modules and used by the OmniPeek console.
/// This interface is exposed by a Distributed Analysis Module in order to show information
/// about, get and set the module's options/preferences and is only a required interface when the
/// module supports options.
#define IPeekPluginOptions_IID \
{ 0xA10542CB, 0xBB84, 0x496F, {0xAF, 0xFE, 0x5E, 0x23, 0x72, 0x4D, 0x26, 0xB9} }

class HE_NO_VTABLE IPeekPluginOptions : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekPluginOptions_IID);

	/// Show option information about a Distributed Analysis Module.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_OUT_OF_MEMORY Out of memory
	/// \remarks This interface should not be implemented if the Distributed Analysis Module
	/// does not support options.
	HE_IMETHOD ShowOptions() = 0;
};

/// \interface IPeekPluginOptions2
/// \ingroup Plugins
/// \brief Manage Distributed Analysis Module options.
///
/// Optional interface implemented by Distributed Analysis Modules and used by the OmniPeek console.
/// This interface is exposed by a Distributed Analysis Module in order to show information
/// about, get and set the module's options/preferences and is only a required interface when the
/// module supports options.
#define IPeekPluginOptions2_IID \
{ 0x685F391C, 0x4B30, 0x4CD3, {0xB1, 0x24, 0x77, 0x6D, 0x5, 0x41, 0xFE, 0xBE} }

class HE_NO_VTABLE IPeekPluginOptions2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekPluginOptions2_IID);

	HE_IMETHOD GetDefaultOptions(/*out*/ Helium::IHeUnknown** ppOptions) = 0;
	HE_IMETHOD GetOptions(/*out*/ Helium::IHeUnknown** ppOptions) = 0;
	HE_IMETHOD SetOptions(/*in*/ Helium::IHeUnknown* pOptions) = 0;
};

/// \enum PeekPluinUIHandlerTypes
/// \ingroup Plugins
/// \brief Plugin UI Handler types: About, Options or Capture Tab.
enum PeekPluginUIHandlerTypes
{
	peekPluginHandlerUI_About      = 0x01,	///< About Info.
	peekPluginHandlerUI_Options    = 0x02,	///< Options Info.
	peekPluginHandlerUI_CaptureTab = 0x04,	///< Capture Tab Info.
	peekPluginHandlerUI_EngineTab  = 0x08	///< Engine Tab Info.
};

/// \enum PeekPluinUIHandlerFlags
/// \ingroup Plugins
/// \brief Plugin UI Handler Flags: Raw or zipped.
enum PeekPluginUIHandlerFlags
{
	peekPluginHandlerUI_Raw            = 0x01,	///< The data is raw text.
	peekPluginHandlerUI_Zipped         = 0x02,	///< The data is zipped.
	peekPluginHandlerUI_Dashboard      = 0x10,	///< The Capture tab listed under the Dashboards.
	peekPluginHandlerUI_NoDefaultPrefs = 0x20,	///< Ignore Preferences on new Captures.
};

/// \interface IPluginUISource
/// \ingroup Plugins
/// \brief Distributed Analysis Module that has an UI handler.
///
/// Optional interface implemented by Distributed Analysis Modules and used by the OmniEngine.
/// This interface is exposed by a Distributed Analysis Module to identify the UI Handler Id and
/// to provide additional information.
#define IPluginUISource_IID \
{ 0x47617279, 0xACDC, 0x4494, {0xB3, 0x4B, 0x12, 0x7F, 0x63, 0x71, 0x1F, 0xA4} }

class HE_NO_VTABLE IPluginUISource : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPluginUISource_IID);

	HE_IMETHOD GetHandlerID(/*out*/ Helium::HeID* pHandlerID) = 0;
	HE_IMETHOD SetHandlerID(/*in*/ const Helium::HeID* pHandlerID) = 0;
	HE_IMETHOD GetData(/*in*/ int nType, /*out*/ UInt32* pFlags, /*out*/ Helium::IHeStream* pData) = 0;
};

/// \interface IPluginUIHandler
/// \ingroup Plugins
/// \brief A Distributed Analysis Module UI handler.
///
/// Interface for displaying Analysis Module's UI: the About and Options dialogs.
#define IPluginUIHandler_IID \
{ 0x47617279, 0x9000, 0x4BC7, {0xB5, 0xAB, 0xC8, 0x77, 0x31, 0x05, 0x3C, 0x2A} }

class HE_NO_VTABLE IPluginUIHandler : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPluginUIHandler_IID);

	HE_IMETHOD SetConsoleSend(/*in*/ IConsoleSend* inConsoleSend) = 0;
	HE_IMETHOD Duplicate(/*out*/ Helium::IHeUnknown** ppCopy) = 0;
	HE_IMETHOD GetStatus(/*out*/ UInt32* nStatus) = 0;
	HE_IMETHOD SetData(/*in*/ int nType, /*in*/ UInt32 nFlags, /*in*/ Helium::IHeStream* pData) = 0;
	HE_IMETHOD ShowAbout() = 0;
	HE_IMETHOD ShowOptions() = 0;
	HE_IMETHOD SetPluginInfo(/*in*/ const Helium::HeCID& cid, /*in*/ Helium::HEBSTR bstrName) = 0;
	HE_IMETHOD GetPluginInfo(/*out*/ Helium::HeCID* pcid, /*out*/ Helium::HEBSTR* pbstrName) = 0;
};

/// \interface IProcessPluginMessage
/// \ingroup Plugins
/// \brief Process plugin messages.
///
/// Optional interface implemented by Distributed Analysis Modules and used by the OmniEngine. This
/// interface is exposed by a Distributed Analysis Module that needs to do process plugin messages.
#define IProcessPluginMessage_IID \
{ 0xF9E20218, 0x526B, 0x4926, {0x86, 0xEF, 0xB2, 0xC3, 0x1C, 0x44, 0x99, 0xB3} }

class HE_NO_VTABLE IProcessPluginMessage : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IProcessPluginMessage_IID);

	/// Do some processing of a request sent by a client.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	HE_IMETHOD ProcessPluginMessage(/*in*/ const Helium::HeCID& pluginID,
		/*in*/ Helium::IHeStream* pInStream, /*in*/ Helium::IHeStream* pOutStream) = 0;
};

/// \internal
/// \interface IPluginsConfig
/// \ingroup Capture
/// \brief Plugins settings in a IXMLDOMDocument.
/// \see PluginsConfig
#define IPluginsConfig_IID \
{ 0x92B19A3A, 0xD604, 0x4A14, {0xB0, 0xE5, 0xAE, 0x91, 0x21, 0xA2, 0x94, 0x03} }

class HE_NO_VTABLE IPluginsConfig : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPluginsConfig_IID);

	/// Add a config (Xml Document).
	HE_IMETHOD Add(/*in*/ Helium::IHeUnknown* pConfig) = 0;
	/// Get the count of plugin IDs.
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	/// Get a config (Xml Document) by Class ID.
	HE_IMETHOD GetItem(/*in*/ const Helium::HeCID& cid, /*out*/ Helium::IHeUnknown** ppConfig) = 0;
	/// Get a config (Xml Document) by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 nIndex, /*out*/ Helium::IHeUnknown** ppConfig) = 0;
	/// Remove a config.
	HE_IMETHOD Remove(/*in*/ const Helium::HeCID& cid) = 0;
	/// Remove all the configs.
	HE_IMETHOD RemoveAll() = 0;
};

/// \internal
/// \interface IPluginCollection
/// \ingroup Plugins
/// \brief A collection of Distributed Analysis Modules.
#define IPluginCollection_IID \
{ 0x1C97A0B8, 0x2A56, 0x41E9, {0x80, 0xF7, 0x76, 0x21, 0xBB, 0xA1, 0x52, 0xBE} }

class HE_NO_VTABLE IPluginCollection : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPluginCollection_IID);

	#define PEEK_E_UNSUPPORTED_MEDIA			((HeResult)	0x80040200L)

	/// Get a Distributed Analysis Module by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ IPeekPlugin** ppPlugin) = 0;
	/// Get the count of Distributed Analysis Modules in the list.
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	/// Get a Distributed Analysis Module by ID.
	HE_IMETHOD ItemFromCID(/*in*/ const Helium::HeCID& cid, /*out*/ IPeekPlugin** ppPlugin) = 0;
	/// Load all registered Distributed Analysis Modules.
	HE_IMETHOD LoadAllPlugins(/*in*/ const Helium::HeID& guidContext) = 0;
	/// Load registered Distributed Analysis Modules that support the given media.
	HE_IMETHOD LoadPluginsSupportingMedia(/*in*/ const Helium::HeID& guidContext,
		/*in*/ TMediaType mt, /*in*/ TMediaSubType mst) = 0;
	/// Load the specified Distributed Analysis Module.
	HE_IMETHOD LoadPlugin(/*in*/ const Helium::HeCID& cid, /*in*/ const Helium::HeID& guidContext) = 0;
	/// Unload all Distributed Analysis Modules.
	HE_IMETHOD UnloadAllPlugins() = 0;
	/// Unload the specified Distributed Analysis Module.
	HE_IMETHOD UnloadPlugin(/*in*/ const Helium::HeCID& cid) = 0;
	/// Load the plug-in settings.
	HE_IMETHOD LoadPluginsConfig(/*in*/ IPluginsConfig* pPluginsConfig) = 0;
	/// Load the specified Distributed Analysis Module if it supports the given media.
	HE_IMETHOD LoadPluginSupportingMedia(/*in*/ const Helium::HeCID& cid, 
		/*in*/ const Helium::HeID& guidContext,
		/*in*/ TMediaType mt, /*in*/ TMediaSubType mst) = 0;
};

/// \interface IPluginAdapter
/// \ingroup Plugins
/// \brief Distributed Analysis Module manages peekPluginAdapters.
///
/// Optional interface implemented by Distributed Analysis Modules. This interface
/// is used to determine if the plugin supports peekPluginAdapter type.
#define IPluginAdapter_IID \
{ 0x3D1A1FB8, 0x289A, 0x48F7, {0x86, 0x98, 0x14, 0xA9, 0x0D, 0x0F, 0x66, 0x01} }

class HE_NO_VTABLE IPluginAdapter : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPluginAdapter_IID);

	/// Delete an adapter.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_ACCESS_DENIED Insufficient user capabilities or adapter is in use.
	/// \remarks If the plugin added this adapter to the Adapter Manager, then this
	/// plugin must remove it from the Adapter Manager.
	HE_IMETHOD DeleteAdapter(/*in*/ const Helium::HeID& adapterID) = 0;
};

/// \enum PeekSummaryLayer
/// \ingroup PacketProcessing
/// \brief Layers for summary information.
///
/// This enumeration defines the various packet layers.
/// \see ISummarizePacket
enum PeekSummaryLayer
{
	peekSummaryLayerNone,			///< No layer.
	peekSummaryLayerPhysical,		///< Physical layer (Layer 1).
	peekSummaryLayerLink,			///< Link layer (Layer 2).
	peekSummaryLayerNetwork,		///< Network layer (Layer 3).
	peekSummaryLayerTransport,		///< Transport layer (Layer 4).
	peekSummaryLayerSession,		///< Session layer (Layer 5).
	peekSummaryLayerPresentation,	///< Presentation layer (Layer 6).
	peekSummaryLayerApplication		///< Application layer (Layer 7).
};

/// \interface ISummarizePacket
/// \ingroup PacketProcessing
/// \brief Summarize a packet for display in a packet list.
///
/// Optional interface implemented by Distributed Analysis Modules and used by 
/// the OmniPeek console. This interface is exposed by a Distributed Analysis 
/// Module that needs to display information in the Summary column of 
/// OmniPeek's packet list.
#define ISummarizePacket_IID \
{ 0xD731D0D7, 0xAD4C, 0x4C85, {0xA9, 0x4D, 0x1B, 0x9D, 0xB1, 0x0F, 0xAB, 0x49} }

class HE_NO_VTABLE ISummarizePacket : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISummarizePacket_IID);

	/// Allow a Distributed Analysis Module to get a text message displayed in the Summary column of a packet list.
	/// \param pPacket Pointer to an IPacket interface that allows the
	/// Distributed Analysis Module to get information about the packet.
	/// \param pbstrSummary Pointer to the text message to be displayed
	/// in the Summary column of the packet list.
	/// \param pColor Pointer to UInt32 which allows the analysis
	/// module to specify the color to be used for the text message.
	/// \param pLayer Pointer to PeekSummaryLayer which allows the
	/// Distributed Analysis Module to specify which layer, if any, of the ISO OSI 7-layer
	/// model corresponds to the text message.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_OUT_OF_MEMORY Out of memory
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid
	/// \remarks This interface should not be implemented if the Distributed Analysis Module
	/// does not need to display summary information in the packet list.
	/// When multiple Distributed Analysis Modules want to display a message for the
	/// same packet, the PeekSummaryLayer information is used to determine which
	/// message is displayed. In this case, the console displays the information corresponding
	/// to the highest layer.
	HE_IMETHOD SummarizePacket(/*in*/ IPacket* pPacket, /*out*/ Helium::HEBSTR* pbstrSummary,
		/*in,out*/ UInt32* pColor, /*in,out*/ PeekSummaryLayer* pLayer) = 0;
};

/// \interface IUpdateSummaryStats
/// \ingroup PacketProcessing
/// \brief Update summary statistics.
///
/// Optional interface implemented by Distributed Analysis Modules and used by the OmniEngine.
/// This interface is exposed by a Distributed Analysis Module that needs to update one or more items
/// in the summary statistics display.
#define IUpdateSummaryStats_IID \
{ 0xE7895BB6, 0x0AC3, 0x4679, {0xB5, 0x53, 0x2A, 0x37, 0x9C, 0x23, 0x36, 0xA1} }

class HE_NO_VTABLE IUpdateSummaryStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IUpdateSummaryStats_IID);

	/// Update summary statistics.
	/// \param pSnapshot Pointer to the snapshot.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid
	/// \remarks This interface should not be implemented if the Distributed Analysis Module
	/// does not update any summary statistics. The module should query for the
	/// ISummaryStatsSnapshot interface using the pointer provided as a parameter
	/// to the interface. Use the ISummaryStatsSnapshot interface to add the
	/// items that are to be shown in the summary statistics display.
	/// \see IProcessPacket
	HE_IMETHOD UpdateSummaryStats(/*in*/ Helium::IHeUnknown* pSnapshot) = 0;
};

/// \defgroup MediaUtils Network media utility interfaces and classes implemented by Omni.
/// \brief Network media utility interfaces and classes.
///
/// These interfaces and classes are implemented by Omni
/// so that Distributed Analysis Modules can access information about network media.

/// \interface IMediaUtils
/// \ingroup MediaUtils
/// \brief Network media utilities.
///
/// Provides access to network media information.
#define IMediaUtils_IID \
{ 0xE3305E02, 0x60C7, 0x4B5F, {0x9C, 0x43, 0x70, 0x74, 0x33, 0xAB, 0x61, 0x81} }

class HE_NO_VTABLE IMediaUtils : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IMediaUtils_IID);

	/// Get the name for a media type.
	/// \param mt The media type.
	/// \param mst The media sub type.
	/// \param pbstr Pointer to a Helium::HEBSTR that holds the name.
	/// \retval HE_S_OK Success
	/// \retval HE_E_INVALID_ARG The non-pointer parameter is invalid.
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetMediaString(/*in*/ TMediaType mt, /*in*/ TMediaSubType mst, /*out*/ Helium::HEBSTR* pbstr) = 0;
	/// Get the name for a media type.
	/// \param mt The media type.
	/// \param pbstr Pointer to a BSTR that holds the name.
	/// \retval HE_S_OK Success
	/// \retval HE_E_INVALID_ARG The non-pointer parameter is invalid.
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetMediaTypeString(/*in*/ TMediaType mt, /*out*/ Helium::HEBSTR* pbstr) = 0;
	/// Get the name for a media sub type.
	/// \param mst The media sub type.
	/// \param pbstr Pointer to a Helium::HEBSTR that holds the name.
	/// \retval HE_S_OK Success
	/// \retval HE_E_INVALID_ARG The non-pointer parameter is invalid.
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetMediaSubTypeString(/*in*/ TMediaSubType mst, /*out*/ Helium::HEBSTR* pbstr) = 0;
	/// Get the name for a media spec class.
	/// \param msc The media spec class.
	/// \param pbstr Pointer to a Helium::HEBSTR that holds the name.
	/// \retval HE_S_OK Success
	/// \retval HE_E_INVALID_ARG The non-pointer parameter is invalid.
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetMediaSpecClassString(/*in*/ TMediaSpecClass msc, /*out*/ Helium::HEBSTR* pbstr) = 0;
	/// Get the name for a media spec type.
	/// \param mst The media spec type.
	/// \param pbstr Pointer to a Helium::HEBSTR that holds the name.
	/// \retval HE_S_OK Success
	/// \retval HE_E_INVALID_ARG The non-pointer parameter is invalid.
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetMediaSpecTypeString(/*in*/ TMediaSpecType mst, /*out*/ Helium::HEBSTR* pbstr) = 0;
	/// Get the prefix for a media spec type.
	/// \param mst The media spec type.
	/// \param pbstr Pointer to a Helium::HEBSTR that holds the prefix.
	/// \retval HE_S_OK Success
	/// \retval HE_E_INVALID_ARG The non-pointer parameter is invalid.
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetMediaSpecTypePrefix(/*in*/ TMediaSpecType mst, /*out*/ Helium::HEBSTR* pbstr) = 0;
	/// Get a default formatted string for a media spec type.
	/// \param mst The media spec type.
	/// \param pbstr Pointer to a Helium::HEBSTR that holds the default.
	/// \retval HE_S_OK Success
	/// \retval HE_E_INVALID_ARG The non-pointer parameter is invalid.
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetMediaSpecTypeDefault(/*in*/ TMediaSpecType mst, /*out*/ Helium::HEBSTR* pbstr) = 0;
	/// Get a string for a media spec.
	/// \param pSpec Pointer to the media spec.
	/// \param pbstr Pointer to a Helium::HEBSTR that holds the string.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD SpecToString(/*in*/ const TMediaSpec* pSpec, /*out*/ Helium::HEBSTR* pbstr) = 0;
	/// Get a media spec from a string.
	/// \param bstrSpec The string to be converted into a media spec.
	/// \param mst The media spec type.
	/// \param pSpec Pointer to a variable that receives the media spec.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD StringToSpec(/*in*/ Helium::HEBSTR bstrSpec, /*in*/ TMediaSpecType mst, /*out*/ TMediaSpec* pSpec) = 0;
	/// Determine whether a media spec is a broadcast or multicast address.
	/// \param pSpec Pointer to the media spec.
	/// \param pbBroadcast Pointer to a boolean that will indicate whether the address is a broadcast.
	/// \param pbMulticast Pointer to a boolean that will indicate whether the address is a multicast.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER One of the pointers is invalid.
	/// \remarks None
	HE_IMETHOD IsSpecBroadcastOrMulticast(/*in*/ const TMediaSpec* pSpec,
		/*out*/ BOOL* pbBroadcast, /*out*/ BOOL* pbMulticast) = 0;
};

/// \internal
/// \defgroup Capture Capture interfaces and classes implemented by Omni.
/// \brief Capture-related interfaces and classes.

/// \internal
/// \enum PeekCaptureState
/// \ingroup Capture
/// \brief Capture state: idle or capturing.
/// \see PeekCaptureStatus
enum PeekCaptureState
{
	peekCaptureStateIdle      = 0x0000,		///< Idle.
	peekCaptureStateCapturing = 0x0001		///< Capturing packets.
};

/// \internal
/// \enum PeekTriggerState
/// \ingroup Capture
/// \brief Trigger state: start active or stop active.
/// \see PeekCaptureStatus
enum PeekTriggerState
{
	peekTriggerStateStartActive = 0x0100,	///< Start trigger is active.
	peekTriggerStateStopActive  = 0x02000	///< Stop trigger is active. TODO: oops
};

/// \internal
/// \enum PeekCaptureStatus
/// \ingroup Capture
/// \brief Combinations of PeekCaptureState and PeekTriggerState
/// \see PeekCaptureState, PeekTriggerState, IPeekCapture
enum PeekCaptureStatus
{
	/// Idle.
	peekCaptureStatusIdle = peekCaptureStateIdle,
	/// Idle - waiting to start trigger.
	peekCaptureStatusIdleStartActive = peekCaptureStateIdle | peekTriggerStateStartActive,
	/// Idle - with a stop trigger active.
	peekCaptureStatusIdleStopActive = peekCaptureStateIdle | peekTriggerStateStopActive,
	/// Idle - waiting to start trigger with a stop trigger active.
	peekCaptureStatusIdleStartActiveStopActive = peekCaptureStateIdle | peekTriggerStateStartActive | peekTriggerStateStopActive,
	/// Capturing - waiting on a start trigger.
	peekCaptureStatusWaitStart = peekCaptureStateCapturing | peekTriggerStateStartActive,
	/// Capturing - waiting on a start trigger with a stop trigger active.
	peekCaptureStatusWaitStartStopActive = peekCaptureStateCapturing | peekTriggerStateStartActive | peekTriggerStateStopActive,
	/// Capturing packets.
	peekCaptureStatusCapturing = peekCaptureStateCapturing,
	/// Capturing packets with a stop trigger active.
	peekCaptureStatusCapturingStopActive = peekCaptureStateCapturing | peekTriggerStateStopActive
};

/// \internal
/// \interface IFilterConfig
/// \ingroup Capture
/// \brief Filter settings in a capture template.
/// \see FilterConfig
#define IFilterConfig_IID \
{ 0x89651E8C, 0x8B06, 0x4F07, {0xBE, 0x62, 0xB0, 0x09, 0x97, 0x79, 0x47, 0x03} }

class HE_NO_VTABLE IFilterConfig : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IFilterConfig_IID);

	/// Get a filter ID by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ Helium::HeID* pID) = 0;
	/// Get the count of filter IDs.
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	/// Add a filter ID.
	HE_IMETHOD Add(/*in*/ const Helium::HeID& id) = 0;
	/// Remove a filter ID.
	HE_IMETHOD Remove(/*in*/ const Helium::HeID& id) = 0;
	/// Get the filter mode.
	HE_IMETHOD GetFilterMode(/*out*/ PeekFilterMode* pMode) = 0;
	/// Set the filter mode.
	HE_IMETHOD SetFilterMode(/*in*/ PeekFilterMode mode) = 0;
};

/// \internal
/// \interface IAlarmConfig
/// \ingroup Capture
/// \brief Alarm settings in a capture template.
/// \see AlarmConfig
#define IAlarmConfig_IID \
{ 0x47314F41, 0x8E9E, 0x4D99, {0xAB, 0x39, 0x50, 0x4F, 0x02, 0x4D, 0x46, 0xA9} }

class HE_NO_VTABLE IAlarmConfig : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IAlarmConfig_IID);

	/// Get an Alarm ID by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ Helium::HeID* pID) = 0;
	/// Get the count of Alarm IDs.
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	/// Add an Alarm ID.
	HE_IMETHOD Add(/*in*/ const Helium::HeID& id) = 0;
	/// Remove an Alarm ID.
	HE_IMETHOD Remove(/*in*/ const Helium::HeID& id) = 0;
	/// Remove all.
	HE_IMETHOD RemoveAll() = 0;
};

/// \internal
/// \interface IPeekCapturePerfItem
/// \ingroup Capture
/// \brief Performance configuration item for a capture template.
/// \see PeekCapturePerfItem
#define IPeekCapturePerfItem_IID \
{ 0x06A39618, 0x9B1E, 0x4B27, {0x90, 0x2A, 0x71, 0x1A, 0xCA, 0x8B, 0xD2, 0x1B} }

class HE_NO_VTABLE IPeekCapturePerfItem : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekCapturePerfItem_IID);

	/// Get the performance item name.
	HE_IMETHOD GetName(/*out*/ Helium::HEBSTR* pbstrName) = 0;
	/// Set the performance item name.
	HE_IMETHOD SetName(/*in*/ Helium::HEBSTR bstrName) = 0;
	/// Get the performance item cost.
	HE_IMETHOD GetCost(/*out*/ SInt32* pnCost) = 0;
	/// Set the performance item cost.
	HE_IMETHOD SetCost(/*in*/ SInt32 cost) = 0;
	/// Get the stat limit support.
	HE_IMETHOD GetStatLimit(/*out*/ BOOL* pbStatLimit) = 0;
	/// Set the stat limit support.
	HE_IMETHOD SetStatLimit(/*in*/ BOOL bStatLimit) = 0;
};

/// \internal
/// \interface IPeekCapturePerf
/// \ingroup Capture
/// \brief Performance configuration for a capture template.
/// \see PeekCapturePerf
#define IPeekCapturePerf_IID \
{ 0x2F10C024, 0xB970, 0x41A1, {0x84, 0xD0, 0xFD, 0xEE, 0x6C, 0x91, 0x14, 0x91} }

class HE_NO_VTABLE IPeekCapturePerf : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekCapturePerf_IID);

	/// Get a performance item by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ IPeekCapturePerfItem** ppItem) = 0;
	/// Get the count of items in the list.
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	/// Add a performance item.
	HE_IMETHOD Add(/*in*/ IPeekCapturePerfItem* pItem) = 0;
};

/// \internal
/// \interface IPeekCaptureInitialize
/// \ingroup Capture
/// \brief Initialization interface for IPeekCapture.
/// \see IPeekCapture, coclass PeekCapture
#define IPeekCaptureInitialize_IID \
{ 0x1FF97F0B, 0x9FB2, 0x4CAD, {0x8A, 0x03, 0x42, 0x6F, 0x23, 0xFA, 0x10, 0xD6} }

class HE_NO_VTABLE IPeekCaptureInitialize : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekCaptureInitialize_IID);

	/// Initialize a capture (settings contains a property bag).
	HE_IMETHOD Initialize(/*in*/ Helium::HEVARIANT vtCaptureSettings) = 0;
	/// Initialize all of this capture's services.
	HE_IMETHOD InitializeServices() = 0;
	/// Get the capture template.
	HE_IMETHOD GetCaptureTemplate(/*out*/ Helium::IHePropertyBag** ppCaptureTemplate) = 0;
	/// Set the capture template.
	HE_IMETHOD SetCaptureTemplate(/*in*/ Helium::IHePropertyBag* pCaptureTemplate) = 0;
};

/// \internal
/// \interface IPeekCapture
/// \ingroup Capture
/// \brief Basic capture interface.
/// \see IPeekCaptureInitialize, IPeekCaptures, coclass PeekCapture
#define IPeekCapture_IID \
{ 0x781F5692, 0x7FFD, 0x4946, {0xBD, 0x35, 0x66, 0x6B, 0xA7, 0x3C, 0x10, 0xA0} }

class HE_NO_VTABLE IPeekCapture : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekCapture_IID);

	/// Get the adapter used by the capture.
	HE_IMETHOD GetAdapter(/*out*/ IAdapter** ppAdapter) = 0;
	/// Get the packet buffer.
	HE_IMETHOD GetPacketBuffer(/*out*/ IPacketBuffer** ppPacketBuffer) = 0;
	/// Get the packet processor.
	HE_IMETHOD GetPacketProcessor(/*out*/ IPacketProcessor** ppPacketProcessor) = 0;
	/// Get the start trigger.
	HE_IMETHOD GetStartTrigger(/*out*/ ITrigger** ppTrigger) = 0;
	/// Set the start trigger.
	HE_IMETHOD SetStartTrigger(/*in*/ ITrigger* pTrigger) = 0;
	/// Get the stop trigger.
	HE_IMETHOD GetStopTrigger(/*out*/ ITrigger** ppTrigger) = 0;
	/// Set the stop trigger.
	HE_IMETHOD SetStopTrigger(/*in*/ ITrigger* pTrigger) = 0;
	/// Get the capture name.
	HE_IMETHOD GetName(/*out*/ Helium::HEBSTR* pbstrName) = 0;
	/// Set the capture name.
	HE_IMETHOD SetName(/*in*/ Helium::HEBSTR bstrName) = 0;
	/// Get the capture ID.
	HE_IMETHOD GetID(/*out*/ Helium::HeID* pID) = 0;
	/// Get the capture status.
	HE_IMETHOD GetCaptureStatus(/*out*/ PeekCaptureStatus* pCaptureStatus) = 0;
	/// Get the date-time capture started in nanoseconds since midnight 1/1/1601 UTC.
	HE_IMETHOD GetStartDateTime(/*out*/ UInt64* pnStartDateTime) = 0;
	/// Get the date-time capture stopped in nanoseconds since midnight 1/1/1601 UTC.
	HE_IMETHOD GetStopDateTime(/*out*/ UInt64* pnStopDateTime) = 0;
	/// Get the capture duration in nanoseconds since midnight 1/1/1601 UTC.
	HE_IMETHOD GetDuration(/*out*/ SInt64* pnDuration) = 0;
	/// Get the count of packets received.
	HE_IMETHOD GetPacketsReceived(/*out*/ UInt64* pnPacketsReceived) = 0;
	/// Get the count of packets accepted by filters.
	HE_IMETHOD GetPacketsFiltered(/*out*/ UInt64* pnPacketsFiltered) = 0;
	/// Get the dropped packet count.
	HE_IMETHOD GetPacketsDropped(/*out*/ UInt64* pnPacketsDropped) = 0;
	/// Get the repeat trigger count.
	HE_IMETHOD GetTriggerCount(/*out*/ UInt32* pnTriggerCount) = 0;
	/// Get the total repeat trigger duration.
	HE_IMETHOD GetTriggerDuration(/*out*/ SInt64* pnTriggerDuration) = 0;
	/// Get the capture reset count. Basically the number of times capture
	/// has been started with full reset.
	HE_IMETHOD GetResetCount(/*out*/ UInt32* pnResetCount) = 0;
	/// Is it capturing?
	HE_IMETHOD IsCapturing(/*out*/ BOOL* pbCapturing) = 0;
	/// Start capture.
	HE_IMETHOD StartCapture(/*in*/ BOOL bFullStart) = 0;
	/// Stop capture.
	HE_IMETHOD StopCapture() = 0;
	/// Clear capture (dump all packets).
	HE_IMETHOD Clear() = 0;
	/// Get the filter mode.
	HE_IMETHOD GetFilterMode(/*out*/ PeekFilterMode* pFilterMode) = 0;
	/// Set the filter mode.
	HE_IMETHOD SetFilterMode(/*in*/ PeekFilterMode mode) = 0;
	/// Get the packet filter.
	HE_IMETHOD GetPacketFilter(/*out*/ IFilterPacket** ppPacketFilter) = 0;
	/// Set the packet filter.
	HE_IMETHOD SetPacketFilter(/*in*/ IFilterPacket* pPacketFilter) = 0;
	/// Get the Options Reset Count. This is more serious since adapters
	/// and other major things could have changed.
	HE_IMETHOD GetOptionsResetCount(/*out*/ UInt32* pnResetCount) = 0;
	/// Flush current packet data to storage.
	HE_IMETHOD Flush() = 0;
	/// Get the total number of bytes allocated and used.
	HE_IMETHOD GetCTDValues(/*out*/ UInt64* pnAllocated, /*out*/ UInt64* pnUsed) = 0;
};

/// \internal
/// \interface IPeekCapture2
/// \ingroup Capture
/// \brief Additional methods for PeekCapture.
/// \see IPeekCapture, coclass PeekCapture
#define IPeekCapture2_IID \
{ 0xE5111CE7, 0x06CC, 0x43B7, {0xAC, 0x6A, 0xB8, 0x38, 0xE2, 0xB9, 0x58, 0xD4} }

class HE_NO_VTABLE IPeekCapture2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekCapture2_IID);

	/// Stop capture.
	HE_IMETHOD StopCapture(/*in*/ BOOL bFullStop) = 0;
};

/// \interface IPeekCaptureEvents
/// \brief Capture events fired from PeekCapture connection point.
#define IPeekCaptureEvents_IID \
{ 0x27F33B38, 0xD5BC, 0x4368, {0x8C, 0x34, 0xA2, 0xDB, 0x57, 0x5B, 0x0D, 0x7A} }

class HE_NO_VTABLE IPeekCaptureEvents : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekCaptureEvents_IID);

	/// Capture about to start.
	HE_IMETHOD OnCaptureStarting() = 0;
	/// Capture started.
	HE_IMETHOD OnCaptureStarted() = 0;
	/// Capture about to stop.
	HE_IMETHOD OnCaptureStopping() = 0;
	/// Capture stopped.
	HE_IMETHOD OnCaptureStopped() = 0;
};

/// \interface IPeekCaptureEvents2
/// \brief Capture events fired from PeekCapture connection point.
#define IPeekCaptureEvents2_IID \
{ 0x5DD5732E, 0xDBF3, 0x42C4, {0xA9, 0x43, 0xC3, 0x6D, 0xEA, 0xB3, 0xD7, 0xD8} }

class HE_NO_VTABLE IPeekCaptureEvents2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekCaptureEvents2_IID);

	/// Capture data start time changed.
	HE_IMETHOD OnCaptureDataStartTimeChanged(/*in*/ UInt64 nDataStartTime) = 0;
};

/// \internal
/// \interface IPeekCaptures
/// \ingroup Capture
/// \brief List of peek captures.
/// \see IPeekCapture, coclass PeekCaptures
#define IPeekCaptures_IID \
{ 0x3DC0B474, 0x3940, 0x4F27, {0xB2, 0xC2, 0x19, 0xC0, 0xF7, 0xE5, 0x8B, 0x94} }

class HE_NO_VTABLE IPeekCaptures : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekCaptures_IID);

	/// Get peek capture by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ IPeekCapture** ppPeekCapture) = 0;
	/// Get the count of peek captures in the list.
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	/// Get a peek capture by ID.
	HE_IMETHOD ItemFromID(/*in*/ const Helium::HeID& guidID, /*out*/ IPeekCapture** ppPeekCapture) = 0;
};

/// \internal
/// \interface IPeekSetup
/// \ingroup Capture
/// \brief Top-level container for captures, etc.
/// \see IPeekCapture, coclass PeekSetup
#define IPeekSetup_IID \
{ 0xD375942A, 0x321A, 0x4954, {0x98, 0xD8, 0xFA, 0xD0, 0x09, 0xED, 0xD6, 0xA8} }

class HE_NO_VTABLE IPeekSetup : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekSetup_IID);

	/// Get the list of captures.
	HE_IMETHOD GetCaptures(/*out*/ IPeekCaptures** ppPeekCaptures) = 0;
	/// Initialize - loads ProtoSpecs, creates default services.
	HE_IMETHOD Initialize() = 0;
	/// Create a new capture.
	HE_IMETHOD NewCapture(/*in*/ Helium::HEVARIANT vtCaptureSettings, /*out*/ IPeekCapture** ppPeekCapture) = 0;
	/// Delete a capture.
	HE_IMETHOD DeleteCapture(/*in*/ IPeekCapture* pPeekCapture) = 0;
};

/// \internal
/// \interface IStartable
/// \ingroup Capture
/// \brief For services that support Start/Stop with a capture
/// \see IPeekCapture, coclass PeekSetup
#define IStartable_IID \
{ 0x04092B8D, 0x31CF, 0x47B2, {0x90, 0xE8, 0x73, 0x91, 0xE6, 0xF5, 0x02, 0xC8} }

class HE_NO_VTABLE IStartable : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IStartable_IID);

	/// Capture is starting.
	HE_IMETHOD Start() = 0;
	/// Capture is stopping.
	HE_IMETHOD Stop() = 0;
	/// Reset the object.
	HE_IMETHOD Reset() = 0;
};

/// \internal
/// \interface ICaptureTemplateItem
/// \ingroup Capture
#define ICaptureTemplateItem_IID \
{ 0xBCDDD082, 0x2994, 0x4F86, {0xA3, 0x7C, 0xBD, 0x02, 0x48, 0x15, 0x87, 0xD9} }

class HE_NO_VTABLE ICaptureTemplateItem : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICaptureTemplateItem_IID);

	// Get the property bag.
	HE_IMETHOD GetPropertyBag(/*out*/ Helium::IHePropertyBag** ppVal) = 0;
	// Put a property bag.
	HE_IMETHOD SetPropertyBag(/*in*/ Helium::IHePropertyBag* pVal) = 0;
	// Get the id.
	HE_IMETHOD GetId(/*out*/ Helium::HeID* pVal) = 0;
	// Set the id.
	HE_IMETHOD SetId(/*in*/ Helium::HeID newVal) = 0;
	// Get the title
	HE_IMETHOD GetTitle(/*out*/ Helium::HEBSTR* pVal) = 0;
};

/// \internal
/// \interface ICaptureTemplateCollection
/// \ingroup Capture
#define ICaptureTemplateCollection_IID \
{ 0xADB387CF, 0xB983, 0x42A3, {0x97, 0xBA, 0xE6, 0xCD, 0x73, 0xDA, 0x3B, 0x69} }

class HE_NO_VTABLE ICaptureTemplateCollection : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICaptureTemplateCollection_IID);

	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ ICaptureTemplateItem** ppCaptureTemplate) = 0;
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	HE_IMETHOD ItemFromID(/*in*/ const Helium::HeID& id, /*out*/ ICaptureTemplateItem** ppItem, /*out*/ BOOL* pbResult) = 0;
	HE_IMETHOD Add(/*in*/ Helium::IHeUnknown* pCaptureTemplate) = 0;
	HE_IMETHOD Remove(/*in*/ Helium::IHeUnknown* pCaptureTemplate) = 0;
	HE_IMETHOD RemoveAll() = 0;
};

/// \internal
/// \enum MutableItemOpsType
enum MutableItemOpsType
{
	mioTypeInvalid,		///< Initial state - invalid
	mioTypeAdd,			///< Add this item
	mioTypeModify,		///< Modify
	mioTypeDelete		///< Delete
};

/// \internal
/// \interface IMutableItem
#define IMutableItem_IID \
{ 0x38E25EED, 0x8984, 0x4699, {0xB3, 0x36, 0x54, 0x8A, 0x34, 0x8D, 0x2C, 0xC1} }

class HE_NO_VTABLE IMutableItem : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IMutableItem_IID);

	HE_IMETHOD GetItem(/*out*/ Helium::IHeUnknown** pVal) = 0;
	HE_IMETHOD SetItem(/*in*/ Helium::IHeUnknown* newVal) = 0;
	HE_IMETHOD GetOpType(/*out*/ MutableItemOpsType* pType) = 0;
	HE_IMETHOD SetOpType(/*in*/ MutableItemOpsType newVal) = 0;
	HE_IMETHOD GetResultCode(/*out*/ HeResult* pResult) = 0;
	HE_IMETHOD SetResultCode(/*in*/ HeResult hr) = 0;
};

/// \internal
/// \interface IMutableCollection
#define IMutableCollection_IID \
{ 0x8C580471, 0x6610, 0x4327, {0xA3, 0xBA, 0xDA, 0xB3, 0x61, 0x03, 0xEB, 0x94} }

class HE_NO_VTABLE IMutableCollection : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IMutableCollection_IID);

	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	HE_IMETHOD GetItem(/*in*/ SInt32 index, /*out*/ IMutableItem** ppItem) = 0;
	HE_IMETHOD Add(/*in*/ IMutableItem* pItem) = 0;
	HE_IMETHOD Remove(/*in*/ SInt32 index) = 0;
};

/// \internal
/// \interface IPacketFileInfo
/// \ingroup PacketBuffer
/// \brief Gathers information about a packet file
/// \see coclass PacketFileInfo
#define IPacketFileInfo_IID \
{ 0x81B14DB3, 0x17E4, 0x484B, {0xA1, 0x7C, 0x98, 0x33, 0xF4, 0x9F, 0xFE, 0x7E} }

class HE_NO_VTABLE IPacketFileInfo : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketFileInfo_IID);

	/// Get a new enumerator that supports IEnumVARIANT and IEnumProperties.
	HE_IMETHOD GetNewEnum(/*out*/ Helium::IHeUnknown** ppEnum) = 0;
	/// Get an item by name or zero-based numbered index.
	HE_IMETHOD GetItem(/*in*/ Helium::HEVARIANT varIndex, /*out*/ Helium::HEVARIANT* pVal) = 0;
	/// Get an item by name (easy for non-automation clients).
	HE_IMETHOD GetItemFromName(/*in*/ const wchar_t* pszName, /*in*/ Helium::HEVARTYPE vt, /*out*/ Helium::HEVARIANT* pVal) = 0;
	/// Get the name for an item.
	HE_IMETHOD GetItemName(/*in*/ SInt32 iIndex, /*out*/ Helium::HEBSTR* pbstr) = 0;
	/// Get the number of properties in the collection.
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	/// Read info for a packet file.
	HE_IMETHOD Read(/*in*/ Helium::HEBSTR bstrFile) = 0;
	/// Refresh the file info.
	HE_IMETHOD Refresh() = 0;
};

/// \internal
/// \interface IPacketFileDatabase
/// \ingroup PacketBuffer
/// \brief Synchronizes information about packet files on disk to a database.
#define IPacketFileDatabase_IID \
{ 0xED8DC8E3, 0x25C6, 0x4E57, {0xBA, 0x36, 0x62, 0x4C, 0xEF, 0x55, 0x7F, 0x41} }

class HE_NO_VTABLE IPacketFileDatabase : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPacketFileDatabase_IID);

	/// Synchronize the database to the file system.
	HE_IMETHOD Sync(/*in*/ Helium::HEBSTR bstrDir) = 0;
	/// Generate statistics from packet files.
	HE_IMETHOD Index() = 0;
	/// Synchronize one file.
	HE_IMETHOD SyncFile(/*in*/ Helium::HEBSTR bstrFile) = 0;
};

/// \internal
/// \enum PeekFileViewStatus
/// \ingroup PacketBuffer
/// \brief Status for a file view from object properties.
/// \see IFileView
enum PeekFileViewStatus
{
	peekFileViewStatusClosed,
	peekFileViewStatusOpening,
	peekFileViewStatusComplete
};

/// \internal
/// \interface IFileView
/// \ingroup PacketBuffer
#define IFileView_IID \
{ 0xB05537A7, 0xC0B7, 0x4B62, {0x9C, 0x03, 0x72, 0x22, 0x11, 0xE2, 0x7A, 0x13} }

class HE_NO_VTABLE IFileView : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IFileView_IID);

#define PEEK_E_FILEVIEW_NO_DATA					((HeResult)	0x80040200L)
#define PEEK_E_FILEVIEW_SEARCH_LIMIT_REACHED	((HeResult)	0x80040201L)
#define PEEK_E_FILEVIEW_ALL_FILES_USED			((HeResult)	0x80040202L)
#define PEEK_E_FILEVIEW_INVALID_SEARCH			((HeResult)	0x80040211L)

	/// Get the ID.
	HE_IMETHOD GetID(/*out*/ Helium::HeID* pID) = 0;
	/// Get the packet buffer.
	HE_IMETHOD GetPacketBuffer(/*out*/ IPacketBuffer** ppPacketBuffer) = 0;
	/// Open the file view.
	/// \return PEEK_E_FILEVIEW_NO_DATA No data was found
	HE_IMETHOD Open(/*in*/ Helium::IHeUnknown* pQuery) = 0;
	/// Close the file view.
	HE_IMETHOD Close() = 0;
	/// Stop loading files into the view.
	HE_IMETHOD StopLoad() = 0;
};

/// Possible errors for creating a new capture.
#define PEEK_E_UNSUPPORTED_CAPTURE				((HeResult)	0x80040200L)	///< Generic: attempt to create an unsupported capture.
#define PEEK_E_NOT_ENOUGH_SPACE					((HeResult)	0x80040201L)	///< Not enough storage space to create capture.
#define PEEK_E_DUPLICATE_CAPTURE				((HeResult)	0x80040202L)	///< Duplicate capture to disk capture on the same interface.
#define PEEK_E_INVALID_ADAPTER					((HeResult)	0x80040203L)	///< Invalid capture adapter specified.

/// \internal
/// \interface ICreateCapture 
/// \ingroup Capture
/// \brief Remote adapter and capture management.
// {35076C8A-F6F3-4729-9E92-28F35839167B}
#define ICreateCapture_IID \
{ 0x35076c8a, 0xf6f3, 0x4729, {0x9e, 0x92, 0x28, 0xf3, 0x58, 0x39, 0x16, 0x7b} }
class HE_NO_VTABLE ICreateCapture : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICreateCapture_IID);

	HE_IMETHOD DoCreateCapture(/*in*/ Helium::IHePropertyBag* pCaptureTemplate, /*out*/ Helium::HeID * pCaptureId) = 0;
};

#define NdisAdapterInfo_CID \
{ 0x56A9F62B, 0x0048, 0x469E, {0xBD, 0x47, 0x12, 0x1A, 0xB1, 0x32, 0x37, 0xC6} }

#define ReplayAdapterInfo_CID \
{ 0x0998E290, 0x3B42, 0x494F, {0x9B, 0xB3, 0x55, 0x9F, 0x2E, 0xB9, 0x1A, 0x23} }

#define PluginAdapterInfo_CID \
{ 0x24A9A571, 0xC7DD, 0x43BE, {0x80, 0x08, 0x2E, 0x26, 0xF4, 0x7C, 0xE7, 0xCA} }

#define PcapAdapterInfo_CID \
{ 0x38FAE028, 0x7F55, 0x4CCF, {0x8E, 0x00, 0xF4, 0x5C, 0xE3, 0x8B, 0x15, 0x9F} }

#define GigAdapterInfo_CID \
NdisAdapterInfo_CID

#define XyratexUtil_CID \
{ 0x275E7DF2, 0xD7C8, 0x40A7, {0x89, 0xB7, 0x1B, 0x41, 0xEA, 0xC4, 0x39, 0x3F} }

#define NapatechUtil_CID \
{ 0x3538628C, 0xF4DC, 0x4916, {0x8A, 0x68, 0x3B, 0xDF, 0x5A, 0x57, 0x40, 0xE3} }

#define AdapterInfoList_CID \
{ 0xAF18BBB0, 0xCD28, 0x456A, {0x8C, 0x32, 0x98, 0x54, 0xD7, 0x03, 0x92, 0x2C} }

#define AdapterConfig_CID \
{ 0xF1E1AD7C, 0x26F2, 0x4200, {0x8E, 0x68, 0xA1, 0xFF, 0xE8, 0x6F, 0x5F, 0x78} }

#define WEPKeySet_CID \
{ 0x30215047, 0x59C4, 0x46D1, {0xA8, 0x69, 0xC2, 0x9E, 0x77, 0xEE, 0xD3, 0x66} }

#define GigHardwareOptions_CID \
{ 0x7DA20E4B, 0xA157, 0x469D, {0xAB, 0x1A, 0xF4, 0x54, 0xD6, 0xCA, 0x5A, 0xE9} }

#define WirelessHardwareOptions_CID \
{ 0xAE727C8B, 0x0531, 0x45C3, {0xA9, 0x7A, 0x0E, 0x32, 0x91, 0x66, 0x17, 0x6C} }

#define WANHardwareOptions_CID \
{ 0xF64FCB14, 0xBCA4, 0x4707, {0x8A, 0xE9, 0x08, 0x63, 0x49, 0x51, 0x20, 0x88} }

#define HardwareOptionsCollection_CID \
{ 0x0791CBB0, 0x84BD, 0x42C6, {0x9D, 0x53, 0x83, 0x24, 0xEB, 0x6D, 0x47, 0x77} }

#define NapatechHardwareChannelFilter_CID \
{ 0x28E7800C, 0x963E, 0x4B1E, {0x81, 0x63, 0xFD, 0x42, 0x82, 0x3E, 0x09, 0x80} }

#define NapatechHardwareFilter_CID \
{ 0xF20D3A99, 0x19A3, 0x4B32, {0x9F, 0x0D, 0x81, 0x10, 0xC8, 0xF7, 0xC7, 0xEC} }

#define ProtoSpecsWrapper_CID \
{ 0x03A00258, 0x9C1A, 0x43AF, {0x8B, 0xA1, 0xF0, 0x6D, 0xFF, 0x1D, 0x18, 0xB9} }

#define FilePacketBuffer_CID \
{ 0xCF6746B2, 0xF5C6, 0x403F, {0xB0, 0xDF, 0xF6, 0xE3, 0xCF, 0x24, 0x30, 0x16} }

#define MultiFilePacketBuffer_CID \
{ 0xB9638CC0, 0xF16A, 0x4F58, {0x97, 0xBE, 0x8B, 0x07, 0x7D, 0x85, 0x88, 0x90} }

#define MultiFilePacketBuffer2_CID \
{ 0x9EA039EA, 0x68EB, 0x459B, {0x9B, 0x07, 0x32, 0x88, 0xC4, 0x9B, 0x75, 0x0D} }

#define RingFilePacketBuffer_CID \
{ 0xE830CC2D, 0x9BFA, 0x4870, {0xB5, 0xCB, 0x3B, 0x30, 0xF8, 0xD2, 0x1A, 0x2D} }

#define MediaUtils_CID \
{ 0x1A9CF25B, 0xC05A, 0x41D0, {0xBE, 0x41, 0x59, 0xD2, 0x81, 0x81, 0xD6, 0xF2} }

#define Packet_CID \
{ 0xFB560ED9, 0x5DA2, 0x40DE, {0x95, 0xEB, 0x1B, 0x06, 0x6A, 0xE4, 0xF5, 0xB6} }

#define MemPacketBuffer_CID \
{ 0x26399FA2, 0xBA20, 0x4EB0, {0xB6, 0x79, 0x00, 0xE9, 0x2C, 0xCB, 0x86, 0xBC} }

#define MessageLog_CID \
{ 0xDB92BF66, 0x6F8E, 0x4551, {0xAA, 0x2E, 0x15, 0x58, 0x8F, 0x01, 0xDD, 0x92} }

#define PeekSecurity_CID \
{ 0x8EE5CE89, 0xAA54, 0x4A87, {0x95, 0xD1, 0x30, 0x9C, 0x25, 0x6F, 0xE6, 0xF1} }

#define NdisAdapter_CID \
{ 0x87DB480B, 0xB166, 0x4B80, {0xA9, 0x3D, 0x39, 0x74, 0x59, 0x43, 0xD4, 0x2F} }

#define ReplayAdapter_CID \
{ 0x84E08171, 0x4EE0, 0x4986, {0xAB, 0x23, 0x0B, 0x7C, 0xDE, 0xB8, 0xE1, 0xEB} }

#define PcapAdapter_CID \
{ 0x13B282F4, 0x5CD6, 0x415F, {0xBF, 0xF2, 0xB7, 0xC0, 0x16, 0xD4, 0x01, 0x43} }

#define GigAdapter_CID \
{ 0x1D89ECCB, 0xD4DD, 0x43DE, {0xB7, 0xEE, 0x7A, 0x77, 0x3C, 0x0A, 0xE9, 0x81} }

#define RemoteAdapter_CID \
{ 0x50272EFC, 0x41F0, 0x443C, {0x8C, 0x02, 0x97, 0x0F, 0xB9, 0xD8, 0x5F, 0xF5} }

#define AdapterManager_CID \
{ 0xAECE9C73, 0xF727, 0x415B, {0x82, 0xDF, 0x67, 0xCE, 0x96, 0x18, 0xAD, 0xA1} }

#define PacketProcessor_CID \
{ 0x5841650E, 0x224B, 0x466F, {0x90, 0xC5, 0x2A, 0x2B, 0x57, 0xA6, 0x2E, 0xE8} }

#define PacketSegmentProcessor_CID \
{ 0xE65AA655, 0x94D8, 0x49DF, {0x84, 0x87, 0x99, 0x1F, 0xCB, 0x70, 0xA0, 0xDD} }

#define RingPacketBuffer_CID \
{ 0x2D274248, 0xE829, 0x4366, {0xBC, 0x31, 0x02, 0xDD, 0xCD, 0x01, 0xEB, 0x8C} }

#define TriggerEvents_CID \
{ 0x51EC8690, 0x2051, 0x456A, {0xBC, 0x90, 0x9D, 0x7A, 0xEA, 0x02, 0x81, 0xA6} }

#define Trigger_CID \
{ 0xE801FFF9, 0xAE26, 0x4DD7, {0xA3, 0x49, 0x45, 0x06, 0xCF, 0x3D, 0x09, 0x0E} }

#define PluginManager_CID \
{ 0xDA90EB56, 0xDCBE, 0x4C28, {0x9C, 0x7E, 0xF8, 0xF1, 0x05, 0x51, 0xF8, 0xEE} }

#define PeekCapture_CID \
{ 0x9BDA7C1D, 0xD372, 0x4973, {0xB6, 0xF8, 0x8B, 0x45, 0xC7, 0xC7, 0x39, 0x46} }

#define PeekCaptures_CID \
{ 0xD22167FB, 0x6C04, 0x4322, {0x86, 0x74, 0x90, 0x76, 0xEA, 0xE2, 0xA3, 0x61} }

#define PeekSetup_CID \
{ 0x1498F1A3, 0x95FB, 0x4CEA, {0x84, 0xD7, 0xE0, 0xFD, 0x42, 0x88, 0xF3, 0x46} }

#define FilterTriggerEvent_CID \
{ 0x505A9EAB, 0x78BD, 0x461A, {0x9D, 0xC0, 0x20, 0x3B, 0xDF, 0x0E, 0x88, 0x37} }

#define TimeTriggerEvent_CID \
{ 0xEC5E8097, 0xB3D5, 0x4B8D, {0xAA, 0x64, 0xF6, 0xC6, 0xB1, 0x3E, 0xCB, 0x37} }

#define BytesCapturedTriggerEvent_CID \
{ 0x1945F627, 0x3E8F, 0x462F, {0x96, 0xD9, 0x99, 0xFB, 0x75, 0x45, 0x4F, 0x9C} }

#define FilterConfig_CID \
{ 0x84D2EAC7, 0x3B74, 0x420E, {0x8E, 0xEE, 0x85, 0xC7, 0x16, 0x5B, 0xB4, 0x71} }

#define PluginsConfig_CID \
{ 0x862E83A1, 0xBC76, 0x479A, {0xA7, 0xFD, 0x06, 0xCD, 0x64, 0x51, 0x81, 0x5A} }

#define AlarmConfig_CID \
{ 0xE18AF1CF, 0x7F83, 0x45A7, {0x95, 0xCB, 0x41, 0x01, 0x38, 0xFB, 0xC3, 0xE3} }

#define PeekCapturePerfItem_CID \
{ 0x8D1D1FB8, 0x4AF7, 0x40D1, {0x8D, 0xDB, 0x51, 0x53, 0x7B, 0x54, 0x42, 0x72} }

#define PeekCapturePerf_CID \
{ 0xD0455177, 0x25FB, 0x4E30, {0xAF, 0x5C, 0x72, 0x76, 0x4B, 0xDB, 0x54, 0x05} }

#define ChannelManager_CID \
{ 0x1BABAFF2, 0x64C1, 0x4636, {0xAF, 0x25, 0xCB, 0xF7, 0xDA, 0x8D, 0x80, 0x18} }

#define CaptureTemplateItem_CID \
{ 0xBD360857, 0x1394, 0x47DD, {0x9E, 0x11, 0x34, 0x07, 0x81, 0x66, 0x4E, 0xD7} }

#define CaptureTemplateCollection_CID \
{ 0xF2A9D611, 0xF285, 0x4895, {0x88, 0x91, 0x10, 0x4D, 0x59, 0x37, 0xC3, 0xCC} }

#define MutableCollection_CID \
{ 0x81B739DD, 0x9EE6, 0x4CB1, {0xA8, 0x4E, 0xBD, 0x35, 0x1C, 0x00, 0x14, 0xB5} }

#define MutableItem_CID \
{ 0x12435681, 0x3B3F, 0x404C, {0x97, 0x69, 0x37, 0x73, 0x39, 0x8D, 0x44, 0x1F} }

#define PacketFileInfo_CID \
{ 0x4765D61D, 0x26E7, 0x4605, {0xAA, 0x84, 0x19, 0xD9, 0x87, 0x0C, 0xE2, 0x91} }

#define PacketFileDatabase_CID \
{ 0x2EE8AD1A, 0xF359, 0x4C48, {0xA6, 0x6F, 0xEE, 0xD4, 0xA6, 0x8B, 0x7B, 0x60} }

#define FileView_CID \
{ 0x69646F71, 0x77FF, 0x4B65, {0xA2, 0xAF, 0xBF, 0x2F, 0x86, 0x6E, 0x73, 0x77} }

#define MediaInfo_CID \
{ 0x87FEDA06, 0xD1B6, 0x425C, {0x84, 0x11, 0x0D, 0xD1, 0xBD, 0x89, 0xFC, 0x37} }

#endif /* PEEKCORE_H */
