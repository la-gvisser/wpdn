//=============================================================================
// EncodedWord.h
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

#ifndef DW_ENCODED_WORD_H
#define DW_ENCODED_WORD_H

/// %Class that represents an encoded word in a header field

class MIMEPP_API EncodedWord : public Node {

public:

    /// Default constructor
    EncodedWord();

    /// Copy constructor
    EncodedWord(const EncodedWord& word);

    /// Constructor that takes an initial string and parent node
    EncodedWord(const String& str, Node* parent=0);

    /// Destructor
    virtual ~EncodedWord();

    /// Assignment operator
    const EncodedWord& operator = (const EncodedWord& word);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the charset designator
    const String& charset() const;

    /// Sets the charset designator
    void setCharset(const String& charset);

    /// Gets the encoding type
    char encodingType() const;

    /// Sets the encoding type
    void setEncodingType(char encodingType);

    /// Gets the decoded text
    const String& decodedText() const;

    /// Sets the decoded text
    void setDecodedText(const String& text);

    enum {
        SAFE = 1,
        UNSAFE = 2,
        SPECIAL = 3
    };

    static unsigned char* QP_ENCODE_MAP;

    /// Encodes a string with the Q encoding
    static String qEncode(const String& str);

    /// Encodes a string with the B encoding
    static String bEncode(const String& str);

    /// Decodes a string with the Q encoding
    static int qDecode(const String& inStr, String& outStr);

    /// Decodes a string with the B encoding
    static int bDecode(const String& inStr, String& outStr);

    /// Creates a new instance
    static EncodedWord* newEncodedWord();

    /// Provides a class factory hook
    static EncodedWord* (*sNewEncodedWord)();

protected:

    String mCharset;
    char mEncodingType;
    String mDecodedText;

};

#endif
