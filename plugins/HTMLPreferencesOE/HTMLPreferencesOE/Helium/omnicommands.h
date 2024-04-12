// =============================================================================
//	omnicommands.h
// =============================================================================
//	Copyright (c) 2003-2015	Savvius, Inc. All rights reserved.

#ifndef OMNICOMMANDS_H
#define OMNICOMMANDS_H

/// \struct PluginMessageHdr
/// \brief Beginning of the plugin message. It is used to route a message from a
/// client plugin to a server plugin and back again.
#include "hepushpack.h"
struct PluginMessageHdr
{
	UInt32			Size;			///< Size of this structure.
	Helium::HeID	CaptureID;		///< ID of the capture.
	Helium::HeID	PluginID;		///< ID of the plugin.
	UInt32			MessageSize;	///< Length of the message data.
	HeResult		MessageResult;	///< Message result code.
} HE_PACK_STRUCT;
#include "hepoppack.h"

// -----------------------------------------------------------------------------
//	Command IDs
//	
//	For Every command there will be a unique id that identifies the command
//	dispatcher (at the client  i.e., Console) and a handler (at the server
//	i.e., Remote Engine)
// -----------------------------------------------------------------------------

namespace OmniProtocolCommands
{
	// Command IDs for operational commands.
	const UInt32 OMNI_GET_VERSION								= 100;
	const UInt32 OMNI_GET_STATUS								= 110;
	const UInt32 OMNI_GET_CAPABILITIES							= 111;
	const UInt32 OMNI_GET_LOG_MSGS								= 112;
	const UInt32 OMNI_GET_ADAPTER_LIST							= 113;
	const UInt32 OMNI_GET_CAPTURE_LIST							= 114;
	const UInt32 OMNI_GET_CAPTURE_PROPS							= 115;
	const UInt32 OMNI_CREATE_CAPTURE							= 116;
	const UInt32 OMNI_DELETE_CAPTURES							= 117;
	const UInt32 OMNI_GET_CAPTURE_OPTIONS						= 118;
	const UInt32 OMNI_SET_CAPTURE_OPTIONS						= 119;
	const UInt32 OMNI_START_CAPTURES							= 120;
	const UInt32 OMNI_STOP_CAPTURES								= 121;
	const UInt32 OMNI_CLEAR_CAPTURES							= 122;
	const UInt32 OMNI_GET_STATS									= 123;
	const UInt32 OMNI_TAKE_SNAPSHOT								= 124;
	const UInt32 OMNI_GET_FILTERS								= 125;
	const UInt32 OMNI_SET_FILTERS								= 126;
	const UInt32 OMNI_GET_FILTER_CONFIG							= 127;
	const UInt32 OMNI_SET_FILTER_CONFIG							= 128;
	const UInt32 OMNI_GET_NOTIFICATIONS							= 129;
	const UInt32 OMNI_SET_NOTIFICATIONS							= 130;
	const UInt32 OMNI_GET_PACKETS								= 131;
	const UInt32 OMNI_SELECT_RELATED							= 132;
	const UInt32 OMNI_CLEAR_LOG									= 133;
//	const UInt32 OMNI_GET_EXPERT_SETTINGS						= 134;	// Obsolete, used by OWS.
//	const UInt32 OMNI_SET_EXPERT_SETTINGS						= 135;	// Obsolete, used by OWS.
//	const UInt32 OMNI_GET_ACTIVE_STREAM_LIST					= 136;
	const UInt32 OMNI_GET_FILE_LIST								= 137;
	const UInt32 OMNI_GET_FILE									= 138;
	const UInt32 OMNI_DELETE_FILES								= 139;
	const UInt32 OMNI_SAVE_CAPTURE_FILE							= 140;
	const UInt32 OMNI_GET_ALARMS								= 141;
	const UInt32 OMNI_SET_ALARMS								= 142;
	const UInt32 OMNI_GET_ALARM_CONFIG							= 143;
	const UInt32 OMNI_SET_ALARM_CONFIG							= 144;
	const UInt32 OMNI_GET_ALARM_STATES							= 145;
	const UInt32 OMNI_MODIFY_ALARMS								= 146;
//	const UInt32 OMNI_DELETE_ALARMS								= 147;	// Never used.
	const UInt32 OMNI_SELECT_FILTER								= 148;
	const UInt32 OMNI_SAVE_SELECTED								= 149;
	const UInt32 OMNI_DELETE_SNAPSHOTS							= 150;
	const UInt32 OMNI_SELECT_FILTER_CONFIG						= 151;
	const UInt32 OMNI_SWITCH_SETTINGS							= 152;
	const UInt32 OMNI_EXPERT_PERFORM_QUERY						= 153;
	const UInt32 OMNI_SELECT_EXPERT								= 154;
	const UInt32 OMNI_GET_HARDWARE_OPTIONS						= 155;
	const UInt32 OMNI_SET_HARDWARE_OPTIONS						= 156;
	const UInt32 OMNI_GET_ADAPTER_CONFIG						= 157;
	const UInt32 OMNI_SET_ADAPTER_CONFIG						= 158;
	const UInt32 OMNI_EXPERT_SAVE_AS							= 159;
	const UInt32 OMNI_GET_GRAPH_TEMPLATES						= 160;
	const UInt32 OMNI_MODIFY_GRAPH_TEMPLATE						= 161;
	const UInt32 OMNI_DELETE_GRAPH_TEMPLATES					= 162;
	const UInt32 OMNI_GET_GRAPH_COLLECTION						= 163;
	const UInt32 OMNI_GET_GRAPH_DATA							= 164;
	const UInt32 OMNI_CAPTURE_ADDGRAPHS							= 165;
	const UInt32 OMNI_CAPTURE_DELETEGRAPHS						= 166;
	const UInt32 OMNI_GET_NAMES									= 167;
	const UInt32 OMNI_SET_NAMES									= 168;
	const UInt32 OMNI_GET_CAPTURE_TEMPLATE_COLLECTION			= 169;
	const UInt32 OMNI_CREATE_CAPTURE_TEMPLATE					= 170;
	const UInt32 OMNI_DELETE_CAPTURE_TEMPLATE					= 171;
	const UInt32 OMNI_MODIFY_CAPTURE_TEMPLATE					= 172;
	const UInt32 OMNI_MODIFY_FILTERS							= 173;
	const UInt32 OMNI_MODIFY_CAPTURE_TEMPLATES					= 174;
	const UInt32 OMNI_MODIFY_ALARM_LIST							= 175;
	const UInt32 OMNI_MODIFY_GRAPH_TEMPLATES					= 176;
	const UInt32 OMNI_GET_DEFAULT_STATSCONTEXT					= 177;
	const UInt32 OMNI_EXPERT_EXECUTE_QUERY						= 178;
	const UInt32 OMNI_PLUGIN_MESSAGE							= 179;
	const UInt32 OMNI_GET_GRAPH_STATS_CTX						= 180;
	const UInt32 OMNI_GET_STAT									= 181;
//	const UInt32 OMNI_GET_SUMMARY_STATS							= 182;	// Never used.
	const UInt32 OMNI_EXPERT_EXECUTE_EXECUTE					= 183;
	const UInt32 OMNI_GET_PACKETFILE_TABLE						= 184;
	const UInt32 OMNI_CREATE_FILE_VIEW							= 185;
	const UInt32 OMNI_DELETE_FILE_VIEW							= 186;
	const UInt32 OMNI_DELETE_CAPTURE_SESSION					= 187;
	const UInt32 OMNI_CREATE_REPORT								= 188;
	const UInt32 OMNI_LOCK_FILTERS								= 189;
	const UInt32 OMNI_LOCK_CAPTURE								= 190;
	const UInt32 OMNI_LOCK_ADAPTER_CONFIG						= 191;
	const UInt32 OMNI_GET_LOCK_INFO								= 192;
	const UInt32 OMNI_DELETE_REPORT_FILES						= 193;
	const UInt32 OMNI_PACKETFILE_TABLE_NO_WAIT					= 194;
	const UInt32 OMNI_PACKETFILE_TABLE_NO_WAIT_COMPLETED		= 195;
	const UInt32 OMNI_GET_COMMAND_LIST							= 196;
	const UInt32 OMNI_STOPLOAD_FILE_VIEW						= 197;
	const UInt32 OMNI_SELECT_OBJECT								= 198;
	const UInt32 OMNI_SYNC_DATABASE								= 199;
	const UInt32 OMNI_INDEX_PACKET_FILES						= 200;
	const UInt32 OMNI_GET_CAPTURE_SESSIONS						= 201;
	const UInt32 OMNI_GET_TIMELINE_DATA							= 202;
	const UInt32 OMNI_DELETE_ALL_CAPTURE_SESSIONS				= 203;
	const UInt32 OMNI_SELECT_RELATED_TASK						= 204;
	const UInt32 OMNI_SELECT_FILTER_TASK						= 205;
	const UInt32 OMNI_SELECT_FILTER_CONFIG_TASK					= 206;
	const UInt32 OMNI_SELECT_EXPERT_TASK						= 207;
	const UInt32 OMNI_GET_TIMELINE_STATS						= 208;
	const UInt32 OMNI_GET_GRAPH_DATA_2							= 209;
	const UInt32 OMNI_GET_FILE_VIEW_LIST						= 210;
	const UInt32 OMNI_GET_EXPERT_PREFS							= 211;
	const UInt32 OMNI_SET_EXPERT_PREFS							= 212;
	const UInt32 OMNI_SET_FILE									= 213;
	const UInt32 OMNI_GET_PACKET_NUMBERS_FROM_TIMES				= 214;
	const UInt32 OMNI_PEEK_PLUGIN_MESSAGE						= 215;
	const UInt32 OMNI_SET_ADAPTER_NAME							= 216;
	const UInt32 OMNI_GET_PACKET_LIST							= 217;
	const UInt32 OMNI_FILTER_PACKET_NUMBERS_WITHIN_TIME_RANGE	= 218;
	const UInt32 OMNI_DELETE_ADAPTERS							= 219;
	const UInt32 OMNI_GET_SECURITY								= 220;
	const UInt32 OMNI_CLEAR_SECURITY							= 221;
	const UInt32 OMNI_GET_SECURITY_SETTINGS						= 222;

