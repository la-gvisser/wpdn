//=============================================================================
// TextUtil.h
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

#ifndef MIMEPP_TEXT_UTIL_H
#define MIMEPP_TEXT_UTIL_H

/// %Class that provides text utility functions

class MIMEPP_API TextUtil {

public:

    /// Named constant for "\r\\n"
    static const char* const CRLF;

    /// Named constant for "\ n"
    static const char* const LF;

    /// The library's default end-of-line characters
    static const char* EOL_CHARS;

    /// Converts to CR LF end-of-line characters
    static String toCrLf(String s);

    /// Converts to LF end-of-line character
    static String toLf(String s);

    /// Converts to the default end-of-line characters
    static String toLocalEol(String s);

};

#endif
