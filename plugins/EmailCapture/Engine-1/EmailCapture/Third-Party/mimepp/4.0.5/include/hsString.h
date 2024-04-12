//=============================================================================
// hsString.h
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
// WildPackets - renamed hsString.h from String.h for VS2008
//=============================================================================

#ifndef DW_STRING_H
#define DW_STRING_H

class MIMEPP_API String {

public:

    /// Assigned the value (size_t)-1
    static const size_t npos;

    /// Default constructor
    String();

    /// Copy constructor
    String(const String& str, size_t pos=0, size_t len=npos);

    /// Constructor that takes a buffer intializer
    String(const char* buf, size_t len);

    /// Constructor that takes a NUL-terminated C string initializer
    String(const char* cstr);

    /// Constructor that takes a single character initializer
    String(size_t len, char ch);

    /// Constructor that takes a <b>%StringRep</b> initializer
    String(StringRep* rep, size_t offset, size_t length);

    /// Destructor
    virtual ~String();

    /// Assignment operator
    String& operator = (const String& str);

    /// Assignment operator
    String& operator = (const char* cstr);

    /// Assignment operator
    String& operator = (char ch);

    /// Returns the length of the string
    size_t size() const;

    /// Returns the length of the string
    size_t length() const;

    /// Returns the maximum length of the string
    size_t max_size() const;

    /// Changes the length of the string
    void resize(size_t len, char ch);

    /// Changes the length of the string
    void resize(size_t len);

    /// Returns the size of the internal buffer
    size_t capacity() const;

    /// Reserves internal buffer capacity
    void reserve(size_t size);

    /// Sets the string to the empty string
    void clear();

    /// Returns true if the string is empty
    bool empty() const;

    /// Returns the char at the specified position
    const char& operator [] (size_t pos) const;

    /// Returns the char at the specified position
    char& operator [] (size_t pos);

    /// Returns the char at the specified position
    const char& at(size_t pos) const;

    /// Returns the char at the specified position
    char& at(size_t pos);

    /// Appends a string
    String& operator += (const String& str);

    /// Appends a NUL-terminated C string
    String& operator += (const char* cstr);

    /// Appends a single character
    String& operator += (char ch);

    /// Appends a string
    String& append(const String& str);

    /// Appends a substring
    String& append(const String& str, size_t pos, size_t len);

    /// Appends a buffer
    String& append(const char* buf, size_t len);

    /// Appends a NUL-terminated C string
    String& append(const char* cstr);

    /// Appends a single character
    String& append(size_t len, char ch);

    /// Assigns from a string
    String& assign(const String& str);

    /// Assigns from a substring
    String& assign(const String& str, size_t pos, size_t len);

    /// Assigns from a buffer
    String& assign(const char* buf, size_t len);

    /// Assigns from a NUL-terminated C string
    String& assign(const char* cstr);

    /// Assigns from a single character
    String& assign(size_t len, char ch);

    /// Inserts from a string
    String& insert(size_t pos, const String& str);

    /// Inserts from a substring
    String& insert(size_t pos1, const String& str, size_t pos2,
        size_t len);

    /// Inserts from a buffer
    String& insert(size_t pos, const char* buf, size_t len);

    /// Inserts from a NUL-terminated C string
    String& insert(size_t pos, const char* cstr);

    /// Inserts from a repeated character
    String& insert(size_t pos, size_t len, char ch);

    /// Erases characters
    String& erase(size_t pos=0, size_t len=npos);

    /// Replaces characters
    String& replace(size_t pos1, size_t len1, const String& str);

    /// Replaces characters
    String& replace(size_t pos1, size_t len1, const String& str,
        size_t pos2, size_t len2);

    /// Replaces characters
    String& replace(size_t pos1, size_t len1, const char* buf,
        size_t len2);

    /// Replaces characters
    String& replace(size_t pos1, size_t len1, const char* cstr);

    /// Replaces characters
    String& replace(size_t pos1, size_t len1, size_t len2, char ch);

    /// Copies characters from a buffer to this string
    size_t copy(char* buf, size_t len, size_t pos=0) const;

    /// Swaps the contents of this string and another string
    void swap(String& str);

    /// Returns a pointer to the internal buffer (NUL-terminated)
    const char* c_str() const;

    /// Returns a pointer to the internal buffer (not NUL-terminated)
    const char* data() const;

    /// Searches for a string
    size_t find(const String& str, size_t pos=0) const;

    /// Searches for a string
    size_t find(const char* buf, size_t pos, size_t len) const;

    /// Searches for a string
    size_t find(const char* cstr, size_t pos=0) const;

    /// Searches for a single character
    size_t find(char ch, size_t pos=0) const;

    /// Searches backward for a string
    size_t rfind(const String& str, size_t pos=npos) const;

    /// Searches backward for a string
    size_t rfind(const char* buf, size_t pos, size_t len) const;

    /// Searches backward for a string
    size_t rfind(const char* cstr, size_t pos=npos) const;

    /// Searches backward for a single character
    size_t rfind(char ch, size_t pos=npos) const;

    /// Searches forward for a matching character
    size_t find_first_of(const String& str, size_t pos=0) const;

    /// Searches forward for a matching character
    size_t find_first_of(const char* buf, size_t pos, size_t len) const;

    /// Searches forward for a matching character
    size_t find_first_of(const char* cstr, size_t pos=0) const;

