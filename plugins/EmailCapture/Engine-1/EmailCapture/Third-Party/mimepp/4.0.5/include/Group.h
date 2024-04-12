//=============================================================================
// Group.h
//
// Copyright (c) 1996-2004 Hunny Software, Inc
// All rights reserved.
//
// IN NO EVENT SHALL HUNNY SOFTWARE, INC BE LIABLE TO ANY PARTY FOR DIRECT,
// INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
// THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF HUNNY SOFTWARE,
// INC HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// HUNNY SOFTWARE, INC SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
// BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON
// AN "AS IS" BASIS, AND HUNNY SOFTWARE, INC HAS NO OBLIGATION TO PROVIDE
// MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
//
//=============================================================================

#ifndef DW_GROUP_H
#define DW_GROUP_H

/// %Class that represents an RFC&nbsp;2822 address group

class MIMEPP_API Group : public Address {

public:

    /// Default constructor
    Group();

    /// Copy constructor
    Group(const Group& other);

    /// Constructor that takes an initial string and parent node
    Group(const String& str, Node* parent=0);

    /// Destructor
    virtual ~Group();

    /// Assignment operator
    const Group& operator = (const Group& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the group name
    const String& groupName() const;

    /// Sets the group name
    void setGroupName(const String& name);

    /// Gets the mailbox list
    class MailboxList& mailboxList() const;

    /// Creates a new instance
    static Group* newGroup();

    /// Provides a class factory hook
    static Group* (*sNewGroup)();

private:

    /// Points to the <b>MailboxList</b> object.
    class MailboxList* mMailboxList;

    String mGroupName;

    void _setMailboxList(MailboxList* mboxList);
};

#endif
