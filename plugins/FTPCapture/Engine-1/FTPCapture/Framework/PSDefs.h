// ============================================================================
//	PSDefs.h
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "Protospecs.h"
#include "PSIDs.h"

// *** PSIDs.h replaces this file

// Also defined in PSIDs.h
#ifndef IS_ROOT_PSID
#define IS_ROOT_PSID( id ) ( ProtoSpecs::IsRootPSpec( id ) == S_OK )
#endif
#ifndef IS_LLC_PSID
#define IS_LLC_PSID( id ) ( IS_ROOT_PSID( id ) || GET_PSID(id) == ProtoSpecDefs::SNAP || GET_PSID(id) == ProtoSpecDefs::LSAP )
#endif


#define	ieee802_3			1
#define	ethernet_type2		2
#define	localtalk			3
#define	token_ring			4
#define	ieee802_11			5
#define pspec_errors		6
#define wan_frmrly			7
#define wan_ppp				8
#define wan_x25				14
#define wan_x25e			15
#define wan_ipars			16
#define wan_u200			17
#define wan_Q931			18

#define	loopback			11
#define	spanning_tree		12
#define	cisco_discovery		13

#define pppoe				20
#define pppoe_discovery			1
#define pppoe_session			2

#define error_crc				50
#define error_frame				51
#define error_crc_frame			52
#define error_osize				53
#define error_crc_osize			54
#define error_frame_osize		55
#define error_crc_frame_osize	56
#define error_runt				57
#define error_crc_runt			58
#define error_frame_runt		59
#define error_crc_frame_runt	60

#define ctrl_80211			200
#define mgmt_80211			201
#define data_80211			202

#define ctrl_ps_poll_80211		210
#define ctrl_rts_80211			211
#define ctrl_cts_80211			212
#define ctrl_ack_80211			213
#define ctrl_cf_end_80211		214
#define ctrl_cf_end_cf_ack_80211 215
#define mgmt_areq_80211			220
#define mgmt_arsp_80211			221
#define mgmt_rreq_80211			222
#define mgmt_rrsp_80211			223
#define mgmt_preq_80211			224
#define mgmt_prsp_80211			225
#define mgmt_beacon_80211		226
#define mgmt_atim_80211			227
#define mgmt_dis_80211			228
#define mgmt_auth_80211			229
#define mgmt_deauth_80211		230
#define data_wep_80211			240

#define mac_8025			100
#define token_8025			150
#define lanmanage_8025		160

#define	appletalk_aarp		450
#define	aarp_request		451
#define	aarp_response		452
#define	aarp_probe			453

#define	ip_arp_etype2		500
#define	ip_rarp_etype2		510
#define	ip_arp_ieee			520
#define	ip_rarp_ieee		530

#define	arp_request				1
#define	arp_response			2
#define	rarp_request			3
#define	rarp_response			4

#define sna					1600

#define	appletalk_phase1	99
#define	appletalk_phase2	7100
#define	appletalk_short		7300
#define	appletalk_long		7500
#define	ip_etype2_appletalk	7700
#define	ipv6_etype2_appltlk	7800
#define	ip_ieee_appletalk	7900
#define	at_rtmp					1
#define	at_rtmp_data			2
#define	at_rtmp_req				3
#define	at_nbp					4
#define	at_nbp_req				5
#define	at_nbp_lkup				6
#define	at_nbp_rply				7
#define	at_nbp_freq				8
#define	at_atp					9
#define	at_atp_treq				10
#define	at_atp_trsp				11
#define	at_atp_trel				12
#define	at_pap_req				13
#define	at_pap_open				14
#define	at_pap_open_reply		15
#define	at_pap_send				16
#define	at_pap_data				17
#define	at_pap_tickle			18
#define	at_pap_close			19
#define	at_pap_close_reply		20
#define	at_pap_send_status		21
#define	at_pap_status			22
#define	at_asp_req				23
#define	at_asp_close			24
#define	at_asp_command			25
#define	at_asp_get_stat			26
#define	at_asp_open				27
#define	at_asp_tickle			28
#define	at_asp_write			29
#define	at_asp_write_cont		30
#define	at_asp_attn				31
#define	at_atp_gzl				32
#define	at_atp_glz				33
#define	at_atp_gmz				34
#define	at_atp_gz_reply			35
#define	at_aep					36
#define	at_aep_req				37
#define	at_aep_reply			38
#define	at_zip					39
#define	at_zip_query			40
#define	at_zip_reply			41
#define	at_zip_ext_reply		42
#define	at_zip_gni_req			43
#define	at_zip_gni_reply		44
#define	at_zip_notify			45
#define	at_adsp					46
#define	at_adsp_control			47
#define	at_adsp_data			48
#define	at_adsp_probe			49
#define	at_adsp_open_req		50
#define	at_adsp_open_ack		51
#define	at_adsp_open_reqack		52
#define	at_adsp_open_deny		53
#define	at_adsp_close_adv		54
#define	at_adsp_forward_reset	55
#define	at_adsp_forward_ack		56
#define	at_adsp_retran_advice	57
#define	at_snmp					58
#define	at_bp					59
#define	at_mac_ip				60
#define	at_zip_req				61
#define	at_zip_rply				62
#define	at_pap_rply				63
#define	at_asp_rply				64
#define	at_e_s_ddp				71
#define	at_e_l_ddp				72
#define	at_aurp					73
#define	at_rireq				74
#define	at_rirsp				75
#define	at_riack				76
#define	at_riupd				77
#define	at_rtrdwn				78
#define	at_znreq				79
#define	at_znrsp				80
#define	at_opnreq				81
#define	at_opnrsp				82
#define	at_tickle				83
#define	at_tcklack				84

