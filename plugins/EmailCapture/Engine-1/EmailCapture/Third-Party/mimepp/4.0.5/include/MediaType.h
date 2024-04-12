//=============================================================================
// MediaType.h
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

#ifndef DW_MEDIA_TYPE_H
#define DW_MEDIA_TYPE_H

/// %Class that represents a MIME media-type

class MIMEPP_API MediaType : public FieldBody {

public:

    enum MType {
        _NULL = 0,
        UNKNOWN = 1,
        TEXT = 2,
        IMAGE = 3,
        AUDIO = 4,
        VIDEO = 5,
        APPLICATION = 6,
        MESSAGE = 7,
        MULTIPART = 8,
        MODEL = 9
    };

    /// Default constructor
    MediaType();

    /// Copy constructor
    MediaType(const MediaType& other);

    /// Constructor that takes an initial string and parent node
    MediaType(const String& str, Node* parent=0);

    /// Destructor
    virtual ~MediaType();

    /// Assignment operator
    const MediaType& operator = (const MediaType& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the primary type as an enumerated value
    MType typeAsEnum() const;

    /// Sets the primary type from an enumerated value
    void typeFromEnum(MType type);

    /// Gets the primary type as a string
    const String& type() const;

    /// Sets the primary type from a string
    void setType(const String& str);

    /// Gets the subtype as a string
    const String& subtype() const;

    /// Sets the subtype from a string
    void setSubtype(const String& str);

    /// Gets the value of the boundary parameter, if present
    const String& boundary() const;

    /// Sets the value of the boundary parameter
    void setBoundary(const String& str);

    /// Creates and adds a boundary parameter
    virtual const String& createBoundary(unsigned level=0);

    /// Gets the value of the name parameter, if present
    const String& name() const;

    /// Sets the value of the name parameter
    void setName(const String& str);

    /// Gets the value of the charset parameter, if present
    const String& charset() const;

    /// Sets the value of the charset parameter
    void setCharset(const String& str);

    /// Gets the number of parameters in the list
    int numParameters() const;

    /// Adds a parameter to the end of the list
    void addParameter(Parameter* param);

    /// Deletes all parameters in the list
    void deleteAllParameters();

    /// Gets the parameter at the specified position in the list
    Parameter& parameterAt(int index) const;

    /// Inserts a parameter at the specified position in the list
    void insertParameterAt(int index, Parameter* param);

    /// Removes the parameter at the specified position in the list
    Parameter* removeParameterAt(int index);

    /// Creates a new instance
    static MediaType* newMediaType();

    /// Provides a class factory hook
    static MediaType* (*sNewMediaType)();

protected:

    String mType;
    String mSubtype;
    MType mTypeEnum;
    String mBoundary;
    String mName;
    String mCharset;

    void typeEnumToStr();
    void typeStrToEnum();

    /// Copies all parameters from another list
    void _copyParameters(const MediaType& type);

    /// Adds a parameter to the end of the list
    void _addParameter(Parameter* parameter);

    /// Inserts a parameter at the specified position in the list
    void _insertParameterAt(int index, Parameter* param);

    /// Removes the parameter at the specified position in the list
    Parameter* _removeParameterAt(int index);

    /// Deletes all parameters in the list
    void _deleteAllParameters();

private:

    int mNumParameters;
    Parameter** mParameters;
    int mParametersSize;

    void mediaType_Init();
    void parseParameters(const char* buf, size_t offset,
        MimeTokenizer& tokenizer);

};

#endif
