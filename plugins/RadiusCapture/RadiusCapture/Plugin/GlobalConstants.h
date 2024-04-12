// ============================================================================
//	 GlobalConstants.h		
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "PeekTime.h"

// Radius Plugin values and defaults
#define kRadiusXMLName			_T("WP-RADIUSCaptureOptions.xml")
#define kRadiusOptionsLog		_T("WP-RADIUSCaptureOptionsLog.txt")
#define kDefaultPeekDir			_T("C:\\Program Files\\WildPackets\\OmniPeek")
#define kDefaultPortOne			_T("1812")
#define kDefaultPortTwo			_T("1813")
#define kDefaultPortThree		_T("1645")
#define kDefaultPortFour		_T("1646")
#define kFileTypePkt			_T(".pkt")
#define kFileTypeLog			_T(".log")
#define kFileInProgress			_T("(In Progress) ")
#define kDefaultOutputDir		_T("C:\\Radius")
#define	kMinutesStr				_T( "Minutes" )
#define	kHoursStr				_T( "Hours" )
#define	kDaysStr				_T( "Days" )
#define	kKilobytesStr			_T( "Kilobytes" )
#define	kMegabytesStr			_T( "Megabytes" )
#define	kGigabytesStr			_T( "Gigabytes" )

#define kSave_Days				0
#define kSave_Hours				1
#define kSave_Minutes			2

#define kMax_Gigabytes			0
#define kMax_Megabytes			1
#define kMax_Kilobytes			2

#define kAgeSizeEditFactor		5
#define kDaysEditMax			120

// Length maxima:
#define MAX_NAME_LENGTH			30
#define MAX_PATH_LENGTH			256

//#define ACTIVATE_ONSUMMARY

//#define TEST_MONITOR

// The following times are in seconds:
#ifdef TEST_MONITOR
  #define MONINTOR_CYCLE_TIME		10
  #define RADIUS_PACKET_TIMEOUT		15
#else
  #define MONINTOR_CYCLE_TIME		70
  #define RADIUS_PACKET_TIMEOUT		60  
#endif

#define SCREEN_LOG_CYCLE_TIME		7

// The following times are in milliseconds:
#define MONITOR_INTERVAL_TIME		200


// Error maxima
#define kMaxPacketWriterErrors		5

// The following is under construction:
//#define PACKET_WRITER_ERROR_HANDLING

//
// The following are for testing / debugging purposes:
//
//#define TEST_NO_CHECKCLOSE
//#define TEST_NO_MONITORING
//#define TEST_NO_LOCKING
//#define DISPLAY_LOCK_ERROR
//#define DISPLAY_LOCK_RETURN
//#define LOCKPTR_DISPLAY_NULL_RETURN
#define DISPLAY_PACKET_NUMBERS

#ifdef TEST_NO_LOCKING
  #define PTR_DOES_NOT_LOCK
#endif

#ifdef _DEBUG
  #define NULL_LOCKPTR_RETURN_DISPLAY(ErrText) ASSERT( 0 );
#else // _DEBUG
  #ifdef LOCKPTR_DISPLAY_NULL_RETURN
  #define NULL_LOCKPTR_RETURN_DISPLAY(ErrText) CString strError = _T("Unable to lock options in ") + (CString)_T(ErrText); \
	AfxMessageBox( strError );
  #else
    #define NULL_LOCKPTR_RETURN_DISPLAY(ErrText)
  #endif // LOCKPTR_DISPLAY_NULL_RETURN
#endif // _DEBUG

//#define TEST_LOCKING
#ifdef TEST_LOCKING
  #define MONITOR_WAIT_TIMEOUT		20000
  #define DEFAULT_TIMEOUT			30000
#else // TEST_LOCKING
  #ifdef _DEBUG
    #define	DEFAULT_TIMEOUT			INFINITE
    #define MONITOR_WAIT_TIMEOUT	INFINITE
  #else // _DEBUG
    #define MONITOR_WAIT_TIMEOUT	20000
    #define	DEFAULT_TIMEOUT			30000
  #endif // _DEBUG
#endif // TEST_LOCKING
