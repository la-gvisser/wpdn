//=============================================================================
// QuotedPrintableEncoder.h
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

#ifndef DW_QUOTED_PRINTABLE_ENCODER_H
#define DW_QUOTED_PRINTABLE_ENCODER_H

/// %Class that performs quoted-printable encoding.

class MIMEPP_API QuotedPrintableEncoder {

public:

    enum {
        SAFE = 1,
        UNSAFE = 2,
        SPECIAL = 3
    };

    /// Default constructor.
    QuotedPrintableEncoder();

    /// Destructor.
    ~QuotedPrintableEncoder();

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

    /// Sets the option to protect "From " at the beginning of a line.
    void setProtectFrom(bool b);

    /// Gets the option to protect "From " at the beginning of a line.
    bool protectFrom();

    /// Sets the option to protect a dot at the beginning of a line.
    void setProtectDot(bool b);

    /// Gets the option to protect a dot at the beginning of a line.
    bool protectDot();

    /// Sets the lookup table that determines how characters are encoded.
    void setEncodeMap(const unsigned char *map);

    /// Starts a multiple-buffer encode operation.
    void start();

    /// Encodes data from the input buffer to the output buffer.
    void encodeSegment(ByteBuffer* inBuf, CharBuffer* outBuf);

    /// Finishes a multiple-buffer encode operation.
    void finish(CharBuffer* outBuf);

    /// Performs single-step buffer-to-buffer quoted-printable encoding.
    String encode(const String& decoded);

    /// Low risk encoding map.
    static const unsigned char* const ENCODE_MAP_LOW_RISK;

    /// High risk encoding map.
    static const unsigned char* const ENCODE_MAP_HIGH_RISK;

private:

    void* mImpl;

};

#endif
