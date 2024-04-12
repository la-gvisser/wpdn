//=============================================================================
// Rfc822Tokenizer.h
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

#ifndef DW_RFC822_TOKENIZER_H
#define DW_RFC822_TOKENIZER_H

class MIMEPP_API Rfc822Tokenizer
{
public:
    enum TokenType {
        NULL_ = 0,
        SPECIAL = 1,
        ATOM = 2,
        COMMENT = 4,
        QUOTED_STRING = 8,
        DOMAIN_LITERAL = 16,
        ERROR_ = 32
    };

    Rfc822Tokenizer(const char* buf, size_t beginOffset, size_t endOffset);
    TokenType parseNext(size_t* tokenBegin, size_t* tokenEnd);

    static void appendDelimited(String& dst, const char* barray, size_t begin,
        size_t end, bool compact);
    static String quoteIfNecessary(const String& str);

private:
    const char* mBuffer;
    size_t mPos;
    size_t mEnd;
    TokenType parseQuotedString();
    TokenType parseComment();
    TokenType parseDomainLiteral();
    TokenType parseAtom();
};

#endif
