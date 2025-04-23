import time
import omniscript

adapter_type_name = ["Unknown", "1 : NIC", "2 : File", "3: Plugin"]


def print_access_control_list(acl, tab=20):
    if acl is None:
        print 'No Access Control List'
        return
    fmt_s = '%%%ds: %%s' % tab
    fmt_s2 = '%%%ds: %%s' % (tab+4)
    print(str(acl))
    print(fmt_s % ('Enabled', acl.enabled))
    for policy in acl.policies:
        print(fmt_s % ('Policy', policy.name))
        for user in policy.users:
            print(fmt_s2 % ('User', user.name))
            if user.description:
                print(fmt_s2 % ('Description', user.description))
            if user.security_id:
                print(fmt_s2 % ('Security Id', user.security_id))
        print


def print_adapter(a, tab=20):
    if a is None:
        print 'No adapter'
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print('Adapter: %s' % a.name)
    print(fmt_s % ('Id', a.id))
    print(fmt_d % ('Features', a.features))
    print(fmt_s % ('Type', adapter_type_name[a.adapter_type]))
    print(fmt_s % ('Address', a.address))
    print(fmt_s % ('Description', a.description))
    print(fmt_s % ('Device Name', a.device_name))
    print(fmt_s % ('Interface Features', a.interface_features))
    print(fmt_s % ('Link Speed', a.link_speed))
    print(fmt_s % ('Media Type', omniscript.MEDIA_TYPE_NAMES[a.media_type]))
    print(fmt_s % ('Media Sub Type', omniscript.MEDIA_SUB_TYPE_NAMES[a.media_sub_type]))
    print(fmt_s % ('OmniPeek API', a.wildpackets_api))
    print


def print_adapter_list(al):
    if al:
        for a in al:
            print_adapter(a)
    print


def print_adapter_configuration(ac, tab=20):
    if ac is None:
        print 'No adapter configuration'
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print(fmt_s % ('Id', ac.id))
    print(fmt_s % ('Type', adapter_type_name[ac.adapter_type]))
    print(fmt_d % ('Link Speed', ac.link_speed))
    print(fmt_d % ('Default Link Speed', ac.default_link_speed))
    print(fmt_d % ('Ring Buffer Size', ac.ring_buffer_size))
    for id in ac.ids:
        print(fmt_s % ('Id', id))
    

def print_alarm(a, tab=20):
    if a is None:
        print('No alarm')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print('Alarm: %s' % a.name)
    print(fmt_s % ('Id', a.id))
    print(fmt_s % ('Created', a.created.ctime()))
    print(fmt_s % ('Modified', a.modified.ctime()))
    print(fmt_d % ('Track Type', a.track_type))
    print


def print_alarm_list(al):
    if al:
        for a in al:
            print_alarm(a)
    print


def print_analysis_module(am, tab=20):
    if am is None:
        print('No Analysis Module')
        return
    fmt_s = '%%%ds: %%s' % tab
    print('Analysis Module: %s' % am.name)
    print(fmt_s % ('Id', am.id))
    print(fmt_s % ('Version', am.version))
    print


def print_analysis_module_list(aml):
    if aml:
        for am in aml:
            print_analysis_module(am)
    print


def print_application_stat(a, tab=20):
    if a is None:
        print('No application')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print('Application: %s' % a.name)
    print(fmt_s % ('', a.first_time.ctime()))
    print(fmt_s % ('', a.last_time.ctime()))
    print(fmt_d % ('', a.packets))
    print(fmt_d % ('', a.bytes))
    print


def print_application_stats(al):
    if al:
        for a in al:
            print_application_stat(a)
    print


def print_audit_log_entry(e, tab=20, verbose=True):
    if e is None:
        print('No Audit Log Entry')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print(fmt_d % ('Index', e.index))
    if verbose:
        print(fmt_s % ('Client', e.client))
        print(fmt_s % ('User', e.user))
        print(fmt_s % ('Timestamp', e.timestamp.ctime()))
        print(fmt_s % ('Message', e.message))
        print(fmt_d % ('Result', e.result))
        print


def print_audit_log(al, tab=20):
    if al is None:
        print('No Audit Log')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print(str(al))
    print(fmt_d % ('Count', al.count))
    print(fmt_s % ('First', al.first.ctime()))
    print(fmt_s % ('Last', al.last.ctime()))
    for e in al.entries:
        print_audit_log_entry(e, (tab+4))


def print_audit_log_indexes(al, tab=20):
    if al is None:
        print('No Audit Log')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print(str(al))
    print(fmt_d % ('Count', al.count))
    print(fmt_s % ('First', al.first.ctime()))
    print(fmt_s % ('Last', al.last.ctime()))
    for e in al.entries:
        print_audit_log_entry(e, (tab+4), False)


