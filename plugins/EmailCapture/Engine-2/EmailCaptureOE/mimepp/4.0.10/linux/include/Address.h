//=============================================================================
// Address.h
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

#ifndef MIMEPP_ADDRESS_H
#define MIMEPP_ADDRESS_H

/// Abstract class that represents an RFC&nbsp;2822 mail address

class MIMEPP_API Address : public FieldBody
{
public:

    /// Destructor
    virtual ~Address();

    /// Returns true if the address is valid
    bool isValid() const;

protected:

    /// Default constructor
    Address();

    /// Copy constructor
    Address(const Address& other);

    /// Constructor that takes an initial string and parent node
    Address(const String& str, Node* parent=0);

    /// Assignment operator
    const Address& operator = (const Address& other);

    /// Set to true if the parse operation was successful.
    bool mIsValid;

};

#endif
