//=============================================================================
// BinHex.h
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

#ifndef DW_BINHEX_H
#define DW_BINHEX_H

/// %Class for converting files to or from %BinHex 4.0 format

class MIMEPP_API BinHex {

public:

    /// Default constructor
    BinHex();

    /// Destructor
    virtual ~BinHex();

    /// Resets to the initial state
    void initialize();

    /// Gets the file name
    const char* fileName() const;

    /// Sets the file name
    void setFileName(const char* name);

    /// Gets the file type
    void fileType(char* buf) const;

    /// Sets the file type
    void setFileType(const char* type);

    /// Gets the file creator
    void fileCreator(char* buf) const;

    /// Sets the file creator
    void setFileCreator(const char* creator);

    /// Gets the first byte of the Macintosh Finder flags
    uint8_t flag1() const;

    /// Sets the first byte of the Macintosh Finder flags
    void setFlag1(uint8_t flag);

    /// Gets the second byte of the Macintosh Finder flags
    uint8_t flag2() const;

    /// Sets the second byte of the Macintosh Finder flags
    void setFlag2(uint8_t flag);

    /// Gets the data fork for the file
    const String& dataFork() const;

    /// Sets the data fork for the file
    void setDataFork(const String& bytes);

    /// Gets the resource fork for the file
    const String& resourceFork() const;

    /// Sets the resource fork for the file
    void setResourceFork(const String& bytes);

    /// Gets the characters of the %BinHex encoded file
    const String& binhexChars() const;

    /// Sets the characters of the %BinHex encoded file
    void setBinHexChars(const String& chars);

    /// Converts the Macintosh file information to %BinHex format
    void encode();

    /// Converts the Macintosh file information from %BinHex format
    int decode();
   
private:

    char mFileName[64];
    char mFileType[8];
    char mFileCreator[8];
    uint8_t mFlag1;
    uint8_t mFlag2;
    String mDataFork;
    String mResourceFork;
    String mBinHexChars;

};

#endif
