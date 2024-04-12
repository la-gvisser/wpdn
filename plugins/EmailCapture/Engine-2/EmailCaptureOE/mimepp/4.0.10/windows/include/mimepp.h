//=============================================================================
// mimepp.h
//
// Copyright (c) 1996-2006 Hunny Software, Inc
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

#ifndef MIMEPP_MIMEPP_H
#define MIMEPP_MIMEPP_H

// If we are compiling on Windows, then define the MIMEPP_API macro to
// correctly define the DLL export/import directive.  Otherwise, define
// MIMEPP_API to a null value.

#if (defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)) && ! defined(MIMEPP_STATIC)
#  ifdef MIMEPP_EXPORTS
#    define MIMEPP_API __declspec(dllexport)
#  else
#    define MIMEPP_API __declspec(dllimport)
#  endif
#elif defined(__APPLE__)
#  ifdef MIMEPP_EXPORTS
#    define MIMEPP_API __attribute__((visibility("default")))
#  else
#    define MIMEPP_API
#  endif
#else
#  define MIMEPP_API
#endif

#include <stdlib.h>

// Compiler specific sections

// Microsoft Visual C++ on Microsoft Windows
#if defined(_MSC_VER) && defined(_WIN32) && ! defined(_WIN32_WCE)
#  include <time.h>   // for time_t
#  if !defined(MIMEPP_INT_TYPES_DEFINED)
#  define MIMEPP_INT_TYPES_DEFINED
   typedef signed char int8_t;
   typedef unsigned char uint8_t;
   typedef short int16_t;
   typedef unsigned short uint16_t;
   typedef int int32_t;
   typedef unsigned int uint32_t;
#  endif // !defined(MIMEPP_INT_TYPES_DEFINED)
#endif

// Microsoft Visual C++ on Microsoft Windows CE
#if defined(_MSC_VER) && defined(_WIN32) && defined(_WIN32_WCE)
#  if !defined(MIMEPP_INT_TYPES_DEFINED)
#  define MIMEPP_INT_TYPES_DEFINED
   typedef signed char int8_t;
   typedef unsigned char uint8_t;
   typedef short int16_t;
   typedef unsigned short uint16_t;
   typedef int int32_t;
   typedef unsigned int uint32_t;
#  endif // !defined(MIMEPP_INT_TYPES_DEFINED)
#endif // Microsoft Visual C++ on Microsoft Windows CE

// GCC or Forte C++ on Sun Solaris
#if defined(__sun)
#  include <time.h>      // for time_t
#  include <inttypes.h>  // for int32_t, etc
#endif // GCC or Forte C++ on Sun Solaris

// GCC on Linux
#if defined(__linux)
#  include <time.h>      // for time_t
#  include <inttypes.h>  // for int32_t, etc
#endif // GCC on Linux

// DEC cxx on Tru64 Unix
#if defined(__DECCXX)
#  if !defined(MIMEPP_INT_TYPES_DEFINED)
#  define MIMEPP_INT_TYPES_DEFINED
   typedef signed char int8_t;
   typedef unsigned char uint8_t;
   typedef short int16_t;
   typedef unsigned short uint16_t;
   typedef int int32_t;
   typedef unsigned int uint32_t;
#  endif // !defined(MIMEPP_INT_TYPES_DEFINED)
#endif // defined(__DECCXX)

// GCC on OS X
#if defined(__APPLE__)
#  include <time.h>
#  include <inttypes.h>  // for int32_t, etc
#endif

namespace mimepp {

enum {
    ADDRESS_CLASS = 1,
    ADDRESS_LIST_CLASS = 2,
    BODY_CLASS = 3,
    BODY_PART_CLASS = 4,
    DATE_TIME_CLASS = 5,
    DISPOSITION_TYPE_CLASS = 6,
    ENCODED_WORD_CLASS = 7,
    ENTITY_CLASS = 8,
    FIELD_CLASS = 9,
    FIELD_BODY_CLASS = 10,
    GROUP_CLASS = 11,
    HEADERS_CLASS = 12,
    MAILBOX_CLASS = 13,
    MAILBOX_LIST_CLASS = 14,
    MEDIA_TYPE_CLASS = 15,
    MESSAGE_CLASS = 16,
    MSG_ID_CLASS = 17,
    NODE_CLASS = 18,
    PARAMETER_CLASS = 19,
    STAMP_CLASS = 20,
    TEXT_CLASS = 21,
    TRANSFER_ENCODING_TYPE_CLASS = 22
};

enum {
    ICU = 1,
    MLANG = 2,
    LIBICONV = 3,
#if defined(WIN32)
    DEFAULT = MLANG
#else
    DEFAULT = LIBICONV
#endif
};

enum {
    HEADER_PARSER_OPTION_STOP = 0,
    HEADER_PARSER_OPTION_SKIP = 1
};

#include "ByteBuffer.h"
#include "CharBuffer.h"
#include "Class.h"
#include "StringRep.h"
//#include "String.h"
#include "hsString.h"
#include "TextUtil.h"
#include "Rfc822Tokenizer.h"
#include "MimeTokenizer.h"
#include "Node.h"
#include "Parameter.h"
#include "FieldBody.h"
#include "MsgId.h"
#include "EncodedWord.h"
#include "Text.h"
#include "Address.h"
#include "Mailbox.h"
#include "AddressList.h"
#include "MailboxList.h"
#include "Group.h"
#include "DateTime.h"
#include "DispositionType.h"
#include "Stamp.h"
#include "TransferEncodingType.h"
#include "MediaType.h"
#include "Field.h"
#include "Headers.h"
#include "Body.h"
#include "Entity.h"
#include "BodyPart.h"
#include "Entity.h"
#include "Message.h"
#include "AppleFile.h"
#include "BinHex.h"
#include "Uuencode.h"
#include "Base64Decoder.h"
#include "Base64Encoder.h"
#include "QuotedPrintableDecoder.h"
#include "QuotedPrintableEncoder.h"
#include "TextDecoder.h"
#include "TextEncoder.h"
#include "TnefDecoder.h"

/// Initializes the library.
MIMEPP_API bool Initialize();
/// Finalizes the library.
MIMEPP_API void Finalize();
/// Sets the maximum depth of nested body parts for the parser.
MIMEPP_API void setMaxDepth(int maxDepth);
/// Sets the selection for the text converter implementation.
MIMEPP_API void setTextConverterImpl(int choice);
/// Sets the option for parsing the entity headers.
MIMEPP_API void setHeaderParserOption(int option);
/// Gets the version information string.
MIMEPP_API const char* versionInfo();
/// Gets the build information string.
MIMEPP_API const char* buildInfo();

} // namespace mimepp

#endif
