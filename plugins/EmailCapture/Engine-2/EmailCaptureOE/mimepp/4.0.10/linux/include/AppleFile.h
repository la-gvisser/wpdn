//=============================================================================
// AppleFile.h
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

#ifndef MIMEPP_APPLE_FILE_H
#define MIMEPP_APPLE_FILE_H

/// %Class for packing and unpacking AppleSingle and AppleDouble data

class MIMEPP_API AppleFile {

public:

    enum Error {
        NO_ERROR_ = 0,
        TOO_SHORT,
        BAD_MAGIC_NUMBER,
        WRONG_VERSION,
        CORRUPT_DATA
    };

    enum FormatType {
        APPLE_SINGLE,
        APPLE_DOUBLE_HEADER
    };

    /// Default constructor.
    AppleFile();

    /// Gets the format type
    int format() const;

    /// Sets the format type
    void setFormat(int formatType);

    /// Returns true if the %AppleFile data contains a file name
    bool hasFileName() const;

    /// Gets the file name
    const char* fileName() const;

    /// Sets the file name
    void setFileName(const char* name);

    /// Returns true if the %AppleFile data contains a file times entry
    bool hasFileTimes() const;

    /// Gets the create time
    int32_t createTime() const;

    /// Sets the create time
    void setCreateTime(int32_t time);

    /// Gets the modify time
    int32_t modifyTime() const;

    /// Sets the modify time
    void setModifyTime(int32_t time);

    /// Gets the backup time
    int32_t backupTime() const;

    /// Sets the backup time
    void setBackupTime(int32_t time);

    /// Gets the access time
    int32_t accessTime() const;

    /// Sets the access time
    void setAccessTime(int32_t time);

    /// Returns true if the %AppleFile data contains a data fork entry
    bool hasDataFork() const;

    /// Gets the data fork
    const String& dataFork() const;

    /// Sets the data fork
    void setDataFork(const String& bytes);

    /// Returns true if the %AppleFile data contains a resource fork entry
    bool hasResourceFork() const;

    /// Gets the resource fork
    const String& resourceFork() const;

    /// Sets the resource fork
    void setResourceFork(const String& bytes);

    /// Gets the buffer of packed file data.
    const String& packedData() const;

    /// Sets the buffer of packed file data.
    void setPackedData(const String& str);

    /// Packs the file data into a single buffer
    void pack();

    /// Unpacks the packed file data
    int unpack();

    /// Converts from Unix time to %AppleFile time
    static int32_t unixTimeToAppleFileTime(uint32_t unixTime);

    /// Converts from %AppleFile time to Unix time
    static uint32_t appleFileTimeToUnixTime(int32_t afTime);

private:

    enum {
        kDataFork          =  1,
        kResourceFork      =  2,
        kRealName          =  3,
        kComment           =  4,
        kIconBW            =  5,
        kIconColor         =  6,
        kIconColour        =  6,
        kFileDatesInfo     =  8,
        kFinderInfo        =  9,
        kMacintoshFileInfo = 10,
        kProDosFileInfo    = 11,
        kMsDosFileInfo     = 12,
        kShortName         = 13
    };

    int isValid();
    int getFileDatesInfo();
    int getRealName();
    int getResourceFork();
    int getDataFork();

    int mNumEntries;

    bool   mHasRealName;
    bool   mHasFileDatesInfo;
    bool   mHasResourceFork;
    bool   mHasDataFork;

    int      mErrorCode;

    int      mFormatType;
    int32_t  mCreateTime;
    int32_t  mModifyTime;
    int32_t  mBackupTime;
    int32_t  mAccessTime;
    String mRealName;
    String mResourceFork;
    String mDataFork;

    String mAppleFileChars;
};

#endif
