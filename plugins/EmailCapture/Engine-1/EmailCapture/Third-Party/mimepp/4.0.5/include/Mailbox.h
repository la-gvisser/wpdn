//=============================================================================
// Mailbox.h
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

#ifndef DW_MAILBOX_H
#define DW_MAILBOX_H

/// %Class that represents an RFC&nbsp;2822 mailbox

class MIMEPP_API Mailbox : public Address
{
public:

    /// Default constructor
    Mailbox();

    /// Copy constructor
    Mailbox(const Mailbox& other);

    /// Constructor that takes an initial string and parent node
    Mailbox(const String& str, Node* parent=0);

    /// Destructor
    virtual ~Mailbox();

    /// Assignment operator
    const Mailbox& operator = (const Mailbox& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the display name
    const String& displayNameUtf8() const;

    /// Gets the charset identifier for the display name
    const String& charset() const;

    /// Sets the display name from a UTF-8 string
    void setDisplayNameUtf8(const String& name);

    /// Sets the display name from a UTF-8 string, with a charset hint
    void setDisplayNameUtf8(const String& name, const String& charset);

    /// Gets the encoded display name
    const String& encodedDisplayName() const;

    /// Sets the encoded display name
    void setEncodedDisplayName(const String& name);

    /// Gets the source route
    const String& route() const;

    /// Sets the source route
    void setRoute(const String& route);

    /// Gets the local part
    const String& localPart() const;

    /// Sets the local part
    void setLocalPart(const String& localPart);

    /// Gets the domain
    const String& domain() const;

    /// Sets the domain
    void setDomain(const String& domain);

    /// Creates a new instance
    static Mailbox* newMailbox();

    /// Provides a class factory hook
    static Mailbox* (*sNewMailbox)();

private:

    String mEncodedDisplayName;
    mutable String mDisplayName;
    mutable String mCharset;
    String mRoute;
    String mLocalPart;
    String mDomain;

    void _setDisplayNameUtf8(const String& name, const String& charset);
    void _decodeDisplayName() const;

};

#endif