def print_call_utilization(utilization, tab=20):
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print(fmt_d % ('Utilization Interval', utilization.interval))
    print(fmt_s % ('Start Time', utilization.start_time.ctime()))
    print(fmt_d % (' Sample Count', len(utilization.samples)))


def print_call_quality(quality, tab=20):
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print(fmt_d % ('Quality Interval', quality.interval))
    print(fmt_s % ('Start Time', quality.start_time.ctime()))
    print(fmt_d % ('Codec Count', len(quality.codecs)))


def print_call_stats(call, tab=20):
    if call is None:
        print('No call statistic')
        return
    fmt_d = '%%%ds: %%d' % tab
    print('All Calls:')
    print(fmt_d % ('Good', call.all_calls['Good']))
    print(fmt_d % ('Fair', call.all_calls['Fair']))
    print(fmt_d % ('Poor', call.all_calls['Poor']))
    print(fmt_d % ('Bad', call.all_calls['Bad']))
    print('Open Calls:')
    print(fmt_d % ('Good', call.open_calls['Good']))
    print(fmt_d % ('Fair', call.open_calls['Fair']))
    print(fmt_d % ('Poor', call.open_calls['Poor']))
    print(fmt_d % ('Bad', call.open_calls['Bad']))
    for utilization in call.utilizations:
        print_call_utilization(utilization)
    for quality in call.qualities:
        print_call_quality(quality)
    print


def print_capture(c, tab=20):
    if c is None:
        print('No capture')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print('Capture: %s' % c.name)
    print(fmt_s % ('Id', c.id))
    print(fmt_s % ('Status', c.status))
    print(fmt_s % ('Comment', c.comment))
    print(fmt_s % ('Creator', c.creator))
    print(fmt_s % ('Creator SID', c.creator_sid))
    print(fmt_s % ('Logged on User SID', c.logged_on_user_sid))
    print(fmt_s % ('Last Modified By', c.last_modified_by))
    print(fmt_s % ('Last Modification', c.last_modification))
    print(fmt_s % ('Adapter', c.adapter))
    print(fmt_d % ('Adapter Type', c.adapter_type))
    print(fmt_d % ('Link Speed', c.link_speed))
    print(fmt_d % ('Media Type', c.media_type))
    print(fmt_d % ('Media Sub Type', c.media_sub_type))
    print(fmt_d % ('Buffer Size', c.buffer_size))
    print(fmt_d % ('Buffer Available', c.buffer_available))
    print(fmt_d % ('Buffer Used', c.buffer_used))
    print(fmt_d % ('Filter Mode', c.filter_mode))
    print(fmt_d % ('Graphs Count', c.graphs_count))
    print(fmt_s % ('Plugin List', c.plugin_list))
    print(fmt_s % ('Start Time', c.start_time.ctime()))
    print(fmt_s % ('Stop Time', c.stop_time.ctime()))
    print(fmt_d % ('Duration', c.duration))
    print(fmt_d % ('Reset Count', c.reset_count))
    print(fmt_d % ('Packets Received', c.packets_received))
    print(fmt_d % ('Packets Filtered', c.packets_filtered))
    print(fmt_d % ('Packet Count', c.packet_count))
    print(fmt_d % ('Analysis Dropped Packets', c.analysis_dropped_packets))
    print(fmt_d % ('Duplicate Packets Discarded', c.duplicate_packets_discarded))
    print(fmt_d % ('Packets Analyzed', c.packets_analyzed))
    print(fmt_d % ('Packets Dropped', c.packets_dropped))
    print(fmt_d % ('First Packet', c.first_packet))
    print(fmt_d % ('Alarms Info', c.alarms_info))
    print(fmt_d % ('Alarms Minor', c.alarms_minor))
    print(fmt_d % ('Alarms Major', c.alarms_major))
    print(fmt_d % ('Alarms Severe', c.alarms_severe))
    print(fmt_d % ('Trigger Count', c.trigger_count))
    print(fmt_d % ('Trigger Duration', c.trigger_duration))
    print(fmt_s % ('Option Alarms', c.option_alarms))
    print(fmt_s % ('Option Expert', c.option_expert))
    print(fmt_s % ('Option Filters', c.option_filters))
    print(fmt_s % ('Option Graphs', c.option_graphs))
    print(fmt_s % ('Option Hidden', c.option_hidden))
    print(fmt_s % ('Option Indexing', c.option_indexing))
    print(fmt_s % ('Option Packet Buffer', c.option_packet_buffer))
    print(fmt_s % ('Option Timeline Stats', c.option_timeline_stats))
    print(fmt_s % ('Option Voice', c.option_voice))
    print(fmt_s % ('Option Web', c.option_web))
    if hasattr(c, 'option_spotlight_capture'):
        if c.option_spotlight_capture:
            print(('%%%ds: True' % tab) % 'Spotlight Capture')
    print_adapter(c.adapter)
    print_media_info(c.media_info)
    

