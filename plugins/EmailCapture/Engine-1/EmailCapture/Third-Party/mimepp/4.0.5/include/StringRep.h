//=============================================================================
// StringRep.h
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

#ifndef DW_STRING_REP_H
#define DW_STRING_REP_H

class MIMEPP_API StringRep {
public:
    // Used by the library to create a StringRep instance
    static StringRep* create(char* buffer, size_t size, int refCount);

    // Used by the library and by applications to allocate memory.
    // Applications should use this function to allocate memory if the
    // library will be freeing the memory.
    static void* allocBuffer(size_t size);

    // Used by the library and by applications to free memory allocated
    // by allocBuffer().
    static void freeBuffer(void* buffer);

    virtual ~StringRep();

    // Returns a pointer to the beginning of the internal buffer.  Called
    // by the library.
    virtual char* buffer() = 0;

    // Returns the size of the internal buffer.  Called by the library.
    virtual size_t size() = 0;

    // Returns true if this StringRep is shared -- that is, the reference
    // count > 1.  Called by the library.
    virtual bool isShared() = 0;

    // Increments the reference count.  Called by the library.
    virtual int incRef() = 0;

    // Decrements the reference count.  Called by the library.
    virtual int decRef() = 0;
};

#endif
