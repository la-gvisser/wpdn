//=============================================================================
// Stamp.h
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

#ifndef DW_STAMP_H
#define DW_STAMP_H

/// %Class that represents the field body of a "Received" header field.

class MIMEPP_API Stamp : public FieldBody {

public:

    /// Default constructor
    Stamp();

    /// Copy constructor
    Stamp(const Stamp& other);

    /// Constructor that takes an initial string and parent node
    Stamp(const String& str, Node* parent=0);

    /// Destructor
    virtual ~Stamp();

    /// Assignment operator
    const Stamp& operator = (const Stamp& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the sending host
    const String& from() const;

    /// Sets the sending host
    void setFrom(const String& sendingHost);

    /// Gets the receiving host
    const String& by() const;

    /// Sets the receiving host
    void setBy(const String& receivingHost);

    /// Gets the physical mechanism
    const String& via() const;

    /// Sets the physical mechanism
    void setVia(const String& mechanism);

    /// Gets the protocol
    const String& with() const;

    /// Sets the protocol
    void setWith(const String& protocol);

    /// Gets the internal message identifier
    const String& id() const;

    /// Sets the internal message identifier
    void setId(const String& id);

    /// Gets the original addressee
    const String& for_() const;

    /// Sets the original addressee
    void setFor(const String& addressee);

    /// Gets the date and time
    DateTime& date();

    /// Sets the date and time
    void setDate(const DateTime& date);

    /// Creates a new instance
    static Stamp* newStamp();

    /// Provides a class factory hook
    static Stamp* (*sNewStamp)();

protected:

    String  mFrom;
    String  mBy;
    String  mVia;
    String  mWith;
    String  mId;
    String  mFor;
    DateTime mDate;

};

#endif
