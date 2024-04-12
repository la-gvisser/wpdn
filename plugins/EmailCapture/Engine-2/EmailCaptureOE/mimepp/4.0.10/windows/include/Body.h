//=============================================================================
// Body.h
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

#ifndef MIMEPP_BODY_H
#define MIMEPP_BODY_H

class BodyPart;
class Message;

/// %Class that represents the body of a message or body part

class MIMEPP_API Body : public Node {

public:

    /// Default constructor
    Body();

    /// Copy constructor
    Body(const Body& other);

    /// Constructor that takes an initial string and parent node
    Body(const String& str, Node* parent=0);

    /// Destructor
    virtual ~Body();

    /// Assignment operator
    const Body& operator = (const Body& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the number of body parts in the list
    int numBodyParts() const;

    /// Adds a body part to the end of the list
    void addBodyPart(BodyPart* part);

    /// Deletes all body parts in the list
    void deleteAllBodyParts();

    /// Gets the body part at the specified position in the list
    BodyPart& bodyPartAt(int index) const;

    /// Inserts a body part at the specified position in the list
    void insertBodyPartAt(int index, BodyPart* part);

    /// Removes the body part at the specified position in the list
    BodyPart* removeBodyPartAt(int index);

    /// Gets the contained message
    class Message* message() const;

    /// Sets the contained message
    void setMessage(class Message* message);

    /// Gets the preamble of a multipart body
    const String& preamble() const;

    /// Sets the preamble of a multipart body
    void setPreamble(const String& str);

    /// Gets the epilogue of a multipart body
    const String& epilogue() const;

    /// Sets the epilogue of a multipart body
    void setEpilogue(const String& str);

    /// Creates a new instance
    static Body* newBody();

    /// Provides a class factory hook
    static Body* (*sNewBody)();

    void _parse(int depth);

protected:

    /// Copies all body parts from another list
    void _copyBodyParts(const Body& body);

    /// Adds a body part to the end of the list
    void _addBodyPart(BodyPart* bart);

    /// Inserts a body part at the specified position in the list
    void _insertBodyPartAt(int index, BodyPart* part);

    /// Removes the body part at the specified position in the list
    BodyPart* _removeBodyPartAt(int index);

    /// Deletes all body parts in the list
    void _deleteAllBodyParts();

    /// Sets the contained message
    void _setMessage(class Message* message);

private:

    String mBoundaryStr;
    String mPreamble;
    String mEpilogue;

    int mNumBodyParts;
    BodyPart** mBodyParts;
    int mBodyPartsSize;

    class Message* mMessage;

    void body_Init();

};

#endif