def print_capture_list(cl):
    if cl:
        for c in cl:
            print_capture(c)
    print


def print_capture_session(cs, tab=20):
    if cs is None:
        print('No capture session')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print(str(cs))
    print(fmt_s % ('Name', cs.name))
    print(fmt_s % ('Session Id', cs.session_id))
    print(fmt_s % ('Adapter Name', cs.adapter_name))
    print(fmt_s % ('Adapter Address', cs.adapter_address))
    print(fmt_s % ('Capture Flags', cs.capture_flags))
    print(fmt_s % ('Capture Id', cs.capture_id))
    print(fmt_s % ('Capture Id Alt', cs.alt_capture_id))
    print(fmt_s % ('Capture State', cs.capture_state))
    print(fmt_s % ('Capture Type', cs.capture_type))
    print(fmt_s % ('Capture Units', cs.capture_units))
    print(fmt_s % ('Dropped Packets', cs.dropped_packet_count))
    print(fmt_s % ('Link Speed', cs.link_speed))
    print(fmt_s % ('Media Type', cs.media_type))
    print(fmt_s % ('Media SubType', cs.media_sub_type))
    print(fmt_s % ('Owner', cs.owner))
    print(fmt_s % ('Packet Count', cs.packet_count))
    print(fmt_s % ('Session Start', cs.session_start_time.ctime()))
    print(fmt_s % ('Start Time', cs.start_time.ctime()))
    print(fmt_s % ('Storage Units', cs.storage_units))
    print(fmt_s % ('Stop Time', cs.stop_time.ctime()))
    print(fmt_s % ('Total Bytes', cs.total_byte_count))
    print(fmt_s % ('Total Dropped', cs.total_dropped_packet_count))
    print(fmt_s % ('Total Packets', cs.total_packet_count))
    print


def print_capture_session_list(csl):
    if csl:
        for cs in csl:
            print_capture_session(cs)
    print


def string_adapter_settings(a):
    at = ['', 'NIC', 'File', 'Plugin']
    return '%s - %s Adapter' % (a.name, at[a.adapter_type])

def string_analysis_settings(a):
    s = []
    if a.option_alarms:
        s.append('Alarm')
    if a.option_analysis_modules:
        s.append('AnMods')
    if a.option_application:
        s.append('Apps')
    if a.option_compass:
        s.append('Compass')
    if a.option_country:
        s.append('Coutry')
    if a.option_error:
        s.append('Err')
    if a.option_expert:
        s.append('Expert')
    if a.option_network:
        s.append('Net')
    if a.option_size:
        s.append('Siz')
    if a.option_summary:
        s.append('Sum')
    if a.option_top_talker:
        s.append('TopTalk')
    if a.option_traffic:
        s.append('Traf')
    if a.option_voice_video:
        s.append('VoiceVid')
    return ' '.join(s) if s else 'None enabled'


def string_general_settings(g):
    s = []
    if g.option_file_age:
        s.append('Age')
    if g.option_continuous_capture:
        s.append('Continuous')
    if g.option_capture_to_disk:
        s.append('CTD')
    if g.option_deduplicate:
        s.append('DeDup')
    if g.option_priority_ctd:
        s.append('PriCTD')
    if g.option_slicing:
        s.append('Slice: %d' % (g.slice_length))
    if g.option_start_capture:
        s.append('Start')
    if g.tap_timestamps > 0:
        tt = ['Default', 'Apcon', 'Anue', 'NetOptics', 'Gigamon']
        s.append('Timestamps:%s' % (tt[g.tap_timestamps]))
    if g.option_timeline_stats:
        tl = []
        if g.option_timeline_app_stats:
            tl.append('App')
        if g.option_timeline_top_stats:
            tl.append('TopTalk')
        if g.option_timeline_voip_stats:
            tl.append('VoIP')
        s.append('TL:%s' % (','.join(tl)))
    return ' '.join(s)


def string_indexing_settings(i):
    s = []
    if i.option_application:
        s.append('App')
    if i.option_country:
        s.append('Country')
    if i.option_ethernet:
        s.append('Eth')
    if i.option_ipv4:
        s.append('IPv4')
    if i.option_ipv6:
        s.append('IPv6')
    if i.option_mpls:
        s.append('MPLS')
    if i.option_port:
        s.append('Port')
    if i.option_protospec:
        s.append('ProtoSpec')
    if i.option_vlan:
        s.append('VLAN')
    return ' '.join(s) if s else 'None enabled'