#define	ip					1000
#define	ipv6				8300
#define	ip_ipv6				8900
#define	ip_icmp				100
#define	ip_icmpv6			120
#define	ip_icmp_echo_req		1
#define	ip_icmp_echo_reply		2
#define	ip_icmp_dst_un			3
#define	ip_icmp_tm_exc			4
#define	ip_icmp_redir			5
#define	ip_icmp_rtr_slct		6
#define	ip_icmp_rtr_ad			7
#define	ip_icmp_nhb_slct		8
#define	ip_icmp_nhb_ad			9
#define	ip_encap			140
#define	ip_igmp				150
#define ip_eigrp			160
#define	ip_eigrp_update			1
#define ip_eigrp_query			2
#define ip_eigrp_reply			3
#define ip_eigrp_hello			4
#define	ip_ospf				200
#define	ip_ospf_hello			1
#define	ip_ospf_d_dsc				2
#define	ip_ospf_req				3
#define	ip_ospf_lsu				4
#define	ip_ospf_lsa				5
#define	ip_pup				250
#define	ip_egp				300
#define	ip_igrp				350
#define	tcp					1400
#define	udp					1500
#define	ip_telnet				1
#define	ip_ftp					2
#define	ip_ntp					3
#define	ip_rip					4
#define	ip_pop3					5
#define	ip_nntp					6
#define	ip_ftp_ctl				7
#define	ip_ftp_data				8
#define	ip_smtp					9
#define	ip_cso					10
#define	ip_rlogin				11
#define	ip_rsh					12
#define	ip_dns					13
#define	ip_bootp				14
#define	ip_tftp					15
#define	ip_gopher				16
#define	ip_finger				17
#define	ip_http					18
#define	ip_snmp					19
#define	ip_snmp_trap			20
#define	ip_rpc_nfs				21
#define	ip_pop2					22
#define	ip_cayman_tunnel		23
#define	ip_cu_see				24
#define	ip_dhcp					25
#define	ip_smip					26
#define	ip_http_proxy			27
#define	ip_afp_tcp				28
#define	ip_netbios				29
#define	ip_nb_nam_svc			30
#define	ip_dudgram				31
#define	ip_dgdgram				32
#define	ip_bcstdgram			33
#define	ip_dgram_err			34
#define	ip_qry_req				35
#define	ip_p_qry_rsp			36
#define	ip_n_qry_rsp			37
#define	ip_sess_msg				38
#define	ip_sess_req				39
#define	ip_p_sess_rsp			40
#define	ip_n_sess_rsp			41
#define	ip_r_sess_rsp			42
#define	ip_sess_ka				43
#define	ip_sess_smb				44
#define	ip_dudgram_smb			45
#define	ip_dgdgram_smb			46
#define	ip_dantz				47
#define	ip_whois				48
#define ip_kerberos				49
#define ip_imap					50
#define ip_bgp					51
#define ip_imap3				52
#define ip_wins					53
#define ip_https				54
#define ip_timbuktu				55
#define ip_rpc					56
#define ip_lotusnotes			57
#define ip_hsrp					58
#define ip_ssh					59
#define ip_aol					60
#define ip_radius				61
#define ip_rtsp					62
#define ip_l2tp					63
#define ip_l2f					64
#define ip_filemaker			65
#define ip_h323_tsap			68
#define ip_msn_messenger		71

