//=============================================================================
// DispositionType.h
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

#ifndef MIMEPP_DISPOSITION_TYPE_H
#define MIMEPP_DISPOSITION_TYPE_H

/// %Class that represents a MIME content-disposition field body

class MIMEPP_API DispositionType : public FieldBody {

public:

    enum DType {
        _NULL = 0,
        UNKNOWN = 1,
        INLINE = 2,
        ATTACHMENT = 3
    };

    /// Default constructor
    DispositionType();

    /// Copy constructor
    DispositionType(const DispositionType& other);

    /// Constructor that takes an initial string and parent node
    DispositionType(const String& str, Node* parent=0);

    /// Destructor
    virtual ~DispositionType();

    /// Assignment operator
    const DispositionType& operator = (const DispositionType& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the disposition type as an enumerated value
    DType asEnum() const;

    /// Sets the disposition type from an enumerated value
    void fromEnum(DType type);

    /// Gets the disposition type value
    const String& type() const;

    /// Sets the disposition type value
    void setType(const String& type);

    /// Gets the value of the filename parameter, if present
    const String& filename() const;

    /// Sets the value of the filename parameter
    void setFilename(const String& name);

    /// Gets the decoded filename parameter, if present
    const String& filenameUtf8() const;

    /// Sets the encoded filename parameter
    void setFilenameUtf8(const String& name);

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
    static DispositionType* newDispositionType();

    /// Provides a class factory hook
    static DispositionType* (*sNewDispositionType)();

protected:

    String mType;
    DType mTypeEnum;
    String mFilename;
    String mFilenameUtf8;

    virtual void enumToStr();
    virtual void strToEnum();

    /// Copies all parameters from another list
    void _copyParameters(const DispositionType& type);

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

    void dispositionType_Init();
    void parseParameters(const char* buf, size_t offset,
        MimeTokenizer& tokenizer);

};

#endif