def string_voip_settings(v):
    return 'Max Calls: %d, Notify: %s, Severity: %d, Stop Analysis: %s' % \
        (v.max_calls, v.option_notify, v.severity, v.option_stop_analysis)


def print_capture_template(ct, tab=20):
    if ct is None:
        print('No capture template')
        return
    if not isinstance(ct, omniscript.CaptureTemplate):
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print(str(ct))
    print(fmt_s % ('Adapter', string_adapter_settings(ct.adapter)))
    print(fmt_d % ('Alarms', len(ct.alarms.alarms) if hasattr(ct, 'alarms') else 0))
    print(fmt_d % ('Analysis Modules', len(ct.plugins.modules) if hasattr(ct, 'plugins') else 0))
    print(fmt_s % ('Analysis Settings', string_analysis_settings(ct.analysis) if hasattr(ct, 'analysis') else 'None enabled'))
    print(fmt_s % ('Filter', 'Yes' if ct.filter else 'None'))
    print(fmt_s % ('General', string_general_settings(ct.general)))
    print(fmt_d % ('Graphs', len(ct.graphs.graphs)))
    print(fmt_s % ('Indexing', string_indexing_settings(ct.indexing)))
    print(fmt_d % ('Plugins Config', len(ct.plugins_config) if ct.plugins_config else 0))
    print(fmt_s % ('Repeat Trigger', 'None'))
    print(fmt_s % ('Start Trigger', ct.start_trigger.enabled))
    print(fmt_s % ('Statistics', ct.adapter.name))
    print(fmt_s % ('Stop Trigger', ct.statistics_output.enabled))
    print(fmt_s % ('VoIP', string_voip_settings(ct.voip)))
    if ct.general.option_spotlight_capture:
        print(fmt_s % ('Spotlight Capture', 'Enabled'))


def print_capture_template_list(ctl):
    if ctl:
        for ct in ctl:
            print_capture_template(ct)
    print


def print_channel_scan_entry(e, tab=20):
    if e is None:
        print 'No hardware options'
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print(fmt_d % ('Channel Number', e.channel_number))
    print(fmt_d % ('Channel Frequency', e.channel_frequency))
    print(fmt_d % ('Channel Band', e.channel_band))
    print(fmt_d % ('Duration (ms)', e.duration))
    print(fmt_s % ('Enabled', e.enabled))
    print


def print_country_stat(cs, tab=20):
    if cs is None:
        print('No country')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print('Country Code: %s' % (cs.country_code))
    print(fmt_s % ('Country Name', cs.country_name))
    print(fmt_s % ('First From', cs.first_time_from.ctime()))
    print(fmt_s % ('Last From', cs.last_time_from.ctime()))
    print(fmt_d % ('Packets From', cs.packets_from))
    print(fmt_d % ('Bytes From', cs.bytes_from))
    print(fmt_s % ('First To', cs.first_time_to.ctime()))
    print(fmt_s % ('Last To', cs.last_time_to.ctime()))
    print(fmt_d % ('Packets To', cs.packets_to))
    print(fmt_d % ('Bytes To', cs.bytes_to))
    print


def print_country_stats(csl):
    if csl is None:
        print('No list of country stats')
        return
    for cs in csl:
        print_country_stat(cs)
    print


def print_engine_status(status, tab=20):
    if status is None:
        print('No engine status')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    fmt_sd = '%%%ds: %%s, %%s: %%d' % tab
    print('Engine Status: %s' % status.name)
    print(fmt_d % ('Adapter Count', status.adapter_count))
    print(fmt_d % ('Alarm Count', status.alarm_count))
    print(fmt_d % ('Capture Count', status.capture_count))
    print(fmt_d % ('Capture Session Count', status.capture_session_count))
    print(fmt_d % ('CPU Count', status.cpu_count))
    print(fmt_s % ('CPU Type', status.cpu_type))
    print(fmt_s % ('Data Folder', status.data_folder))
    print(fmt_s % ('Engine Type', status.engine_type))
    print(fmt_d % ('File Count', status.file_count))
    print(fmt_s % ('File Version', status.file_version))
    print(fmt_d % ('Filter Count', status.filter_count))
    print(fmt_s % ('Filter Modification Time', status.filters_modification_time.ctime()))
    print(fmt_d % ('Forensic Search Count', status.forensic_search_count))
    print(fmt_d % ('Graph Count', status.graph_count))
    print(fmt_sd % ('Host', status.host, 'Port', status.port))
    print(fmt_d % ('Log Total Count', status.log_total_count))
    print(fmt_d % ('Physical Memory Available', status.memory_available_physical))
    print(fmt_d % ('Physical Memory Total', status.memory_total_physical))
    print(fmt_d % ('Name Table Count', status.name_table_count))
    print(fmt_d % ('Notification Count', status.notification_count))
    print(fmt_s % ('Operating System', status.operating_system))
    print(fmt_s % ('OS', status.os))
    print(fmt_s % ('Platform', status.platform))
    print(fmt_s % ('Product Version', status.product_version))
    print(fmt_s % ('Free Storage Space', ''))
    print(fmt_d % ('Total Storage Space', status.storage_total))
    print(fmt_d % ('Storage Space Used', status.storage_used))
    print(fmt_d % ('Storage Space Available', status.storage_available))
    print(fmt_s % ('Time', status.time.ctime()))
    print(fmt_d % ('Time Zone Bias', status.time_zone_bias))
    print(fmt_d % ('Uptime', status.uptime))
    print