	const UInt32 OMNI_QUERY_DATABASE							= 400;

	// Command IDs for administrative commands.
	const UInt32 OMNI_GET_ENGINE_SETTINGS						= 500;
	const UInt32 OMNI_SET_ENGINE_SETTINGS						= 501;
//	const UInt32 OMNI_GET_ENGINE_STATS							= 502;		// Only used by OmniScript and never implemented.
//	const UInt32 OMNI_GET_CLIENT_STATS							= 503;		// Only used by OmniScript and never implemented.
//	const UInt32 OMNI_GET_OPS_CMD_LIST							= 504;		// Only used by OmniScript and never implemented.
//	const UInt32 OMNI_GET_ADM_CMD_LIST							= 505;		// Only used by OmniScript.
	const UInt32 OMNI_GET_COMPUTER_ACCT_LIST					= 506;
	const UInt32 OMNI_GET_DIR_LIST								= 507;
	const UInt32 OMNI_RESTART_ENGINE							= 508;
//	const UInt32 OMNI_GET_CERTIFICATES							= 509;		// Never used, never implemented.
//	const UInt32 OMNI_SELECT_CERTIFICATE						= 510;		// Never used, never implemented.
	const UInt32 OMNI_GET_CONNECTED_USERS						= 511;
	const UInt32 OMNI_DISCONNECT_USERS							= 512;
	const UInt32 OMNI_GET_UPDATE_SVC_INFO						= 513;
	const UInt32 OMNI_SET_UPDATE_SVC_INFO						= 514;
	const UInt32 OMNI_GET_ACL									= 515;		// Only used by OmniScript.
	const UInt32 OMNI_SET_ACL									= 516;
	const UInt32 OMNI_CREATE_DIR								= 517;
//	const UInt32 OMNI_DELETE_DIR								= 518;
//	const UInt32 OMNI_SAVE_SETINGS								= 519;		// Never used, never implemented.
	const UInt32 OMNI_GET_TIME									= 520;
	const UInt32 OMNI_SET_TIME									= 521;
//	const UInt32 OMNI_ADMCMD_GET_COMMAND_LIST					= 522;		// Obsolete, use OMNI_GET_COMMAND_LIST
	const UInt32 OMNI_GET_INSTALLED_PLUGINS						= 523;

