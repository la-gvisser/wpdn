//=============================================================================
// Base64Decoder.h
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

#ifndef MIMEPP_BASE64_DECODER_H
#define MIMEPP_BASE64_DECODER_H

/// %Class that performs base64 decoding.

class MIMEPP_API Base64Decoder {

public:

    /// Default constructor.
    Base64Decoder();

    /// Destructor.
    ~Base64Decoder();

    /// Starts a multiple-buffer decode operation.
    void start();

    /// Decodes data from the input buffer to the output buffer.
    int decodeSegment(CharBuffer* inBuf, ByteBuffer* outBuf);

    /// Performs single-step buffer-to-buffer base64 decoding.
    String decode(const String& encoded);

private:

    void* mImpl;

};

#endif
