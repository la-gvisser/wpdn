// ============================================================================
//	GPSInfo.h
//		interface for CGPSInfo class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#include "CaptureFile.h"
#include <vector>
#include <string>

using CaptureFile::CByteVector;
using CaptureFile::AtoW;
using CaptureFile::WtoA;


// ============================================================================
//		Data Structures
// ============================================================================

typedef struct {
	UInt64		fIndex;				// GPS Sample counter
	UInt32		fFlags;				// Flags
	UInt32		fReserved;			// Reserved
	UInt64		fTimeStamp;			// System Time of the GPS Time: Nanoseconds since Jan 1, 1601
	UInt64		fGpsTime;			// GPS Time
	double		fLatitude;			// ddd.mmmmmm (0.mmmmm * 60 = mm.mmmm')
	double		fLongitude;			// ddd.mmmmmm (0.mmmmm * 60 = mm.mmmm')
	double		fAltitude;			// mmmmm.mm	meters
	double		fSpeed;				// mmmmm.mm meters per second
} WP_GPS_INFO;


// ============================================================================
//		CGPSInfo
// ============================================================================

class CGPSInfo
{
public:
	// Flags
	static const UInt32	kWpGps_Valid_Latitude	= 0x00000001;	// The Latitude is valid.
	static const UInt32	kWpGps_Valid_Longitude	= 0x00000002;	// The Longitude is valid.
	static const UInt32	kWpGps_Valid_Position	= 0x00000003;	// The Latitude and Longitude is valid.
	static const UInt32	kWpGps_Valid_GPSTime	= 0x00000008;	// The GPS Time is valid.
	static const UInt32	kWpGps_Valid_Altitude	= 0x00000010;	// The Altitude is valid.
	static const UInt32	kWpGps_Valid_Speed		= 0x00000020;	// The Speed is valid.
	static const UInt32	kWpGps_Valid_TimeStamp	= 0x00000040;	// The Timestamp (system time) is valid.
	static const UInt32	kWpGps_Warning			= 0x80000000;	// GPS Information received with a warning.

protected:
	WP_GPS_INFO		m_GpsInfo;

public:
	static UInt32	GetRecordSize();

public:
	;			CGPSInfo() { memset( &m_GpsInfo, 0, sizeof( m_GpsInfo ) ); }
	;			CGPSInfo( const CGPSInfo& inInfo ) : m_GpsInfo( inInfo.m_GpsInfo ) {}
	;			CGPSInfo( const WP_GPS_INFO* pInfo )
	{
		if ( pInfo ) {
			m_GpsInfo = *pInfo;
		}
		else {
			memset( &m_GpsInfo, 0, sizeof( m_GpsInfo ) ); 
		}
	}
	virtual		~CGPSInfo() {};

	const WP_GPS_INFO& GetInfo() { return m_GpsInfo; };

	bool		Compare( const CGPSInfo& inInfo ) const;
	void		ConvertToUSMeasurements(){
					static float	kFeetPerMeter = (float)(1 / (float)0.3048);
					m_GpsInfo.fAltitude *= kFeetPerMeter;	// meters -> feet.
					m_GpsInfo.fSpeed *= 0.6215;				// mps -> mph
				};

	std::string		FormatA() const { return WtoA( Format() ); }
	std::wstring	Format() const;

	std::wstring	GetAltitude() const;
	bool			GetAltitude( double& outAltitude ) const;
	bool			GetAltitude( SInt32& outAltitude ) const;
	void			GetFlags( UInt64& outFlags ) const { outFlags = m_GpsInfo.fFlags; };
	std::wstring	GetFlags() const;
	std::wstring	GetLatitude() const;
	bool			GetLatitude( double& outLatitude ) const;
	bool			GetLatitude( SInt32& outLatitude ) const;
	bool			GetLatitudeDirection( wchar_t& outDirection ) const;
	std::wstring	GetLongitude() const;
	bool			GetLongitude( double& outLongitude ) const;
	bool			GetLongitude( SInt32& outLongitude ) const;
	bool			GetLongitudeDirection( wchar_t& outDirection ) const;
	std::wstring	GetSpeed() const;
	bool			GetSpeed( double& outSpeed ) const;
	bool			GetSpeed( SInt32& outSpeed ) const;
	std::wstring	GetTime( int inFormat, bool inLocalTime ) const;
	bool			GetTime( UInt64& outTime ) const;
	UInt64			GetTimeStamp()  const{ return m_GpsInfo.fTimeStamp; };

	bool		IsCautioned() const { return ((m_GpsInfo.fFlags & kWpGps_Warning) != 0); };

	bool		Load( size_t inLength, const UInt8* inData );
	//bool		Read( const CByteVector& inData );

	static void	TextHeadings( std::wstring& outHeading, wchar_t inDelimiter );
#if (0)
	void		ToText( CByteVector& inStream, wchar_t inDelimiter, int inFormat, bool bLocalTime );
#endif

	bool		Write( CByteVector& outData );
};


// ============================================================================
//		CGPSInfoList
// ============================================================================

class CGPSInfoList
	:	public std::vector<CGPSInfo>
{
public:
	bool	AddUniqueItem( const CGPSInfo& inInfo );

	bool	BinaryFind( UInt64 inTimeStamp, CGPSInfo& outInfo ) const;

	bool	Deserialize( size_t inLength, const UInt8* inData );
	bool	Deserialize( const CByteVector& inData ) {
		return Deserialize( inData.size(), &inData[0] );
	}

	bool	Find( UInt64 inTimeStamp, CGPSInfo& outInfo ) const;
	
	size_t	GetSerialSize() const;
	
	bool	Serialize( CByteVector& outData ) const;
};
