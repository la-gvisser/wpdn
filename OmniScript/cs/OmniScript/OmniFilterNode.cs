// =============================================================================
// <copyright file="Adapter.cs" company="LiveAction, Inc.">
//  Copyright (c) 2018-2021 Savvius, Inc. All rights reserved.
//  Copyright (c) 2015-2018 Savvius, Inc. All rights reserved.
// </copyright>
// =============================================================================

namespace Savvius.Omni.OmniScript
{
    using System;
    using System.Xml.Linq;
    using System.Collections.Generic;

    public class OmniFilterNode
    {
        public static Dictionary<OmniId, String> ConsoleIdNameMap = new Dictionary<OmniId, String>()
        {
            {new OmniId("{B4298A64-5A40-4F5F-ABCD-B14BA0F8D9EB}"), ""},
            {new OmniId("{2D2D9B91-08BF-44CF-B240-F0BBADBF21B5}"), "AddressFilterNode"},
            {new OmniId("{8C7C9172-82B2-43DC-AAF1-41ED80CE828F}"), "ApplicationFilterNode"},
            {new OmniId("{11FC8E5E-B21E-446B-8024-39E41E6865E1}"), "BpfFilterNode"},
            {new OmniId("{6E8DAF74-AF0D-4CD3-865D-D559A5798C5B}"), "ChannelFilterNode"},
            {new OmniId("{EF8A52E6-233F-4337-AAED-B021CBD8E9E4}"), "CountryFilterNode"},
            {new OmniId("{D0BDFB3F-F72F-4AEF-8E17-B16D4D3543FF}"), "ErrorFilterNode"},
            {new OmniId("{CF190294-C869-4D67-93F2-9A53FDFAE77D}"), "LengthFilterNode"},
            {new OmniId("{D8B5CE02-90CA-48AC-8188-468AC293B9C6}"), ""},
            {new OmniId("{47D49D7C-8219-40D5-9E5D-8ADEAACC644D}"), "PatternFilterNode"},
            {new OmniId("{D0329C21-8B27-489F-84D7-C7B783634A6A}"), "PluginFilterNode"},
            {new OmniId("{297D404D-3610-4A18-95A2-22768B554BED}"), "PortFilterNode"},
            {new OmniId("{F4342DAD-4A56-4ABA-9436-6E3C30DAB1C8}"), "ProtocolFilterNode"},
            {new OmniId("{85F2216E-6E65-4AE9-B14B-CC8DF48CAE6A}"), "TimeRangeFilterNode"},
            {new OmniId("{838F0E57-0D9F-4095-9C12-F1040C84E428}"), "ValueFilterNode"},
            {new OmniId("{1999CC65-01DA-4256-81B4-C303BDE88BDA}"), "VlanFilterNode"},
            {new OmniId("{90BAE500-B97B-42B0-9886-0947F34001EF}"), "WANDirectionFilterNode"},
            {new OmniId("{899E11AD-1849-40BA-9454-9F03798B3A6C}"), "WirelessFilterNode"}
        };

        static public FilterNode ParseOmniFilter(XElement element)
        {
            String name = element.Name.ToString();
            XAttribute clsidAttribute = element.Attribute("clsid");
            OmniId clsid = (clsidAttribute != null)
                ? new OmniId(clsidAttribute.Value)
                : null;
            if (clsid == null) throw new OmniException("Failed to find filter node class id.");
            
            String className = OmniScript.ClassIdNames[clsid];
            Object obj = Activator.CreateInstance(Type.GetType("Savvius.Omni.OmniScript." + className));
            
            FilterNode root = (FilterNode)obj;
            if (root == null) throw new OmniException("Failed to create filter node.");
            
            XElement filterNode = element.Element("filternode");
            root.Load(filterNode);
            if (filterNode != null)
            {
                XElement andNode = filterNode.Element("andnode");
                if (andNode != null)
                {
                    root.AndNode = ParseOmniFilter(andNode);
                }
                XElement orNode = filterNode.Element("ornode");
                if (orNode != null)
                {
                    root.OrNode = ParseOmniFilter(orNode);
                }
            }
            return root;
        }

        static public FilterNode ParseConsoleFilter(XElement element)
        {
            String name = element.Name.ToString();
            XAttribute clsidAttribute = element.Attribute("clsid");
            OmniId clsid = (clsidAttribute != null)
                ? new OmniId(clsidAttribute.Value)
                : null;
            if (clsid == null) throw new OmniException("Failed to find filter node class id.");

            String className = ConsoleIdNameMap[clsid];
            Object obj = Activator.CreateInstance(Type.GetType("Savvius.Omni.OmniScript." + className));

            FilterNode root = (FilterNode)obj;
            if (root == null) throw new OmniException("Failed to create filter node.");

            XElement filterNode = element.Element("filternode");
            root.Load(filterNode);

            XElement andNode = element.Element("andnode");
            if (andNode != null)
            {
                root.AndNode = ParseConsoleFilter(andNode);
            }
            XElement orNode = element.Element("ornode");
            if (orNode != null)
            {
                root.OrNode = ParseConsoleFilter(orNode);
            }
            return root;
        }

        static public void StoreOmniFilter(XElement element, String operatation, FilterNode criteria)
        {
            String className = criteria.GetType().Name;
            OmniId id = OmniScript.ClassNameIds[className];
            if (id == null) throw new OmniException("Failed to find filter node class id.");

            XElement rootnode = new XElement(operatation, new XAttribute("clsid", id.ToString()));
            XElement filter_node = new XElement("filternode");
            criteria.Store(filter_node);
            if (criteria.AndNode != null)
            {
                StoreOmniFilter(filter_node, "andnode", criteria.AndNode);
            }
            if (criteria.OrNode != null)
            {
                StoreOmniFilter(filter_node, "ornode", criteria.OrNode);
            }
            rootnode.Add(filter_node);
            element.Add(rootnode);
        }
    }
}