def print_event_log_entry(e, tab=20, verbose=True):
    if e is None:
        print('No Event Log Entry')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print(fmt_d % ('Index', e.index))
    if verbose:
        print(fmt_s % ('Message', e.message))
        print(fmt_d % ('Severity', e.severity))
        print(fmt_s % ('Timestamp', e.timestamp.ctime()))
        if e.capture_id:
            print(fmt_s % ('Capture Id', str(e.capture_id)))
        if e.source_id:
            print(fmt_s % ('Source Id', str(e.source_id)))
        if e.source_key:
            print(fmt_d % ('Source Key', str(e.source_key)))
        print


def print_event_log(al, tab=20):
    if al is None:
        print('No Audit Log')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print(str(al))
    print(fmt_d % ('Count', al.count))
    print(fmt_d % ('Informational', al.informational))
    print(fmt_d % ('Minor', al.minor))
    print(fmt_d % ('Major', al.major))
    print(fmt_d % ('Severe', al.severe))
    print(fmt_s % ('First', al.first.ctime()))
    print(fmt_s % ('Last', al.last.ctime()))
    for e in al.entries:
        print_event_log_entry(e, (tab+4))


def print_event_log_indexes(al, tab=20):
    if al is None:
        print('No Audit Log')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print(str(al))
    print(fmt_d % ('Count', al.count))
    print(fmt_d % ('Informational', al.informational))
    print(fmt_d % ('Minor', al.minor))
    print(fmt_d % ('Major', al.major))
    print(fmt_d % ('Severe', al.severe))
    print(fmt_s % ('First', al.first.ctime()))
    print(fmt_s % ('Last', al.last.ctime()))
    for e in al.entries:
        print_event_log_entry(e, (tab+4), False)


def print_expert_result(er, tab=20):
    if er is None:
        print('No expert result')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print('Expert Result: %s' % er.table)
    print(fmt_d % ('Rows', len(er.rows)))
    print(fmt_d % ('First Index', er.first_index))
    print(fmt_s % ('Time', er.time.ctime()))
    cols = min(len(er.columns), 4)
    print(fmt_s % ('', '\t'.join(er.columns[:cols])))
    for row in er.rows:
        print(fmt_s % ('', ' '.join(str(i[1]) for i in row.items()[:cols])))


def print_expert_result_list(erl):
    if erl:
        for er in erl:
            print_expert_result(er)
    print


def print_file_adapter(fa, tab=20):
    if fa is None:
        print('No file adapter')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print('File Adapter: %s' % fa.filename)
    print(fmt_s % ('Limit', fa.limit))
    print(fmt_d % ('Mode', fa.mode))
    print(fmt_d % ('Speed', fa.speed))


def print_filter(f, tab=20):
    if f is None:
        print('No filter')
        return
    fmt_s = '%%%ds: %%s' % tab
    print('Filter: %s' % f.name)
    print(fmt_s % ('Id', f.id))
    print(fmt_s % ('Comment', f.comment))
    print(fmt_s % ('Created', f.created.ctime() if f.created else ''))
    print(fmt_s % ('Modified', f.modified.ctime() if f.modified else ''))
    print(fmt_s % ('Criteria', str(f.criteria)))


def print_filter_list(fl):
    if fl:
        for f in fl:
            print_filter(f)
    print


