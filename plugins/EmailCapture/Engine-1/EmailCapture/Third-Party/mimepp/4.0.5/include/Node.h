//=============================================================================
// Node.h
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

#ifndef DW_NODE_H
#define DW_NODE_H

/// Abstract base class for all classes representing message components

class MIMEPP_API Node {

public:

    /// Default constructor
    Node();

    /// Copy constructor
    Node(const Node& other);

    /// Constructor that takes an initial string and parent node
    Node(const String& str, Node* parent=0);

    /// Destructor
    virtual ~Node();

    /// Assignment operator
    const Node& operator = (const Node& other);

    /// Parses the string representation
    virtual void parse() = 0;

    /// Assembles the string representation
    virtual void assemble() = 0;

    /// Creates a copy of this object
    virtual Node* clone() const = 0;

    /// Sets the string representation
    void setString(const String& str);

    /// Sets the string representation
    void setString(const char* cstr);

    /// Gets the string representation
    const String& getString() const;

    /// Gets the parent node
    Node* parent() const;

    /// Sets the parent node
    void setParent(Node* parent);

    /// Gets the is-modified flag
    bool isModified() const;

    /// Sets the is-modified flag
    void setModified();

    /// Gets a reference to the singleton <b>%Class</b> instance
    const class Class& class_() const;

    /// Sets the string representation
    void _setString(const String& str);

protected:

    /// %String representation
    String mString;

    /// Is-modified flag
    bool mIsModified;

    /// Parent node
    Node* mParent;

    /// Pointer to the singleton <b>%Class</b> instance
    const class Class* mClass;

};

#endif
