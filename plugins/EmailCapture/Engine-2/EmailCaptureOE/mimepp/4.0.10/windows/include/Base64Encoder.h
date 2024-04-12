//=============================================================================
// Base64Encoder.h
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

#ifndef MIMEPP_BASE64_ENCODER_H
#define MIMEPP_BASE64_ENCODER_H

/// %Class that performs base64 encoding.

class MIMEPP_API Base64Encoder {

public:

    /// Default constructor.
    Base64Encoder();

    /// Destructor.
    ~Base64Encoder();

    /// Sets the maximum line length of the encoded output.
    void setMaxLineLen(int len);

    /// Gets the maximum line length of the encoded output.
    int maxLineLen();

    /// Sets the CRLF end-of-line characters option.
    void setOutputCrLf(bool b);

    /// Gets the CRLF end-of-line characters option.
    bool outputCrLf();

    /// Sets the option to suppress a final newline in the output.
    void setSuppressFinalNewline(bool b);

    /// Gets the option to suppress a final newline in the output.
    bool suppressFinalNewline();

    /// Starts a multiple-buffer encode operation.
    void start();

    /// Encodes data from the input buffer to the output buffer.
    void encodeSegment(ByteBuffer* inBuf, CharBuffer* outBuf);

    /// Finishes a multiple-buffer encode operation.
    void finish(CharBuffer* outBuf);

    /// Performs single-step buffer-to-buffer base64 encoding.
    String encode(const String& decoded);

private:

    void* mImpl;

};

#endif
