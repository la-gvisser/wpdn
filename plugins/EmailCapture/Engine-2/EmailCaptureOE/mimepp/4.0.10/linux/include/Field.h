//=============================================================================
// Field.h
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

#ifndef MIMEPP_FIELD_H
#define MIMEPP_FIELD_H

class FieldBody;

/// %Class that represents a header field

class MIMEPP_API Field : public Node {

public:

    /// Default constructor
    Field();

    /// Copy constructor
    Field(const Field& other);

    /// Constructor that takes an initial string and parent node
    Field(const String& str, Node* parent=0);

    /// Destructor
    virtual ~Field();

    /// Assignment operator
    const Field& operator = (const Field& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the field name
    const String& fieldName() const;

    /// Sets the field name
    void setFieldName(const String& str);

    /// Gets the field body
    class FieldBody& fieldBody() const;

    /// Sets the field body
    void setFieldBody(class FieldBody* fieldBody);

    /// Creates an instance of a subclass of <b>%FieldBody</b>
    static class FieldBody* createFieldBody(const String& fieldName,
        const String& fieldBody, Node* parent);

    /// Default factory function for creating <b>%FieldBody</b> instances
    static class FieldBody* _createFieldBody(const String& fieldName,
        const String& fieldBody, Node* parent);

    /// Factory function hook for creating <b>%FieldBody</b> instances
    static class FieldBody* (*sCreateFieldBody)(const String& fieldName,
        const String& fieldBody, Node* parent);

    /// Creates a new instance
    static Field* newField();

    /// Provides a class factory hook
    static Field* (*sNewField)();

protected:

    // Field name
    String mFieldName;

    // Pointer to the <b>%FieldBody</b> object
    class FieldBody* mFieldBody;

    /// Sets the field body
    void _setFieldBody(class FieldBody* aFieldBody);

};

#endif
