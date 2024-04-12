//=============================================================================
// TextEncoder.h
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

#ifndef DW_TEXT_ENCODER_H
#define DW_TEXT_ENCODER_H

/// %Class that performs text character encoding.

class MIMEPP_API TextEncoder {
public:

    /// Initializes the class.
    static int initialize();

    /// Finalizes the class.
    static void finalize();

    /// Creates a %TextEncoder instance.
    static TextEncoder* create(const char* charset);

    /// Destructor.
    virtual ~TextEncoder();

    /// Gets the length of the encoded text (number of bytes).
    virtual int getBytesLen(const char* dec, int decLen) = 0;

    /// Gets the encoded text.
    virtual int getBytes(const char* dec, int decLen,
        char* enc, int encMaxLen) = 0;
};

#endif
