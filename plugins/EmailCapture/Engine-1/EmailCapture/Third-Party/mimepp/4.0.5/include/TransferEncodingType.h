//=============================================================================
// TransferEncodingType.h
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

#ifndef DW_TRANSFER_ENCODING_TYPE_H
#define DW_TRANSFER_ENCODING_TYPE_H

/// %Class that represents a MIME content-transfer-encoding field body

class MIMEPP_API TransferEncodingType : public FieldBody {

public:

    enum EType {
        _NULL = 0,
        UNKNOWN = 1,
        _7BIT = 2,
        _8BIT = 3,
        QUOTED_PRINTABLE = 4,
        BASE64 = 5,
        BINARY = 6
    };

    /// Default constructor
    TransferEncodingType();

    /// Copy constructor
    TransferEncodingType(const TransferEncodingType& other);

    /// Constructor that takes an initial string and parent node
    TransferEncodingType(const String& str, Node* parent=0);

    /// Destructor
    virtual ~TransferEncodingType();

    /// Assignment operator
    const TransferEncodingType& operator = (const TransferEncodingType& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the transfer encoding type
    const String& type() const;

    /// Sets the transfer encoding type
    void setType(const String& type);

    /// Gets the transfer encoding type as an enumerated value
    EType asEnum() const;

    /// Sets the transfer encoding type from an enumerated value
    void fromEnum(EType cte);

    /// Creates a new instance
    static TransferEncodingType* newTransferEncodingType();

    /// Provides a class factory hook
    static TransferEncodingType* (*sNewTransferEncodingType)();

private:

    String mType;
    EType mTypeEnum;

    void enumToStr();
    void strToEnum();

};

#endif
