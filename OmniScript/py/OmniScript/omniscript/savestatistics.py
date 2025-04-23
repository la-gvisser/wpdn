"""savestatistics module.
"""
#Copyright (c) WildPackets, Inc. 2013-2014. All rights reserved.

import sys
import time

import omniscript
import config

from nodestatistic import NodeStatistic
from protocolstatistic import ProtocolStatistic
from summarystatistic import SummaryStatistic

from protocolstatistic import flatten


node_columns = [
    config.NODE_COLUMN_BYTES_SENT,
    config.NODE_COLUMN_BYTES_RECEIVED,
    config.NODE_COLUMN_PACKETS_SENT,
    config.NODE_COLUMN_PACKETS_RECEIVED,
    config.NODE_COLUMN_BROADCAST_PACKETS,
    config.NODE_COLUMN_BROADCAST_BYTES,
    config.NODE_COLUMN_MULTICAST_PACKETS,
    config.NODE_COLUMN_MULTICAST_BYTES,
    config.NODE_COLUMN_MIN_SIZE_SENT,
    config.NODE_COLUMN_MAX_SIZE_SENT,
    config.NODE_COLUMN_MIN_SIZE_RECEIVED,
    config.NODE_COLUMN_MAX_SIZE_RECEIVED,
    config.NODE_COLUMN_FIRST_TIME_SENT,
    config.NODE_COLUMN_LAST_TIME_SENT,
    config.NODE_COLUMN_FIRST_TIME_RECEIVED,
    config.NODE_COLUMN_LAST_TIME_RECEIVED
    ]

def save_nodestats(capture, filename='NodeStatistics.txt'):
    """Creates a Node Statistics report.

    Args:
        capture (Capture object): the capture to create the report from. 
        
        filename (str): Fully-qualified filename. Defaults to
        NodeStatistics.txt in the current directory.
    
    Notes:
        Creates the file 'filename' with the contents of the report.
    """
    nodes = capture.get_node_stats()
    with open (filename, "w") as report: 
        report.write(NodeStatistic.report_header(node_columns))
        for n in nodes:
            report.write(n.report(node_columns))


def save_protocolstats(capture, filename='ProtocolStatistics.txt'):
    """Creates a Protocol Statistics report.

    Args:
        capture (Capture object): the capture to create the report from. 
        
        filename (str): Fully-qualified filename. Defaults to
        ProtocolStatistics.txt in the current directory.
    
    Notes:
        Creates the file 'filename' with the contents of the report.
    """
    protocols = capture.get_protocol_stats()
    with open (filename, "w") as report:
        report.write(ProtocolStatistic.report_header())
        p_flat = flatten(protocols, True)
        for p in p_flat:
            report.write(p.report())


def save_summarystats(capture, filename='SummaryStatistics.txt'):
    """Creates a Summary Statistics report.

    Args:
        capture (Capture object): the capture to create the report from. 
        
        filename (str): Fully-qualified filename. Defaults to
        SummaryStatistics.txt in the current directory.
    
    Notes:
        Creates the file 'filename' with the contents of the report.
    """
    sums = capture.get_summary_stats()
    groups = list(g for g, gg in sums.iteritems())
    groups.sort()
    with open (filename, "w") as report: 
        report.write(SummaryStatistic.report_header())
        for g in groups:
            for _, v in sums[g].iteritems():
                report.write(v.report())