    /// Searches backward for a matching character
    size_t find_last_of(const String& str, size_t pos=npos) const;

    /// Searches backward for a matching character
    size_t find_last_of(const char* buf, size_t pos, size_t len) const;

    /// Searches backward for a matching character
    size_t find_last_of(const char* cstr, size_t pos=npos) const;

    /// Searches forward for a non-matching character
    size_t find_first_not_of(const String& str, size_t pos=0) const;

    /// Searches forward for a non-matching character
    size_t find_first_not_of(const char* buf, size_t pos, size_t len) const;

    /// Searches forward for a non-matching character
    size_t find_first_not_of(const char* cstr, size_t pos=0) const;

    /// Searches backward for a non-matching character
    size_t find_last_not_of(const String& str, size_t pos=npos) const;

    /// Searches backward for a non-matching character
    size_t find_last_not_of(const char* buf, size_t pos, size_t len) const;

    /// Searches backward for a non-matching character
    size_t find_last_not_of(const char* cstr, size_t pos=npos) const;

    /// Returns a substring of this string
    String substr(size_t pos=0, size_t len=npos) const;

    /// Performs a string comparison
    int compare(const String& str) const;

    /// Performs a string comparison
    int compare(size_t pos1, size_t len1, const String& str) const;

    /// Performs a string comparison
    int compare(size_t pos1, size_t len1, const String& str,
        size_t pos2, size_t len2) const;

    /// Performs a string comparison
    int compare(const char* cstr) const;

    /// Performs a string comparison
    int compare(size_t pos1, size_t len1, const char* buf) const;

    /// Performs a string comparison
    int compare(size_t pos1, size_t len1, const char* buf, 
        size_t len2) const;

    // Non-ANSI member functions

    /// Converts all characters to lower case
    void convertToLowerCase();

    /// Converts all characters to upper case
    void convertToUpperCase();

    /// Removes space characters from the beginning and the end
    void trim();

private:

    StringRep* mRep;
    size_t mStart;
    size_t mLength;

    void _copy();
    void _replace(size_t pos1, size_t len1, const char* buf, size_t len2);
    void _replace(size_t pos1, size_t len1, size_t len2, char ch);

};

MIMEPP_API String operator + (const String& str1, const String& str2);
MIMEPP_API String operator + (const char* cstr, const String& str2);
MIMEPP_API String operator + (char ch, const String& str2);
MIMEPP_API String operator + (const String& str1, const char* cstr);
MIMEPP_API String operator + (const String& str1, char ch);

MIMEPP_API bool operator == (const String& str1, const String& str2);
MIMEPP_API bool operator == (const String& str1, const char* cstr);
MIMEPP_API bool operator == (const char* cstr, const String& str2);

MIMEPP_API bool operator != (const String& str1, const String& str2);
MIMEPP_API bool operator != (const String& str1, const char* cstr);
MIMEPP_API bool operator != (const char* cstr, const String& str2);

MIMEPP_API bool operator < (const String& str1, const String& str2);
MIMEPP_API bool operator < (const String& str1, const char* cstr);
MIMEPP_API bool operator < (const char* cstr, const String& str2);

MIMEPP_API bool operator > (const String& str1, const String& str2);
MIMEPP_API bool operator > (const String& str1, const char* cstr);
MIMEPP_API bool operator > (const char* cstr, const String& str2);

MIMEPP_API bool operator <= (const String& str1, const String& str2);
MIMEPP_API bool operator <= (const String& str1, const char* cstr);
MIMEPP_API bool operator <= (const char* cstr, const String& str2);

MIMEPP_API bool operator >= (const String& str1, const String& str2);
MIMEPP_API bool operator >= (const String& str1, const char* cstr);
MIMEPP_API bool operator >= (const char* cstr, const String& str2);

MIMEPP_API int Strcasecmp(const String& str1, const String& str2);
MIMEPP_API int Strcasecmp(const String& str, const char* cstr);
MIMEPP_API int Strcasecmp(const char* cstr, const String& str);

MIMEPP_API int Strncasecmp(const String& str1, const String& str2, size_t len);
MIMEPP_API int Strncasecmp(const String& str, const char* cstr, size_t len);
MIMEPP_API int Strncasecmp(const char* cstr, const String& str, size_t len);

MIMEPP_API int Strcmp(const String& str1, const String& str2);
MIMEPP_API int Strcmp(const String& str, const char* cstr);
MIMEPP_API int Strcmp(const char* cstr, const String& str);

MIMEPP_API int Strncmp(const String& str1, const String& str2, size_t len);
MIMEPP_API int Strncmp(const String& str, const char* cstr, size_t len);
MIMEPP_API int Strncmp(const char* cstr, const String& str, size_t len);

MIMEPP_API void Strcpy(String& strDest, const String& strSrc);
MIMEPP_API void Strcpy(String& strDest, const char* cstrSrc);
MIMEPP_API void Strcpy(char* cstrDest, const String& strSrc);

MIMEPP_API void Strncpy(String& strDest, const String& strSrc, size_t len);
MIMEPP_API void Strncpy(String& strDest, const char* cstrSrc, size_t len);
MIMEPP_API void Strncpy(char* cstrDest, const String& strSrc, size_t len);

MIMEPP_API char* Strdup(const String& str);

#endif
