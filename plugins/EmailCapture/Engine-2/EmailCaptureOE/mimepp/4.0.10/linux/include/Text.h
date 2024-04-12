//=============================================================================
// Text.h
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

#ifndef MIMEPP_TEXT_H
#define MIMEPP_TEXT_H

/// %Class that represents text in an unstructured header field body

class MIMEPP_API Text : public FieldBody {

public:

    /// Default constructor
    Text();

    /// Copy constructor
    Text(const Text& other);

    /// Constructor that takes an initial string and parent node
    Text(const String& str, Node* parent=0);

    /// Destructor
    virtual ~Text();

    /// Assignment operator
    const Text& operator = (const Text& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the text as a UTF-8 string
    const String& utf8Text() const;

    /// Gets the charset identifier for the text content
    const String& charset() const;

    /// Sets the text from a UTF-8 string
    void setUtf8Text(const String& utf8str);

    /// Sets the text from a UTF-8 string with charset hint
    void setUtf8Text(const String& utf8str, const String& charset);

    /// Gets the number of encoded words in the list
    int numEncodedWords() const;

    /// Adds an encoded word to the end of the list
    void addEncodedWord(EncodedWord* word);

    /// Deletes all encoded words in the list
    void deleteAllEncodedWords();

    /// Gets the encoded word at the specified position in the list
    EncodedWord& encodedWordAt(int index) const;

    /// Inserts an encoded word at the specified position in the list
    void insertEncodedWordAt(int index, EncodedWord* word);

    /// Removes the encoded word at the specified position in the list
    EncodedWord* removeEncodedWordAt(int index);

    /// Creates a new instance
    static Text* newText();

    /// Provides a class factory hook
    static Text* (*sNewText)();

protected:

    /// Copies all encoded words from another list
    void _copyEncodedWords(const Text& text);

    /// Adds an encoded word to the end of the list
    void _addEncodedWord(EncodedWord* word);

    /// Inserts an encoded word at the specified position in the list
    void _insertEncodedWordAt(int index, EncodedWord* word);

    /// Removes the encoded word at the specified position in the list
    EncodedWord* _removeEncodedWordAt(int index);

    /// Deletes all encoded words in the list
    void _deleteAllEncodedWords();

private:

    int mNumEncodedWords;
    EncodedWord** mEncodedWords;
    int mEncodedWordsSize;

    mutable bool mIsUtf8Null;
    mutable String mUtf8Text;
    mutable String mCharset;

    void _parseAlt();
    void text_Init();
    void _getUtf8Text() const;
    void _encode_ASCII();
    void _encode_utf8();
    void _encode_other();
    void _qencode_8bit();
    void _qencode_utf8();
    void _bencode_utf8();
    void _bencode_other();

};

#endif
