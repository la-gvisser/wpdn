//=============================================================================
// TextDecoder.h
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

#ifndef MIMEPP_TEXT_DECODER_H
#define MIMEPP_TEXT_DECODER_H

/// %Class that performs text character decoding.

class MIMEPP_API TextDecoder {
public:

    /// Initializes the class.
    static int initialize();

    /// Finalizes the class.
    static void finalize();

    /// Creates a %TextDecoder instance.
    static TextDecoder* create(const char* charset);

    /// Destructor.
    virtual ~TextDecoder();

    /// Gets the length of the decoded text (number of bytes).
    virtual int getCharsLen(const char* enc, int encLen) = 0;

    /// Gets the decoded text.
    virtual int getChars(const char* enc, int encLen,
        char* dec, int decMaxLen) = 0;
};

#endif