#define	nw_ethernet			3750
#define	nw_spx					1
#define	nw_rip					2
#define rip_request					35
#define rip_reply					36
#define	nw_sap					3
#define	sap_request					30
#define sap_reply					31
#define	nw_ncp					4
#define nw_ncp_connreq				10
#define nw_ncp_request				11
#define nw_ncp_reply				12
#define nw_ncp_destroy				13
#define nw_ncp_burst				14
#define nw_ncp_delay				15
#define nw_nlsp					5
#define nw_nlsp_hello				20
#define nw_nlsp_lsp					21
#define nw_nlsp_csnp				22
#define nw_nlsp_psnp				23
#define	nw_bios					6
#define	nw_diag					7
#define nw_serialize			8


#define	xns_etype2			4000
#define	xns_lsap_ieee		4250
#define	xns_rip					1
#define	xns_echo				2
#define	xns_error				3
#define	xns_pep					4
#define	xns_spp					5

#define	vines_etype2		4500
#define	vines_lsap_ieee		4600
#define	vines_snap_ieee		4700
#define	vines_echo				1
#define	vines_inet_ipc			2
#define	vines_inet_spp			3
#define	vines_inet_arp			4
#define	vines_inet_rtp			5
#define	vines_inet_icp			6

#define	dec_ethernet			5000
#define	dec_mop_dl				10
#define	dec_mop_rc				20
#define	dec_ph_4				30
#define	dec_ph_4_ctl_init		31
#define	dec_ph_4_ctl_vrfy		32
#define	dec_ph_4_ctl_helo		33
#define	dec_ph_4_ctl_l1_r		34
#define	dec_ph_4_ctl_l2_r		35
#define	dec_ph_4_ctl_rhlo		36
#define	dec_ph_4_ctl_ehlo		37
#define	dec_ph_4_dta_shrt		38
#define	dec_ph_4_dta_long		39
#define	dec_ph_4_nsp_data		40
#define	dec_ph_4_nsp_ack		41
#define	dec_ph_4_nsp_ctl		42
#define	dec_ph_4_nsp_noop		43
#define	dec_ph_4_nsp_cnin		44
#define	dec_ph_4_nsp_cncf		45
#define	dec_ph_4_nsp_dcin		46
#define	dec_ph_4_nsp_dccf		47
#define	dec_ph_4_nsp_rcni		48
#define	dec_lat					70
#define	dec_lat_run				71
#define	dec_lat_conn			72
#define	dec_lat_disc			73
#define	dec_lat_serv			74
#define	dec_diag				80
#define	dec_upt					85
#define	dec_ctl					88
#define	dec_lad					90