def print_forensic_file(ff, tab=20):
    if ff is None:
        print('No forensic file')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_dd = '%%%ds: %%d, %%s: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    fmt_ss = '%%%ds: %%s, %%s: %%s' % tab
    print('Forensic File: %s' % ff.name)
    print(fmt_s % ('Path', ff.path))
    print(fmt_d % ('Status', ff.status))
    print(fmt_d % ('Index', ff.file_index))
    print(fmt_s % ('Session Id', ff.session_id))
    print(fmt_d % ('Size', ff.size))
    print(fmt_dd % ('Media Type', ff.media_type, 'Sub Type', ff.media_sub_type))
    print(fmt_ss % ('Adapter', ff.adapter_name, 'Address', ff.adapter_address))
    print(fmt_d % ('Link Speed', ff.link_speed))
    print(fmt_s % ('Capture', ff.capture_name))
    print(fmt_s % ('Capture Id', ff.capture_id))
    print(fmt_d % ('Count', ff.packet_count))
    print(fmt_d % ('Dropped', ff.dropped_packet_count))
    print(fmt_s % ('Start', ff.start_time.ctime()))
    print(fmt_s % ('End', ff.end_time.ctime() if ff.end_time else '- - -'))
    print(fmt_d % ('Timezone Bias', ff.time_zone_bias))
    print


def print_forensic_file_list(ffl):
    if ffl:
        for ff in ffl:
            print_forensic_file(ff)
    print


def print_forensic_search(fs, tab=20):
    if fs is None:
        print('No forensic search')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_dd = '%%%ds: %%d, %%s: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print('Forensic Search: %s' % fs.name)
    print(fmt_s % ('Id', fs.id))
    print(fmt_s % ('Adapter', fs.adapter))
    print(fmt_s % ('Capture Name', fs.capture_name))
    print(fmt_s % ('Session Id', fs.session_id))
    print(fmt_s % ('Creator', fs.creator))
    print(fmt_s % ('Creator SID', fs.creator_sid))
    print(fmt_d % ('Duration', fs.duration))
    print(fmt_d % ('Status', fs.status))
    print(fmt_d % ('First Packet', fs.first_packet))
    print(fmt_d % ('Link Speed', fs.link_speed))
    print(fmt_s % ('Load Progress', fs.load_progress))
    print(fmt_s % ('Load Percent', fs.load_percent))
    print(fmt_dd % ('Media Info: Type:', fs.media_info.media_type, 'Subtype', fs.media_info.media_sub_type))
    print(fmt_dd % ('            Domain:', fs.media_info.domain, 'Link Speed',fs.media_info.link_speed))
    print(fmt_d % ('Media Type', fs.media_type))
    print(fmt_d % ('Media Sub Type', fs.media_sub_type))
    print(fmt_s % ('Modified By', fs.modified_by))
    print(fmt_s % ('Modification Type', fs.modification_type))
    print(fmt_d % ('Open Result', fs.open_result))
    print(fmt_d % ('Packet Count', fs.packet_count))
    print(fmt_d % ('Percent Progress', fs.percent_progress))
    print(fmt_d % ('Process % Progress', fs.process_percent_progress))
    print(fmt_s % ('Process Progress', fs.process_progress))
    print(fmt_s % ('Progress', fs.progress))
    print(fmt_s % ('Start Time', fs.start_time.ctime()))
    print(fmt_s % ('Stop Time', fs.stop_time.ctime()))
    print(fmt_s % ('Option Expert', fs.option_expert))
    print(fmt_s % ('Option Graphs', fs.option_graphs))
    print(fmt_s % ('Option Indexing', fs.option_indexing))
    print(fmt_s % ('Option Log', fs.option_log))
    print(fmt_s % ('Option Packet Buffer', fs.option_packet_buffer))
    print(fmt_s % ('Option Voice', fs.option_voice))
    print(fmt_s % ('Option Web', fs.option_web))
    print


def print_forensic_search_list(fsl):
    if fsl:
        for fs in fsl:
            print_forensic_search(fs)
    print


def print_forensic_template(ft, tab=25):
    if ft is None:
        print('No forensic template')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_dd = '%%%ds: %%d, %%s: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print('Forensic Template: %s' % ft.name)
    print(fmt_s % ('Id', ft.session_id))
    print(fmt_s % ('Adapter', ft.adapter_name))
    print(fmt_s % ('Capture', ft.capture_name))
    print(fmt_s % ('End Time', ft.end_time.ctime()))
    print(fmt_s % ('File Name', ft.filename))
    print(fmt_d % ('Filter Mode', ft.filter_mode))
    print(fmt_d % ('Graph Interval', ft.graph_interval))
    print(fmt_d % ('Limit', ft.limit))
    print(fmt_d % ('Limit Size', ft.limit_size))
    print(fmt_d % ('Media Type', ft.media_type))
    print(fmt_d % ('Media Sub Type', ft.media_sub_type))
    print(fmt_s % ('Start Time', ft.start_time.ctime()))
    print(fmt_s % ('Option Error', ft.option_error))
    print(fmt_s % ('Option Expert', ft.option_expert))
    print(fmt_s % ('Option Graphs', ft.option_graphs))
    print(fmt_s % ('Option Histort', ft.option_history))
    print(fmt_s % ('Option Log', ft.option_log))
    print(fmt_s % ('Option Network', ft.option_network))
    print(fmt_s % ('Option Packets', ft.option_packets))
    print(fmt_s % ('Option Plugins', ft.option_plugins))
    print(fmt_s % ('Option Size', ft.option_size))
    print(fmt_s % ('Option Summary', ft.option_summary))
    print(fmt_s % ('Option Top Talkers', ft.option_top_talkers))
    print(fmt_s % ('Option Voice', ft.option_voice))
    print(fmt_s % ('Option Web', ft.option_web))
    print(fmt_s % ('Option Wireless Channel', ft.option_wireless_channel))
    print(fmt_s % ('Option Wireless Node', ft.option_wireless_node))
    if ft.node_limits is not None:
        print(fmt_s % ('Option Node Limits', ft.is_node_limits_enabled()))
    if ft.protocol_limits is not None:
        print(fmt_s % ('Option Protocol Limits', ft.is_protocol_limits_enabled()))
    if ft.is_node_protocol_detail_limits_enabled():
        print(fmt_s % ('Option Node/Protocol Detail Limits',
                       ft.is_node_protocol_detail_limits_enabled()))
    if ft.files is not None:
        for f in ft.files:
            print(fmt_s % ('File', f))
    if ft.filter:
        print(ft.filter.to_string(0))


