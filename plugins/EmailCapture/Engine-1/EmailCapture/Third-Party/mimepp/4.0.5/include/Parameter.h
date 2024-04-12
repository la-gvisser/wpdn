//=============================================================================
// Parameter.h
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

#ifndef DW_PARAMETER_H
#define DW_PARAMETER_H

/// %Class that represents a MIME field body parameter

class MIMEPP_API Parameter : public Node {

public:

    /// Default constructor
    Parameter();

    /// Copy constructor
    Parameter(const Parameter& other);

    /// Constructor that takes an initial string and parent node
    Parameter(const String& str, Node* parent=0);

    /// Destructor
    virtual ~Parameter();

    /// Assignment operator
    const Parameter& operator = (const Parameter& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the name
    const String& name() const;

    /// Sets the name
    void setName(const String& name);

    /// Gets the value
    const String& value() const;

    /// Sets the value
    void setValue(const String& value);

    /// Creates a new instance
    static Parameter* newParameter();

    /// Provides a class factory hook
    static Parameter* (*sNewParameter)();

private:

    String mName;
    String mValue;

};

#endif
