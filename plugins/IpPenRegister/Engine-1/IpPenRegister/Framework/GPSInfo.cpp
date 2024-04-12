// =============================================================================
//	GPSInfo.cpp
// =============================================================================
//	Copyright (c) 2004-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "GPSInfo.h"
#include <math.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>


#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

using namespace std;

using std::string;
using std::wstring;
using std::wostringstream;
using std::setiosflags;
using std::setprecision;

const bool		g_bShowCautioned = false;


// =============================================================================
//	CGPSInfo
// =============================================================================

// -----------------------------------------------------------------------------
//		Compare
// -----------------------------------------------------------------------------

bool
CGPSInfo::Compare(
	const CGPSInfo&	inInfo ) const
{
	UInt64	nFlags;
	UInt64	nInFlags;
	GetFlags( nFlags );
	inInfo.GetFlags( nInFlags );
	if ( nInFlags != nFlags ) return false;

	bool	bHave;
	bool	bInHave;
	double	dValue;
	double	dInValue;

	bHave = GetLatitude( dValue );
	bInHave = inInfo.GetLatitude( dInValue );
	// Is the presence of each value the same?
	if ( bInHave != bHave ) return false;
	// If the value is present, then compare them.
	if ( bHave ) {
		// If they are different then return false.
		// Otherwise check the next value.
		if ( dInValue != dValue ) return false;
	}

	bHave = GetLongitude( dValue );
	bInHave = inInfo.GetLongitude( dInValue );
	if ( bInHave != bHave ) return false;
	if ( bHave ) {
		if ( dInValue != dValue ) return false;
	}

	bHave = GetAltitude( dValue );
	bInHave = inInfo.GetAltitude( dInValue );
	if ( bInHave != bHave ) return false;
	if ( bHave ) {
		if ( dInValue != dValue ) return false;
	}

	bHave = GetSpeed( dValue );
	bInHave = inInfo.GetSpeed( dInValue );
	if ( bInHave != bHave ) return false;
	if ( bHave ) {
		if ( dInValue != dValue ) return false;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		Format
// -----------------------------------------------------------------------------

wstring
CGPSInfo::Format() const
{
	wostringstream	ssFormat;
	ssFormat <<
		"LAT " << GetLatitude()  << ", " <<
		"LON " << GetLongitude() << ", " <<
		"ALT " << GetAltitude()  << ", " <<
		"SPD " << GetSpeed();

	return ssFormat.str();
}


// -----------------------------------------------------------------------------
//		GetAltitude
// -----------------------------------------------------------------------------

wstring
CGPSInfo::GetAltitude() const
{
	bool	bCautioned = IsCautioned() && g_bShowCautioned;

	wstring	strAltitude;
	if ( (m_GpsInfo.fFlags & kWpGps_Valid_Altitude) && !bCautioned ) {
		wostringstream ossAltitude;
		// %01.2f
		ossAltitude << setiosflags(ios::fixed) << setprecision(2) << m_GpsInfo.fAltitude;
	}
	else {
		strAltitude = L"N/A";
	}

	return strAltitude;
}

bool
CGPSInfo::GetAltitude(
	double& outAltitude ) const
{
	bool	bCautioned = IsCautioned() && g_bShowCautioned;

	if ( !(m_GpsInfo.fFlags & kWpGps_Valid_Altitude) || bCautioned ) {
		return false;
	}

	outAltitude = m_GpsInfo.fAltitude;
	return true;
}

bool
CGPSInfo::GetAltitude(
	SInt32& outAltitude ) const
{
	bool	bCautioned = IsCautioned() && g_bShowCautioned;

	if ( !(m_GpsInfo.fFlags & kWpGps_Valid_Altitude) || bCautioned ) {
		return false;
	}

	outAltitude = (INT32) m_GpsInfo.fAltitude;
	return true;
}


// -----------------------------------------------------------------------------
//		GetFlags
// -----------------------------------------------------------------------------

wstring
CGPSInfo::GetFlags() const
{
	wstring	strFlags;
	if ( IsCautioned() ) {
		strFlags = L"W";
	}

	return strFlags;
}


// -----------------------------------------------------------------------------
//		GetLatitude
// -----------------------------------------------------------------------------

wstring
CGPSInfo::GetLatitude() const
{
	bool	bCautioned = IsCautioned() && g_bShowCautioned;

	wstring	strLatitiude;
	if ( (m_GpsInfo.fFlags & kWpGps_Valid_Latitude) && !bCautioned ) {
		double	fDegrees = 0;
		double	fMinutes = 0;

		fMinutes = modf( fabs( m_GpsInfo.fLatitude ), &fDegrees );
		fMinutes *= 60;

		wstring strCompass = (m_GpsInfo.fLatitude >= 0) ? L"N" : L"S";

		wostringstream ossLatitude;
		if ( fMinutes < 10 ) {

			ossLatitude <<
				strCompass <<
				setiosflags(wios::fixed) << setprecision(0) << fDegrees <<
				L"°0" <<
				setiosflags(wios::fixed) << setprecision(4) << fMinutes;

			//_stprintf(
			//	szText,
			//	_T("%c %1.0f°0%.4f"),
			//	,
			//	fDegrees,
			//	fMinutes );
		}
		else {
			ossLatitude <<
				strCompass <<
				setiosflags(wios::fixed) << setprecision(0) << fDegrees <<
				L"°" <<
				setiosflags(wios::fixed) << setprecision(4) << fMinutes;

			//_stprintf(
			//	szText,
			//	_T("%c %1.0f°%.4f"),
			//	(m_GpsInfo.fLatitude >= 0) ? _T('N') : _T('S'),
			//	fDegrees,
			//	fMinutes );
		}
		strLatitiude = ossLatitude.str();
	}
	else {
		strLatitiude = L"N/A";
	}

	return strLatitiude;
}

bool
CGPSInfo::GetLatitude(
	double& outLatitude ) const
{
	bool	bCautioned = IsCautioned() && g_bShowCautioned;

	if ( !(m_GpsInfo.fFlags & kWpGps_Valid_Latitude) || bCautioned ) {
		return false;
	}

	outLatitude = m_GpsInfo.fLatitude;
	return true;
}

bool
CGPSInfo::GetLatitude(
	SInt32& outLatitude ) const
{
	bool	bCautioned = IsCautioned() && g_bShowCautioned;

	if ( !(m_GpsInfo.fFlags & kWpGps_Valid_Latitude) || bCautioned ) {
		return false;
	}

	outLatitude = static_cast<SInt32>( m_GpsInfo.fLatitude );
	return true;
}


// -----------------------------------------------------------------------------
//		GetLatitudeDirection
// -----------------------------------------------------------------------------

bool
CGPSInfo::GetLatitudeDirection(
	wchar_t& outDirection ) const
{
	bool	bCautioned = IsCautioned() && g_bShowCautioned;

	if ( !(m_GpsInfo.fFlags & kWpGps_Valid_Latitude) || bCautioned ) {
		return false;
	}

	outDirection = (m_GpsInfo.fLatitude >= 0) ? 'N' : 'S';
	return true;
}


// -----------------------------------------------------------------------------
//		GetLongitude
// -----------------------------------------------------------------------------

wstring
CGPSInfo::GetLongitude() const
{
	bool	bCautioned = IsCautioned() && g_bShowCautioned;

	wstring	strLongitude;
	if ( (m_GpsInfo.fFlags & kWpGps_Valid_Longitude) && !bCautioned ) {
		double	fDegrees = 0;
		double	fMinutes = 0;

		fMinutes = modf( fabs( m_GpsInfo.fLongitude ), &fDegrees );
		fMinutes *= 60;

		wstring	strCompass = (m_GpsInfo.fLatitude >= 0) ? L"E" : L"W";

		wostringstream	ossLongitude;
		if ( fMinutes < 10 ) {
			ossLongitude <<
				strCompass <<
				std::setiosflags(wios::fixed) << std::setprecision(0) << fDegrees <<
				L"°0" <<
				setiosflags(wios::fixed) << setprecision(4) << fMinutes;

			//_stprintf(
			//	szText,
			//	_T("%c %1.0f°0%.4f"),
			//	(m_GpsInfo.fLongitude >= 0) ? _T('E') : _T('W'),
			//	fDegrees,
			//	fMinutes );
		}
		else {
			ossLongitude <<
				strCompass <<
				setiosflags(wios::fixed) << setprecision(0) << fDegrees <<
				L"°" <<
				setiosflags(wios::fixed) << setprecision(4) << fMinutes;

			//_stprintf(
			//	szText,
			//	_T("%c %1.0f°%.4f"),
			//	(m_GpsInfo.fLongitude >= 0) ? _T('E') : _T('W'),
			//	fDegrees,
			//	fMinutes );
		}
		strLongitude = ossLongitude.str();
	}
	else {
		strLongitude = L"N/A";
	}

	return strLongitude;
}

bool
CGPSInfo::GetLongitude(
	double& outLongitude ) const
{
	bool	bCautioned = IsCautioned() && g_bShowCautioned;

	if ( !(m_GpsInfo.fFlags & kWpGps_Valid_Longitude) || bCautioned ) {
		return false;
	}
	outLongitude = m_GpsInfo.fLongitude;
	return true;
}

bool
CGPSInfo::GetLongitude(
	SInt32& outLongitude ) const
{
	bool	bCautioned = IsCautioned() && g_bShowCautioned;

	if ( !(m_GpsInfo.fFlags & kWpGps_Valid_Longitude) || bCautioned ) {
		return false;
	}
	outLongitude = static_cast<SInt32>( m_GpsInfo.fLongitude );
	return true;
}


// -----------------------------------------------------------------------------
//		GetLongitudeDirection
// -----------------------------------------------------------------------------

bool
CGPSInfo::GetLongitudeDirection(
	wchar_t& outDirection ) const
{
	bool	bCautioned = IsCautioned() && g_bShowCautioned;

	if ( !(m_GpsInfo.fFlags & kWpGps_Valid_Longitude) || bCautioned ) {
		return false;
	}

	outDirection = (m_GpsInfo.fLongitude >= 0) ? 'E' : 'W';
	return true;
}


// -----------------------------------------------------------------------------
//		GetSpeed
// -----------------------------------------------------------------------------

wstring
CGPSInfo::GetSpeed() const
{
	bool	bCautioned = IsCautioned() && g_bShowCautioned;

	wstring	strSpeed;
	if ( (m_GpsInfo.fFlags & kWpGps_Valid_Speed) && !bCautioned ) {
		wostringstream ossSpeed;

		ossSpeed << setiosflags(ios::fixed) << setprecision(2) << m_GpsInfo.fSpeed;
		strSpeed = ossSpeed.str();

		//_stprintf(
		//	szText,
		//	_T("%1.2f"),
		//	m_GpsInfo.fSpeed );
	}
	else {
		strSpeed = L"N/A";
	}

	return strSpeed;
}


// -----------------------------------------------------------------------------
//		GetSpeed
// -----------------------------------------------------------------------------

bool
CGPSInfo::GetSpeed(
	double& outSpeed ) const
{
	bool	bCautioned = IsCautioned() && g_bShowCautioned;

	if ( !(m_GpsInfo.fFlags & kWpGps_Valid_Speed) || bCautioned ) {
		return false;
	}

	outSpeed = m_GpsInfo.fSpeed;
	return true;
}


// -----------------------------------------------------------------------------
//		GetSpeed
// -----------------------------------------------------------------------------

bool
CGPSInfo::GetSpeed(
	SInt32& outSpeed ) const
{
	bool	bCautioned = IsCautioned() && g_bShowCautioned;

	if ( !(m_GpsInfo.fFlags & kWpGps_Valid_Speed) || bCautioned ) {
		return false;
	}

	outSpeed = static_cast<SInt32>( m_GpsInfo.fSpeed );
	return true;
}


// -----------------------------------------------------------------------------
//		GetTime
// -----------------------------------------------------------------------------
//TODO: get actual time
wstring
CGPSInfo::GetTime(
	int		/*inFormat*/,
	bool	/*inLocalTime*/ ) const
{
	bool	bCautioned = IsCautioned() && g_bShowCautioned;

	wstring	strFormat;
	if ( (m_GpsInfo.fFlags & kWpGps_Valid_GPSTime) && !bCautioned ) {
		// Convert the time stamp to a date.
		wstring	strDate = L"Date";
		//TimeStampFormat::TimeStampToDateString(
		//	m_GpsInfo.fGpsTime, szDate, TEXT_BUFFSIZE, bLocalTime );

		// Convert the timestamp to absolute time.
		wstring	strTime = L"Time";
		//TimeStampFormat::TimeStampToTimeString(
		//	m_GpsInfo.fGpsTime, 
		//	inFormat, szTime, TEXT_BUFFSIZE, bLocalTime );

		strFormat = strDate + L" " + strTime;
	}
	else {
		strFormat = L"N/A";
	}

	return strFormat;
}


// -----------------------------------------------------------------------------
//		GetTime
// -----------------------------------------------------------------------------

bool
CGPSInfo::GetTime(
	UInt64&	outTime ) const
{
	bool	bCautioned = IsCautioned() && g_bShowCautioned;

	if ( !(m_GpsInfo.fFlags & kWpGps_Valid_GPSTime) || bCautioned ) {
		return false;
	}

	outTime = m_GpsInfo.fGpsTime;
	return true;
}


// -----------------------------------------------------------------------------
//		TextHeadings		[static]
// -----------------------------------------------------------------------------

void
CGPSInfo::TextHeadings(
	std::wstring&	outHeading,
	wchar_t			inDelimiter )
{
	wstring	strTime;
	wstring	strFlags;
	wstring	strLatitude;
	wstring	strLongitude;
	wstring	strAltitude;
	wstring	strSpeed;

	//strTime.LoadString( IDS_PACKET_LIST_GPSTIME );
	//strFlags.LoadString( IDS_PACKET_LIST_FLAGS );
	//strLatitude.LoadString( IDS_PACKET_LIST_LATITUDE );
	//strLongitude.LoadString( IDS_PACKET_LIST_LONGITUDE );
	//strAltitude.LoadString( IDS_PACKET_LIST_ALTITUDE );
	//strSpeed.LoadString( IDS_PACKET_LIST_SPEED );

	wstring	strDelimiter;
	strDelimiter.resize( 1 );
	strDelimiter[0] = inDelimiter;

	wostringstream	ssHeading;
	ssHeading <<
		strTime << strDelimiter <<
		strFlags << strDelimiter <<
		strLatitude << strDelimiter <<
		strLongitude << strDelimiter <<
		strAltitude << strDelimiter <<
		strSpeed;

	outHeading = ssHeading.str();

	//inHeading.Format(
	//	_T("%s%c%s%c%s%c%s%c%s%c%s\r\n"),
	//	strTime, inDelimiter,
	//	strFlags, inDelimiter,
	//	strLatitude, inDelimiter,
	//	strLongitude, inDelimiter,
	//	strAltitude, inDelimiter,
	//	strSpeed );
}


#if (0)
// -----------------------------------------------------------------------------
//		ToText
// -----------------------------------------------------------------------------

void
CGPSInfo::ToText(
	CByteVector&	inStream,
	wchar_t			inDelimiter,
	int				inFormat,
	bool			bLocalTime ) const
{
	wchar_t	szTime[32] = { 0 };
	wchar_t	szLatitude[32] = { 0 };
	wchar_t	szLongitude[32] = { 0 };
	wchar_t	szAltitude[32] = { 0 };
	wchar_t	szSpeed[32] = { 0 };

	GetTime( inFormat, szTime, bLocalTime );
	GetLatitude( szLatitude );
	GetLongitude( szLongitude );
	GetAltitude( szAltitude );
	GetSpeed( szSpeed );

	CString	strText;
	strText.Format(
		_T("%s%c%s%c%s%c%s%c%s\r\n"),
		szTime, inDelimiter,
		szLatitude, inDelimiter,
		szLongitude, inDelimiter,
		szAltitude, inDelimiter,
		szSpeed );

	// The degree's character is an issue...
	// inStream.Write( CT2UTF8( strText ) );
	inStream.Write( CT2CA( strText ) );
}
#endif

// -----------------------------------------------------------------------------
//		GetRecordSize
// -----------------------------------------------------------------------------

UInt32
CGPSInfo::GetRecordSize()
{
	UInt32	nRecordSize = 0;

	nRecordSize += sizeof( UInt64 );		// Peek TimeStamp
	nRecordSize += sizeof( UInt32 );		// Flags
	nRecordSize += sizeof( UInt64 );		// GPS Time
	nRecordSize += sizeof( double ) * 2;	// Latitude, Longitude
	nRecordSize += sizeof( float ) * 2;		// Altitude, Speed as float (not double)

	return nRecordSize;
}


// -----------------------------------------------------------------------------
//		Load
// -----------------------------------------------------------------------------

bool
CGPSInfo::Load(
	size_t			inLength,
	const UInt8*	inData )
{
	// UInt64	TimeStamp
	// UInt32	Flags
	// UInt64	GPS Time
	// double	Latitude
	// double	Longitude
	// float	Altitude
	// float	Speed

	if ( inLength < GetRecordSize() ) return false;

	const UInt8*	pData = inData;

	m_GpsInfo.fTimeStamp = *(reinterpret_cast<const UInt64*>( pData ));
	pData += sizeof( UInt64 );

	m_GpsInfo.fFlags = *(reinterpret_cast<const UInt32*>( pData ));
	pData += sizeof( UInt32 );

	m_GpsInfo.fGpsTime = *(reinterpret_cast<const UInt64*>( pData ));
	pData += sizeof( UInt64 );

	m_GpsInfo.fLatitude = *(reinterpret_cast<const double*>( pData ));
	pData += sizeof( double );

	m_GpsInfo.fLongitude = *(reinterpret_cast<const double*>( pData ));
	pData += sizeof( double );

	m_GpsInfo.fAltitude = *(reinterpret_cast<const float*>( pData ));
	pData += sizeof( float );

	m_GpsInfo.fSpeed = *(reinterpret_cast<const float*>( pData ));
	pData += sizeof( float );

	return true;
}


// -----------------------------------------------------------------------------
//		Write
// -----------------------------------------------------------------------------

bool
CGPSInfo::Write(
	CByteVector& /*outData*/ )
{
#if (0)
	bool	bResult;
	UInt32	nValue;
	float	fValue;

	// UInt64	TimeStamp
	// UInt32	Flags
	// UInt64	GPS Time
	// double	Latitude
	// double	Longitude
	// float	Altitude
	// float	Speed

	bResult = theStream.WriteBytes( &m_GpsInfo.fTimeStamp, sizeof( m_GpsInfo.fTimeStamp ) );
	if ( !bResult ) return false;

	nValue = (UInt32) m_GpsInfo.fFlags;
	bResult = theStream.WriteBytes( &nValue, sizeof( nValue ) );
	if ( !bResult ) return false;

	bResult = theStream.WriteBytes( &m_GpsInfo.fGpsTime, sizeof( m_GpsInfo.fGpsTime ) );
	if ( !bResult ) return false;

	bResult = theStream.WriteBytes( &m_GpsInfo.fLatitude, sizeof( m_GpsInfo.fLatitude ) );
	if ( !bResult ) return false;

	bResult = theStream.WriteBytes( &m_GpsInfo.fLongitude, sizeof( m_GpsInfo.fLongitude ) );
	if ( !bResult ) return false;

	fValue = (float) m_GpsInfo.fAltitude;
	bResult = theStream.WriteBytes( &fValue, sizeof( fValue ) );
	if ( !bResult ) return false;

	fValue = (float) m_GpsInfo.fSpeed;
	bResult = theStream.WriteBytes( &fValue, sizeof( fValue ) );
	if ( !bResult ) return false;
#endif

	return true;
}


// =============================================================================
//	CGPSInfoList
// =============================================================================

// -----------------------------------------------------------------------------
//		AddUniqueItem
// -----------------------------------------------------------------------------

bool
CGPSInfoList::AddUniqueItem(
	const CGPSInfo& inItem )
{
	CGPSInfo	theItem( inItem );
#if (0)
	CGPSInfo	LastItem;
	if ( size()() > 0 ) {
		if ( GetItem( (GetCount() - 1), LastItem ) ) {
			bool	bEqual = theItem.Compare( LastItem );
			if ( !bEqual ) {
				AddItem( theItem );
				return true;
			}
		}
		return false;
	}
#endif
	push_back( inItem );
	return true;
}


// -----------------------------------------------------------------------------
//		BinaaryFind
// -----------------------------------------------------------------------------

bool
CGPSInfoList::BinaryFind(
	UInt64		inTimeStamp,
	CGPSInfo&	outInfo ) const
{
	// Check for empty list.
	if ( empty() ) return false;
	// Check the timestamp against the first item.
	if ( inTimeStamp < begin()->GetTimeStamp() ) return false;

	// Check the timestamp against the last item.
	const_iterator	itrLast = (end() - 1);
	if ( inTimeStamp > itrLast->GetTimeStamp() ) {
		outInfo = *itrLast;
		return true;
	}

	size_t	nFirst = 0;
	size_t	nLast = size() - 1;
	while ( nFirst <= nLast ) {
		size_t			nMid = (nFirst + nLast) / 2;
		const CGPSInfo&	info = at( nMid );
		if ( inTimeStamp > info.GetTimeStamp() ) {
			nFirst = nMid + 1;
		}
		else if ( inTimeStamp < info.GetTimeStamp() ) {
			nLast = nMid - 1;
		}
		else {
			outInfo = info;
			return true;
		}
	}

	outInfo = *itrLast;
	return true;
}


// -----------------------------------------------------------------------------
//		Find
// -----------------------------------------------------------------------------

bool
CGPSInfoList::Find(
	UInt64		inTimeStamp,
	CGPSInfo&	outInfo ) const
{
	if ( empty() ) return false;

	const_iterator itr = begin();
	while ( (itr != end()) && (itr->GetTimeStamp() > inTimeStamp) ) {
		++itr;
	}

	// If not found, return last entry.
	if ( itr == end() ) {
		itr = end() - 1;
	}

	outInfo = *itr;
	return true;
}


// -----------------------------------------------------------------------------
//		GetSize
// -----------------------------------------------------------------------------

size_t
CGPSInfoList::GetSerialSize() const
{
	return CGPSInfo::GetRecordSize() * size();
}


// -----------------------------------------------------------------------------
//		Serialize
// -----------------------------------------------------------------------------

bool
CGPSInfoList::Serialize(
	CByteVector&	/*outData*/ ) const
{
	for ( size_t i = 0; i < size(); i++ ) {
		bool	bResult = false;
#if (0)
		bResult = GetItem( i )->Write( *pStream );
#endif
		if ( !bResult ) return false;
	}
	return false;
}


// -----------------------------------------------------------------------------
//		Deserialize
// -----------------------------------------------------------------------------

bool
CGPSInfoList::Deserialize(
	size_t			inLength,
	const UInt8*	inData )
{
	UInt32			nRecordSize = CGPSInfo::GetRecordSize();
	UInt32			nBytesRead = 0;
	const UInt8*	pData = inData;

	clear();

	for (;;) {
		if ( nBytesRead == inLength ) return true;
		if ( (nBytesRead + nRecordSize) > inLength ) break;

		CGPSInfo	GpsInfo;
		if ( !GpsInfo.Load( (inLength - nBytesRead), pData ) ) return false;

		push_back( GpsInfo );

		nBytesRead += nRecordSize;
		pData += nRecordSize;
	}

	return false;
}