def print_forensic_template_list(ftl):
    if ftl:
        for ft in ftl:
            print_forensic_template(ft)
    print


def print_graph_template(gt, tab=20):
    if gt is None:
        print('No graph template')
        return
    fmt_s = '%%%ds: %%s' % tab
    print('Graph Template: %s' % gt.name)
    print(fmt_s % ('Id', gt.id))
    print(fmt_s % ('Graph Id', gt.graph_id))
    print


def print_graph_template_list(gtl):
    if gtl:
        for gt in gtl:
            print_graph_template(gt)
    print


def print_wireless_hardware_options(w, scan=True, tab=20):
    if w is None:
        print 'No hardware options'
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print(fmt_d % ('Channel Number', w.channel_number))
    print(fmt_d % ('Channel Frequency', w.channel_frequency))
    print(fmt_d % ('Channel Band', w.channel_band))
    print(fmt_s % ('ESSID', w.essid))
    print(fmt_s % ('BSSID', w.bssid))
    if w.key_set:
        ks = w.key_set
        if ks.name:
            print(fmt_s % ('Name', ks.name))
        print(fmt_d % ('Algorithm', ks.algorithm))
        for k in ks.keys:
            print(fmt_s % ('Key', k[1]))
    if scan and w.channel_scanning:
        print(fmt_d % ('Channel Scanning Entries: ', len(w.channel_scanning)))
        for e in w.channel_scanning:
            print_channel_scan_entry(e, tab)
    print


def print_hardware_options(hwo, scan=True, tab=20):
    if hwo is None:
        print 'No hardware options'
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print('Hardware Options: %s' % hwo.name)
    print(fmt_s % ('Id', hwo.id))
    print(fmt_s % ('Comment', hwo.comment))
    print(fmt_d % ('Color', hwo.color))
    print(fmt_s % ('Created', hwo.created.ctime()))
    print(fmt_s % ('Modified', hwo.modified.ctime()))
    if isinstance(hwo, omniscript.hardwareoptions.WirelessHardwareOptions):
        print_wireless_hardware_options(hwo, scan, tab)
    print


def print_hardware_options_list(hwol, scan=True):
    if hwol:
        for hwo in hwol:
            print_hardware_options(hwo, scan)
    print


def print_media_info(mi, tab=20):
    if mi is None:
        print(('No media information'))
        return
    fmt_d = '%%%ds: %%d' % tab
    print(fmt_d % ('Media Type', mi.media_type))
    print(fmt_d % ('Media Subtype', mi.media_sub_type))
    print(fmt_d % ('Domain', mi.domain))
    print(fmt_d % ('Link Speed', mi.link_speed))
    print


