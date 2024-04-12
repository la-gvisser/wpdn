//=============================================================================
// Entity.h
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

#ifndef MIMEPP_ENTITY_H
#define MIMEPP_ENTITY_H

/// Abstract class that represents a MIME entity

class MIMEPP_API Entity : public Node {

public:

    /// Default constructor
    Entity();

    /// Copy constructor
    Entity(const Entity& other);

    /// Constructor that takes an initial string and parent node
    Entity(const String& str, Node* parent=0);

    /// Destructor
    virtual ~Entity();

    /// Assignment operator
    const Entity& operator = (const Entity& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Gets the header fields of the entity
    class Headers& headers() const;

    /// Gets the body of the entity
    class Body& body() const;

    virtual void _parse(int depth);

protected:

    class Headers* mHeaders;
    class Body* mBody;

};

#endif
