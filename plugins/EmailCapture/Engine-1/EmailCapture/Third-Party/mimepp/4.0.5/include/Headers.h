//=============================================================================
// Headers.h
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

#ifndef DW_HEADERS_H
#define DW_HEADERS_H

/// %Class that represents the collection of header fields in a message or body part

class MIMEPP_API Headers : public Node {

public:

    /// Default constructor
    Headers();

    /// Copy constructor
    Headers(const Headers& other);

    /// Constructor that takes an initial string and parent node
    Headers(const String& str, Node* parent=0);

    /// Destructor
    virtual ~Headers();

    /// Assignment operator
    const Headers& operator = (const Headers& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the number of header fields in the list
    int numFields() const;

    /// Adds a header field to the end of the list
    void addField(Field* field);

    /// Deletes all header fields in the list
    void deleteAllFields();

    /// Gets the header field at the specified position in the list
    Field& fieldAt(int index) const;

    /// Inserts a header field at the specified position in the list
    void insertFieldAt(int index, Field* field);

    /// Removes the header field at the specified position in the list
    Field* removeFieldAt(int index);

    /// Returns true if the specified header field is present
    bool hasField(const char* fieldName) const;

    /// Gets the field body of the specified header field
    class FieldBody& fieldBody(const char* fieldName);

    // RFC 2822 fields

    /// Gets the field body of the Bcc header field
    AddressList& bcc();
    /// Gets the field body of the Cc header field
    AddressList& cc();
    /// Gets the field body of the Date header field
    DateTime& date();
    /// Gets the field body of the From header field
    MailboxList& from();
    /// Gets the field body of the %Message-Id header field
    MsgId& messageId();
    /// Gets the field body of the Received header field
    Stamp& received();
    /// Gets the field body of the Reply-To header field
    AddressList& replyTo();
    /// Gets the field body of the Resent-Bcc header field
    AddressList& resentBcc();
    /// Gets the field body of the Resent-Cc header field
    AddressList& resentCc();
    /// Gets the field body of the Resent-Date header field
    DateTime& resentDate();
    /// Gets the field body of the Resent-From header field
    MailboxList& resentFrom();
    /// Gets the field body of the Resent-%Message-Id header field
    MsgId& resentMessageId();
    /// Gets the field body of the Resent-Reply-To header field
    AddressList& resentReplyTo();
    /// Gets the field body of the Resent-Sender header field
    Mailbox& resentSender();
    /// Gets the field body of the Resent-To header field
    AddressList& resentTo();
    /// Gets the field body of the Return-Path header field
    Address& returnPath();
    /// Gets the field body of the Sender header field
    Mailbox& sender();
    /// Gets the field body of the Subject header field
    Text& subject();
    /// Gets the field body of the To header field
    AddressList& to();

    // RFC 2045 fields

    /// Gets the field body of the Content-Id header field
    MsgId& contentId();
    /// Gets the field body of the Content-Transfer-Encoding header field
    TransferEncodingType& contentTransferEncoding();
    /// Gets the field body of the Content-Type header field
    MediaType& contentType();

    // RFC 1806 Content-Disposition field

    /// Gets the field body of the Content-Dispostion header field
    DispositionType& contentDisposition();

    /// Searches for the specified field
    Field* findField(const char* fieldName) const;

    /// Searches for the specified field
    int findFieldPos(const char* fieldName) const;

    /// Creates a new instance
    static Headers* newHeaders();

    /// Provides a class factory hook
    static Headers* (*sNewHeaders)();

protected:

    /// Copies all header fields from another list
    void _copyFields(const Headers& headers);

    /// Adds a header field to the end of the list
    void _addField(Field* field);

    /// Inserts a header field at the specified position in the list
    void _insertFieldAt(int index, Field* field);

    /// Removes the header field at the specified position in the list
    Field* _removeFieldAt(int index);

    /// Deletes all header fields in the list
    void _deleteAllFields();

private:

    int mNumFields;
    Field** mFields;
    int mFieldsSize;

    void headers_Init();

};

#endif
