//=============================================================================
// MailboxList.h
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

#ifndef MIMEPP_MAILBOX_LIST_H
#define MIMEPP_MAILBOX_LIST_H

/// %Class that represents a list of RFC&nbsp;2822 mailboxes

class MIMEPP_API MailboxList : public FieldBody {

public:

    /// Default constructor
    MailboxList();

    /// Copy constructor
    MailboxList(const MailboxList& other);

    /// Constructor that takes an initial string and parent node
    MailboxList(const String& str, Node* parent=0);

    /// Destructor
    virtual ~MailboxList();

    /// Assignment operator
    const MailboxList& operator = (const MailboxList& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the number of mailboxes in the list
    int numMailboxes() const;

    /// Adds a mailbox to the end of the list
    void addMailbox(Mailbox* mailbox);

    /// Deletes all mailboxes in the list
    void deleteAllMailboxes();

    /// Gets the mailbox at the specified position in the list
    Mailbox& mailboxAt(int index) const;

    /// Inserts a mailbox at the specified position in the list
    void insertMailboxAt(int index, Mailbox* mailbox);

    /// Removes the mailbox at the specified position in the list
    Mailbox* removeMailboxAt(int index);

    /// Creates a new instance
    static MailboxList* newMailboxList();

    /// Provides a class factory hook
    static MailboxList* (*sNewMailboxList)();

protected:

    /// Copies all mailboxes from another list
    void _copyMailboxes(const MailboxList& list);

    /// Adds a mailbox to the end of the list
    void _addMailbox(Mailbox* mailbox);

    /// Inserts a mailbox at the specified position in the list
    void _insertMailboxAt(int index, Mailbox* mailbox);

    /// Removes the mailbox at the specified position in the list
    Mailbox* _removeMailboxAt(int index);

    /// Deletes all mailboxes in the list
    void _deleteAllMailboxes();

private:

    int mNumMailboxes;
    Mailbox** mMailboxes;
    int mMailboxesSize;

    void mailboxList_Init();

};

#endif
