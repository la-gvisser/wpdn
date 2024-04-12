//=============================================================================
// MsgId.h
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

#ifndef DW_MSG_ID_H
#define DW_MSG_ID_H

/// %Class that represents an RFC&nbsp;2822 message ID

class MIMEPP_API MsgId : public FieldBody {

public:

    /// Default constructor
    MsgId();

    /// Copy constructor
    MsgId(const MsgId& other);

    /// Constructor that takes an initial string and parent node
    MsgId(const String& str, Node* parent=0);

    /// Destructor
    virtual ~MsgId();

    /// Assignment operator
    const MsgId& operator = (const MsgId& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the local part
    const String& localPart() const;

    /// Sets the local part
    void setLocalPart(const String& localPart);

    /// Gets the domain
    const String& domain() const;

    /// Gets the domain
    void setDomain(const String& domain);

    /// Sets the local part and the domain to default values
    virtual void createDefault();

    /// Sets the local part to a default value
    virtual void createLocalPart();

    /// Sets the domain to a default value
    virtual void createDomain();

    /// Sets the default value for the domain
    static void setDefaultDomain(const char* domain);

    /// Creates a new instance
    static MsgId* newMsgId();

    /// Provides a class factory hook
    static MsgId* (*sNewMsgId)();

protected:

    /// Default value for the domain
    static char* sDefaultDomain;

private:

    String mLocalPart;
    String mDomain;

};

#endif
