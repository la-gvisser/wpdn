//=============================================================================
// Message.h
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

#ifndef MIMEPP_MESSAGE_H
#define MIMEPP_MESSAGE_H

/// %Class that represents an RFC&nbsp;2822/MIME message

class MIMEPP_API Message : public Entity {

public:

    /// Default constructor
    Message();

    /// Copy constructor
    Message(const Message& other);

    /// Constructor that takes an initial string and parent node
    Message(const String& str, Node* parent=0);

    /// Destructor
    virtual ~Message();

    /// Assignment operator
    const Message& operator = (const Message& other);

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Creates a new instance
    static Message* newMessage();

    /// Provides a class factory hook
    static Message* (*sNewMessage)();

};

#endif
