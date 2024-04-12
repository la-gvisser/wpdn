//=============================================================================
// DateTime.h
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

#ifndef MIMEPP_DATE_TIME_H
#define MIMEPP_DATE_TIME_H

/// %Class that represents an RFC&nbsp;2822 date-time

class MIMEPP_API DateTime : public FieldBody {

public:

    /// Default constructor
    DateTime();

    /// Copy constructor
    DateTime(const DateTime& other);

    /// Constructor that takes an initial string and parent node
    DateTime(const String& str, Node* parent=0);

    /// Destructor
    virtual ~DateTime();

    /// Assignment operator
    const DateTime& operator = (const DateTime& other);

    /// Parses the string representation
    virtual void parse();

    /// Assembles the string representation
    virtual void assemble();

    /// Creates a copy of this object
    virtual Node* clone() const;

    /// Gets the date and time as a UNIX (POSIX) time
    int32_t asUnixTime() const;

    /// Sets the date and time from a UNIX (POSIX) time
    void fromUnixTime(int32_t time, bool convertToLocal=true);

    /// Gets the date and time as a system-dependent calendar time
    time_t asCalendarTime() const;

    /// Sets the date and time from a system-dependent calendar time
    void fromCalendarTime(int32_t time, bool convertToLocal=true);

    /// Sets values, excluding time zone
    void setValuesLocal(int year, int month, int day, int hour, int minute,
        int second);

    /// Sets values, including time zone
    void setValuesLiteral(int year, int month, int day, int hour, int minute,
        int second, int zoneOffset, const char* zoneName=0);

    /// Gets the year
    int year() const;

    /// Gets the month
    int month() const;

    /// Gets the day of the month
    int day() const;

    /// Gets the hour
    int hour() const;

    /// Gets the minute
    int minute() const;

    /// Gets the second
    int second() const;

    /// Gets the time zone
    int zone() const;

    /// Gets the name of the time zone
    const String& zoneName() const;

    /// Gets the day of the week
    int dayOfTheWeek() const;

    /// Creates a new instance
    static DateTime* newDateTime();

    /// Provides a class factory hook
    static DateTime* (*sNewDateTime)();

protected:

    /// Sets the date and time from a UNIX (POSIX) time
    void _fromUnixTime(int32_t time, bool convertToLocal);

    /// Sets the date and time from a system-dependent calendar time
    void _fromCalendarTime(int32_t time, bool convertToLocal);

    int mYear;
    int mMonth;
    int mDay;
    int mHour;
    int mMinute;
    int mSecond;
    int mZone;
    String mZoneName;

private:

    // Initialization code common to all constructors.
    void dateTime_Init();

};

#endif
