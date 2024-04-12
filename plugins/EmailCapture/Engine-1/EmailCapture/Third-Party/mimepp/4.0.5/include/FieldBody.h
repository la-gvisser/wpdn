//=============================================================================
// FieldBody.h
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

#ifndef DW_FIELD_BODY_H
#define DW_FIELD_BODY_H

/// %Class that represents a header field body

class MIMEPP_API FieldBody : public Node {

public:

    /// Default constructor
    FieldBody();

    /// Copy constructor
    FieldBody(const FieldBody& other);

    /// Constructor that takes an initial string and parent node
    FieldBody(const String& str, Node* parent=0);

    /// Destructor
    virtual ~FieldBody();

    /// Assignment operator
    const FieldBody& operator = (const FieldBody& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the unfolded text of the field body
    const String& text() const;

    /// Sets the unfolded text of the field body
    void setText(const String& text);

    /// Folds the string representation, if enabled
    void maybeFold();

    /// Returns true if line folding is enabled
    bool isFoldingEnabled() const;

    /// Enables or disables line folding
    void setFoldingEnabled(bool b);

    /// Performs line folding on its argument
    static String fold(const String& unfolded, unsigned offset);

    /// Performs line unfolding on its argument
    static String unfold(const String& folded);

    /// Creates a new instance
    static FieldBody* newFieldBody();

    /// Provides a class factory hook
    static FieldBody* (*sNewFieldBody)();

protected:

    bool mIsFoldingEnabled;
    String mText;

protected:

    static size_t skipWhiteSpace(const char* barray, size_t pos, size_t end);
    static size_t skipNonWhiteSpace(const char* barray, size_t pos, size_t end);
    static size_t skipComment(const char* barray, size_t pos, size_t end);

};

#endif
