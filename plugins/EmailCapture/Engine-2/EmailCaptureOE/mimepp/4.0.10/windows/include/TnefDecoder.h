//=============================================================================
// TnefDecoder.h
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

#ifndef MIMEPP_TNEF_DECODER_H
#define MIMEPP_TNEF_DECODER_H

/// %Class that represents a TNEF attachment.

class MIMEPP_API TnefAttachment {

public:

    /// Default constructor
    TnefAttachment();

    /// Copy constructor
    TnefAttachment(const TnefAttachment&);

    /// Constructor that takes an initial value
    TnefAttachment(const mimepp::String& fileName,
        const mimepp::DateTime& modificationTime,
        const mimepp::String& content);

    /// Destructor
    ~TnefAttachment();

    /// Assignment operator
    TnefAttachment& operator=(const TnefAttachment&);

    /// Gets the attachment file name
    const mimepp::String fileName() const;

    /// Gets the attachment modification time
    const mimepp::DateTime& modificationTime() const;

    /// Gets the content of the attachment
    const mimepp::String& content() const;

private:

    mimepp::String mFileName;
    mimepp::DateTime mModificationTime;
    mimepp::String mContent;

};

/// Utility class that performs TNEF attachment decoding.

class MIMEPP_API TnefDecoder {

public:

    /// Constructor that takes an initial string
    TnefDecoder(mimepp::String str);

    /// Destructor
    ~TnefDecoder();

    /// Parses the string representation
    int parse();

    /// Gets the number of attachments
    unsigned numAttachments() const;

    /// Gets the attachment at the specified position
    TnefAttachment& attachmentAt(unsigned index) const;

private:

    mimepp::String mString;
    TnefAttachment** mAttachments;
    unsigned mNumAttachments;
    unsigned mAttachmentsSize;

    void TnefDecoder_Init();
    int parseAttributes(size_t pos);
    void _copyAttachments(const TnefDecoder& dec);
    void _addAttachment(TnefAttachment* att);
    void _insertAttachmentAt(unsigned index, TnefAttachment* att);
    TnefAttachment* _removeAttachmentAt(unsigned index);
    void _deleteAllAttachments();

};

#endif