def print_node_stat(stat, total_packets=0, total_bytes=0, tab=20):
    fmt_d = '%%%ds: %%d' % tab
    fmt_f = '%%%ds: %%.4f%%%%' % tab
    fmt_s = '%%%ds: %%s' % tab
    if stat is None:
        print('No node statistics')
        return
    print('Node Statistic: %s:' % stat.node)
    print(fmt_d % ('Bytes Received', stat.bytes_received))
    print(fmt_d % ('Packets Received', stat.packets_received))
    if stat.did_receive():
        print(fmt_d % ('Max Packet Size Received', stat.max_packet_size_received))
        print(fmt_d % ('Min Packet Size Received', stat.min_packet_size_received))
        print(fmt_s % ('First Time Received', stat.first_time_received.ctime()))
        print(fmt_s % ('Last Time Received', stat.last_time_received.ctime()))

    print(fmt_d % ('Bytes Sent', stat.bytes_sent))
    print(fmt_d  % ('Packets Sent', stat.packets_sent))
    if stat.did_send():
        print(fmt_d % ('Max Packet Size Sent', stat.max_packet_size_sent))
        print(fmt_d % ('Min Packet Size Sent', stat.min_packet_size_sent))
        print(fmt_s % ('First Time Sent', stat.first_time_sent.ctime()))
        print(fmt_s % ('Last Time Sent', stat.last_time_sent.ctime()))
        print(fmt_d % ('Broadcast Bytes', stat.broadcast_bytes))
        print(fmt_d % ('Broadcast Packets', stat.broadcast_packets))
        print(fmt_d % ('Multicast Bytes', stat.multicast_bytes))
        print(fmt_d % ('Multicast Packet', stat.multicast_packets))
        print(fmt_d % ('Broadcast/Multicast Bytes', stat.broadcast_multicast_bytes))
        print(fmt_d % ('Broadcast/Multicast Packets', stat.broadcast_multicast_packets))
    print(fmt_f % ('Percentage Bytes', (stat.percentage_bytes(total_bytes) * 100)))
    print(fmt_f % ('Percentage Packets', (stat.percentage_packets(total_packets) * 100)))
#    print('Total Packets: %d' % stat.total_packets)
    print


def print_node_stats(nodes, total_packets=0, total_bytes=0):
    if nodes:
        for node in nodes:
            print_node_stat(node, total_packets, total_bytes)
    print
    

def print_packet(p, tab=20):
    if p is None:
        print('No packet')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print('Packet Number: %d' % (p.number))
    print(fmt_d % ('Index', p.index))
    print(fmt_s % ('Time Stamp', p.timestamp.ctime()))
    print(fmt_d % ('ProtoSpec', p.proto_spec))
    print(fmt_s % ('ProtoSpec Name', p.protocol_name()))
    print(fmt_s % ('Application', p.application))
    print(fmt_s % ('Flags', hex(p.flags)))
    print(fmt_d % ('Flow Id', p.flow_id))
    print(fmt_s % ('Status', hex(p.status)))
    print(fmt_d % ('Length', p.packet_length))
    print


def print_packet_list(pl):
    if pl:
        for p in pl:
            print_packet(p)
    print


def print_protocol_stat(stat, tab=20):
    if stat is None:
        print('No protocol statistics')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_f = '%%%ds: %%.4f%%%%' % tab
    fmt_s = '%%%ds: %%s' % tab
    print('Protocol Name: %s' % stat.protocol)
    print(fmt_d % ('Bytes', stat.bytes))
    print(fmt_d % ('Packets', stat.packets))
    print(fmt_s % ('First Time', stat.first_time.ctime()))
    print(fmt_s % ('Last Time', stat.last_time.ctime()))
    print(fmt_f % ('Percentage Bytes', (stat.percentage_bytes * 100)))
    print(fmt_f % ('Percentage Packets', (stat.percentage_packets * 100)))
    print(fmt_f % ('Utilization', (stat.utilization * 100)))
    print


def print_protocol_stats(protocols):
    if not protocols:
        print('No protocol stats')
        return
    for protocol in protocols:
        print_protocol_stat(protocol)
    print
    

def print_stats_context(context, tab=20):
    if not context:
        print('No statistics context')
        return
    fmt_d = '%%%ds: %%d' % tab
    fmt_s = '%%%ds: %%s' % tab
    print('Statistics Context')
    print(fmt_s % ('Request Time', context.request_time.ctime()))
    print(fmt_d % ('Media Type', context.media_type))
    print(fmt_d % ('Media Subtype', context.media_sub_type))
    print(fmt_d % ('Link Speed', context.link_speed))
    print(fmt_s % ('Start Time', context.start_time.ctime()))
    print(fmt_s % ('End Time', context.end_time.ctime()))
    print(fmt_d % ('Total Packets', context.total_packets))
    print(fmt_d % ('Total Bytes', context.total_bytes))


def print_summary_stat(stat):
    if stat is None:
        print('No summary statistic')
        return
    print('%s = %s' % (stat.name, stat.value))


def print_summary_group(name, group):
    if isinstance(group, dict):
        print("Group: %s" % name)
        for key, stat in group.iteritems():
            print_summary_stat(stat)
    print


def print_summary_stats(summarys):
    if isinstance(summarys, dict):
        for name, group in summarys.iteritems():
            print_summary_group(name, group)
    elif isinstance(summarys, list):
        for summary in summarys:
            print_summary_stat(summary)
    else:
        print('No summary statistics')
    print
