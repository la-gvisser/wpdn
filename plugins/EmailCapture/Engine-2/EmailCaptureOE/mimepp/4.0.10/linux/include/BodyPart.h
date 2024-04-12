//=============================================================================
// BodyPart.h
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

#ifndef MIMEPP_BODY_PART_H
#define MIMEPP_BODY_PART_H

/// %Class that represents a MIME body part

class MIMEPP_API BodyPart : public Entity {

public:

    /// Default constructor
    BodyPart();

    /// Copy constructor
    BodyPart(const BodyPart& other);

    /// Constructor that takes an initial string and parent node
    BodyPart(const String& str, Node* parent=0);

    /// Destructor
    virtual ~BodyPart();

    /// Assignment operator
    const BodyPart& operator = (const BodyPart& other);

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Creates a new instance
    static BodyPart* newBodyPart();

    /// Provides a class factory hook
    static BodyPart* (*sNewBodyPart)();

};

#endif
