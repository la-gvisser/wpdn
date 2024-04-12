//=============================================================================
// AddressList.h
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

#ifndef MIMEPP_ADDRESS_LIST_H
#define MIMEPP_ADDRESS_LIST_H

/// %Class that represents a list of RFC&nbsp;2822 mail addresses

class MIMEPP_API AddressList : public FieldBody {

public:

    /// Default constructor
    AddressList();

    /// Copy constructor
    AddressList(const AddressList& other);

    /// Constructor that takes an initial string and parent node
    AddressList(const String& str, Node* parent=0);

    /// Destructor
    virtual ~AddressList();

    /// Assignment operator
    const AddressList& operator = (const AddressList& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the number of addresses in the list
    int numAddresses() const;

    /// Adds an address to the end of the list
    void addAddress(Address* address);

    /// Deletes all addresses in the list
    void deleteAllAddresses();

    /// Gets the address at the specified position in the list
    Address& addressAt(int index) const;

    /// Inserts an address at the specified position in the list
    void insertAddressAt(int index, Address* address);

    /// Removes the address at the specified position in the list
    Address* removeAddressAt(int index);

    /// Creates a new instance
    static AddressList* newAddressList();

    /// Provides a class factory hook
    static AddressList* (*sNewAddressList)();

protected:

    /// Copies all addresses from another list
    void _copyAddresses(const AddressList& list);

    /// Adds an address to the end of the list
    void _addAddress(Address* address);

    /// Inserts an address at the specified position in the list
    void _insertAddressAt(int index, Address* address);

    /// Removes the address at the specified position in the list
    Address* _removeAddressAt(int index);

    /// Deletes all addresses in the list
    void _deleteAllAddresses();

private:

    int mNumAddresses;
    Address** mAddresses;
    int mAddressesSize;

    void addressList_Init();

};

#endif
