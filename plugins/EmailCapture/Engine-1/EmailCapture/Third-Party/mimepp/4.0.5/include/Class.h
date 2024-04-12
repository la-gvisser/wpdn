//=============================================================================
// Class.h
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

#ifndef DW_CLASS_H
#define DW_CLASS_H

/// %Class that provides run-time type information

class MIMEPP_API Class {
public:
    /// Constructor
    Class(const char* name, int id, const Class* superclass);
    /// Gets the class name
    const char* name() const { return mName; }
    /// Gets the class ID
    int id() const { return mId; }
    /// Checks the type of an object by ID
    const bool isA(int id) const;
    /// Checks the type of an object by name
    const bool isA(const char* name) const;
    /// Gets a pointer to the superclass
    const Class* superclass() const { return mSuperclass; }
private:
	/// Assignment operator
	Class& operator=(const Class& ) {}
protected:
    /// %Class name
    const char* const mName;
    /// %Class ID
    const int mId;
    /// Pointer to the superclass instance
    const Class* const mSuperclass;
};

inline mimepp::Class::Class(const char* name, int id, const Class* superclass)
 :  mName(name),
    mId(id),
    mSuperclass(superclass)
{
}

#endif