	const UInt32 OMNI_SET_DEF_ACCOUNT							= 550;		// Used only with OWS?

//	const UInt32 OMNI_ECHO										= 600;		// Useless.

	const UInt32 OMNI_GET_SERVICE_LOG_LEVEL						= 701;
	const UInt32 OMNI_SET_SERVICE_LOG_LEVEL						= 702;
	const UInt32 OMNI_GET_SERVICE_LOG							= 703;		// Never used, never implemented.
	const UInt32 OMNI_CLEAR_SERVICE_LOG							= 704;		// Never used, never implemented.

	const UInt32 OMNI_GET_AUDIT_LOG								= 710;
	const UInt32 OMNI_CLEAR_AUDIT_LOG							= 711;
	const UInt32 OMNI_QUERY_AUDIT_LOG							= 712;

	const UInt32 OMNI_GET_DIAGNOSTICS_INFO						= 720;

	const UInt32 OMNI_DEBUG_COMMAND								= 730;

	const UInt32 OMNI_SET_IP									= 802;		// OWS only. Never used.
	const UInt32 OMNI_GET_NTP_SERVER							= 803;		// Never used?
	const UInt32 OMNI_SET_NTP_SERVER							= 804;		// OWS only.

} /* namespace OmniProtocolCommands */

#endif /* OMNICOMMANDS_H */
