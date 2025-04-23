"""AccessControlList class.
"""
#Copyright (c) WildPackets, Inc. 2013-2017. All rights reserved.

import sys
import xml.etree.ElementTree as ET

import omniscript

from omniid import OmniId

find_attribs = ['name', 'security_id', 'description']

class AccessControlUser(object):
    """An Access Control List User.
    """

    description = None
    """Description of the user."""

    name = None
    """User's account name."""

    security_id = None
    """User's Security Id."""

    #XML Tags
    _tag_description = "desc"
    _tag_name = "name"
    _tag_security_id = "sid"

    def __init__(self, name=None, security_id=None, description=None, node=None):
        self.name = name
        self.security_id = security_id
        self.description = description
        self._load(node)

    def _load(self, node):
        if node is not None:
            if AccessControlUser._tag_name in node.attrib:
                self.name = node.attrib[AccessControlUser._tag_name]
            if AccessControlUser._tag_description in node.attrib:
                self.description = node.attrib[AccessControlUser._tag_description]
            if AccessControlUser._tag_security_id in node.attrib:
                self.security_id = node.attrib[AccessControlUser._tag_security_id]

    def __repr__(self):
        return 'AccessControlUser: %s - %s' % (self.name, self.security_id)

    def _store(self, node):
        user = ET.SubElement(node, 'user')
        if self.name:
            user.attrib[AccessControlUser._tag_name] = self.name
        if self.description:
            user.attrib[AccessControlUser._tag_description] = self.description
        if self.name:
            user.attrib[AccessControlUser._tag_security_id] = self.security_id


class AccessControlPolicy(object):
    """An Access Control List Policy.
    """

    id = None
    """The id of the item as an OmniId.
    """

    name = None
    """The name/description of the item."""

    users = None
    """The list of 
    :class:`AccessControlUser <omniscript.accesscontrollist.AccessControlUser>`
    ."""

    #XML Tags
    _tag_id = "id"
    _tag_name = "name"
    _tag_policy = "policy"
    _tag_user = "user"
    _tag_users = "user-list"

    def __init__(self, node=None):
        self.id = AccessControlPolicy.id
        self.name = AccessControlPolicy.name
        self.users = []
        self._load(node)
   
    def __repr__(self):
        return 'AccessControlPolicy: %s' % (self.name)

    def _load(self, node):
        if node is not None:
            id_names = omniscript.get_id_class_names()
            if AccessControlPolicy._tag_id in node.attrib:
                self.id = OmniId(node.attrib[AccessControlPolicy._tag_id])
                self.name = id_names[self.id]
            user_list = node.find(AccessControlPolicy._tag_users)
            if user_list is not None:
                users = user_list.findall(AccessControlPolicy._tag_user)
                for user in users:
                    self.users.append(AccessControlUser(node=user))

    def _store(self, node):
        policy = ET.SubElement(node, AccessControlPolicy._tag_policy,
                               {AccessControlPolicy._tag_id:str(self.id)})
        if self.users:
            users = ET.SubElement(policy, AccessControlPolicy._tag_users)
            for user in self.users:
                user._store(users)

    def add_user(self, user, security_id=None, description=None):
        """Adds a 
        :class:`AccessControlUser <omniscript.accesscontrollist.AccessControlUser>`
        to the users list if the user is not already in the list.

        Parameter:
            user as a
            :class:`AccessControlUser <omniscript.accesscontrollist.AccessControlUser>`
            is added to the users list.
            As string a new
            :class:`AccessControlUser <omniscript.accesscontrollist.AccessControlUser>`
            is created and the 
        """
        if isinstance(user, AccessControlUser):
            if not self.find_user(user):
                self.users.append(user)
        elif isinstance(user, basestring):
            acu = AccessControlUser()
            acu.name = user
            acu.security_id = security_id
            acu.description = description
            if not self.find_user(acu):
                self.users.append(acu)
        else:
            raise OmniError("The user parameter must be an AccessControlUser or string.")

    def find_user(self, value, attrib=find_attribs[0]):
        """Finds an user by name, id or description. Returns None if not found."""
        if attrib not in find_attribs:
            return None
        if isinstance(value, AccessControlUser):
            return next((u for u in self.users if u.security_id == value.security_id), None)
        return next((u for u in self.users if getattr(u, attrib) == value), None)

    def remove_user(self, value, attrib=find_attribs[0]):
        """Remove a 
        :class:`AccessControlUser <omniscript.accesscontrollist.AccessControlUser>`
        to the users list.

        Parameter:
            user as a
            :class:`AccessControlUser <omniscript.accesscontrollist.AccessControlUser>`
            or string value of type 
            is removed from the users list.
        """
        if isinstance(value, AccessControlUser):
            self.users = [u for u in self.users if u.security_id != value.security_id]
        elif isinstance(user, basestring):
            self.users = [u for u in self.users if getattr(u, attrib) != value]
        else:
            raise OmniError("The user parameter must be an AccessControlUser or string.")


class AccessControlList(object):
    """The Access Control List of an OmniEngine.
    """

    engine = None
    """The engine that this Access Control List came from."""

    enabled = False
    """Is Access Control enabled on the OmniEngine?"""

    policies = None
    """The list of
    :class:`AccessControlPolicy <omniscript.accesscontrollist.AccessControlPolicy>`
    """

    #XML Tags
    _tag_enabled = "using-acl"
    _tag_policy = "policy"
    _tag_rights = "user-rights"
    _tag_settings = "acl-settings"

    def __init__(self, engine, node):
        self.engine = engine
        self.enabled = AccessControlList.enabled
        self.policies = []
        self._load(node)
   
    def __repr__(self):
        return 'AccessControlList: %s' % (str(self.engine))

    def _load(self, xml):
        if xml:
            root = ET.fromstring(xml)
            if root is not None:
                acl = root.find(AccessControlList._tag_settings)
                if acl is not None:
                    _enabled = acl.attrib[AccessControlList._tag_enabled]
                    if _enabled is not None:
                        self.enabled = not _enabled.lower() == 'false'
                    rights = acl.find(AccessControlList._tag_rights)
                    if rights is not None:
                        for pol in rights.findall(AccessControlList._tag_policy):
                            self.policies.append(AccessControlPolicy(pol))

    def find_policy(self, name):
        """Finds a policy by name."""
        return next((p for p in self.policies if p.name == name), None)

    def find_user(self, value, attrib=find_attribs[0]):
        """Finds an user by name, id or description. Returns None if not found."""
        if attrib not in find_attribs:
            return None
        for policy in self.policies:
            user = policy.find_user(value, attrib)
            if user:
                return user
        return None

    def store(self, node):
        settings = ET.SubElement(node, AccessControlList._tag_settings,
                                 {AccessControlList._tag_enabled:['false','true'][self.enabled]})
        if self.policies:
            rights = ET.SubElement(settings, AccessControlList._tag_rights)
            for policy in self.policies:
                policy._store(rights)
