//=============================================================================
// Uuencode.h
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

#ifndef MIMEPP_UUENCODE_H
#define MIMEPP_UUENCODE_H

/// %Class for performing uuencode or uudecode operations

class MIMEPP_API Uuencode {

public:

    /// Default constructor
    Uuencode();

    /// Destructor
    virtual ~Uuencode();

    /// Sets the file name
    void setFileName(const char* name);

    /// Gets the file name
    const char* fileName() const;

    /// Sets the file mode
    void setFileMode(uint16_t mode);

    /// Gets the file mode
    uint16_t fileMode() const;

    /// Sets the binary file content
    void setBinaryChars(const String& bytes);

    /// Gets the binary file content
    const String& binaryChars() const;

    /// Sets the encoded characters
    void setAsciiChars(const String& chars);

    /// Gets the encoded characters
    const String& asciiChars() const;

    /// Performs the encode operation
    void encode();

    /// Performs the decode operation
    int decode();

private:

    uint16_t mFileMode;
    String mFileName;
    String mBytes;
    String mAsciiChars;
   
};

#endif