#define netbeui_ieee		6000
#define netbeui_addgname_q	6001
#define netbeui_addname_q	6002
#define netbeui_name_conf	6003
#define netbeui_stat_q		6004
#define netbeui_term_tr_r	6005
#define netbeui_datagram	6006
#define netbeui_datag_bcast	6007
#define netbeui_name_q		6008
#define netbeui_addname_rsp	6009
#define netbeui_name_recog	6010
#define netbeui_stat_rsp	6011
#define netbeui_term_tr_l	6012
#define netbeui_data_ack	6013
#define netbeui_data_1_mid	6014
#define netbeui_data_only_l	6015
#define netbeui_sess_conf	6016
#define netbeui_sess_end	6017
#define netbeui_sess_init	6018
#define netbeui_no_recv		6019
#define netbeui_recv_outs	6020
#define netbeui_recv_cont	6021
#define netbeui_sess_alive	6022
// smb in netbeui 4 differnt ways
#define netbeui_smb_dlast1	6100
#define netbeui_smb_dgram1	6200
#define netbeui_smb_dlast2	6300
#define netbeui_smb_dgram2	6400
// smb in netbios in ip 24 ways
//	  ip 4 different ways (next 4 #define's)
//	  tcp & udp each way (x2)
//	  3 different ip in netbios pkts (x3)
// we add ip_type which is 400 or 500 to each
// therefore these really start at 10000
//		ie 9600 + 400
// and go to 12399
//		ie 11400 + 500(udp) +
//			       400(dg_dgram) + (99)
#define smb_ip_etype2		 9600
#define smb_ipv6_etype2		10200
#define smb_ip_ipv6			10800
#define smb_ip_ieee			11400
#define	smb_sess_msg	  0
#define	smb_du_dgram	200
#define	smb_dg_dgram	400
// the different SMB types
#define smb_creat_dir			 1
#define smb_delete_dir			 2
#define smb_open_file			 3
#define smb_create_file			 4
#define smb_close_file			 5
#define smb_flush_file			 6
#define smb_delete_file			 7
#define smb_rename_file			 8
#define smb_get_file_attrib		 9
#define smb_set_file_attrib		10
#define smb_read_from_file		11
#define smb_write_to_file		12
#define smb_lock_byte_range		13
#define smb_unlock_byte_range	14
#define smb_create_temp_file	15
#define smb_make_new_file		16
#define smb_check_dir_path		17
#define smb_process_exit		18
#define smb_seek				19
#define smb_lock_read_data		20
#define smb_write_unlock_data	21
#define smb_read_blk_raw		22
#define smb_read_blk_mpx		23
#define smb_read_blk_2nd_rsp	24
#define smb_write_blk_raw		25
#define smb_write_blk_mpx		26
#define smb_write_blk_2nd_req	27
#define smb_write_cmp_rsp		28
#define smb_set_file_attr_xpand	29
#define smb_get_file_attr_xpand	30
#define smb_lock_byte_ranges_x	31
#define smb_xact_name_bytes_io	32
#define smb_xact_2nd			33
#define smb_IOCTL				34
#define smb_IOCTL_2nd			35
#define smb_copy				36
#define smb_move				37
#define smb_echo				38
#define smb_write_close			39
#define smb_open_x				40
#define smb_read_x				41
#define smb_write_x				42
#define smb_xact_func_io		43
#define smb_xact2_2nd			44
#define smb_find_close			45
#define smb_find_notify_close	46
#define smb_tree_conn			47
#define smb_tree_disc			48
#define smb_neg_proto			49
#define smb_sess_set_up_x		50
#define smb_user_logoff_x		51
#define smb_tree_connect_x		52
#define smb_get_svr_attr		53
#define smb_search_dir			54
#define smb_find_1st			55
#define smb_find_unique			56
#define smb_find_close2			57
#define smb_open_p_spool_file	58
#define smb_write_p_spool_file	59
#define smb_close_p_spool_file	60
#define smb_rtn_print_q			61
#define smb_send_single_blk_msg	62
#define smb_send_bcast_msg		63
#define smb_fwd_user_name		64
#define smb_cancel_fwd			65
#define smb_get_mach_name		66
#define smb_send_SOMB_msg		67
#define smb_send_EOMB_msg		68
#define smb_send_TOMB_msg		69

#define osi_ieee			8000
#define osi_clnp			8100
#define osi_clnp_x_data		8101
#define osi_clnp_x_data_ack	8102
#define osi_clnp_reject		8103
#define osi_clnp_data_ack	8104
#define osi_clnp_tpdu_error	8105
#define osi_clnp_disc_req	8106
#define osi_clnp_disc_conf	8107
#define osi_clnp_conn_conf	8108
#define osi_clnp_conn_req	8109
#define osi_clnp_data		8110
#define osi_clnp_tarp_pdu	8111
#define osi_clnp_tarp_req1	8112
#define osi_clnp_tarp_req2	8113
#define osi_clnp_tarp_resp	8114
#define osi_clnp_tarp_achg	8115
#define osi_clnp_tarp_reqt	8116
#define osi_esis			8150
#define osi_esis_eshello	8151
#define osi_esis_ishello	8152
#define osi_esis_redirect	8153
#define osi_isis			8200
#define osi_isis_l1_rhello	8201
#define osi_isis_l2_rhello	8202
#define osi_isis_pp_hello	8203
#define osi_isis_l1_lsp		8204
#define osi_isis_l2_lsp		8205
#define osi_isis_xid		8206
#define osi_isis_l1_csnp	8207
#define osi_isis_l2_csnp	8208
#define osi_isis_l1_psnp	8209
#define osi_isis_l2_psnp	8210

#define	no_color	 0
#define	color_1		 1
#define	color_2		 2
#define	color_3		 3
#define	color_4		 4
#define	color_5		 5
#define	color_6		 6
#define	color_7		 7
#define	color_8		 8
#define	color_9		 9
#define	color_10	10
#define	color_11	11
#define	color_12	12
#define	color_13	13
#define	color_14	14
#define	color_15	15
#define	color_16	16
#define	color_17	17
#define	color_18	18
#define	color_19	19
#define	color_20	20
#define	color_21	21
#define	color_22	22
#define	color_23	23


