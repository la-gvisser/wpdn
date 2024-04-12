#ifndef XLog_H_
#define XLog_H_

#include "AGTypes.h"

namespace XLog {

typedef enum LogLevel
{
	LEVEL_OFF,
	LEVEL_ALWAYS = LEVEL_OFF,
	LEVEL_LOW,
	LEVEL_MEDIUM,
	LEVEL_HIGH
} LogLevel;


#ifdef OPT_LOG
#define DEBUG_LOG(x,y) X_LOG(x,y);
extern void LogInit(const char* pFilename, int nLevel);
extern void LogBeginFunction(int nLevel, const char* szBuf);
extern void LogEndFunction(int nLevel, const char* szBuf);
extern void Log(int nLevel, const char* szBuf);
extern void Log1n(int nLevel, const char* szBuf, const int dParam);
extern void Log1l(int nLevel, const char* szBuf, const UInt64 dParam);
extern void Log1s(int nLevel, const char* szBuf, const char* dParam);
extern void HexDump( int nLevel, PBYTE buffer, DWORD length );
extern void LineDump( int nLevel, PBYTE buffer, DWORD length );

#define X_LOG_INIT(x,y)		XLog::LogInit(x,y);
#define X_LOG(x,y)			XLog::Log(x,y)
#define X_LOG_1n(x,y,z)		XLog::Log1n(x,y,z);
#define X_LOG_1l(x,y,z)		XLog::Log1l(x,y,z);
#define X_LOG_1s(x,y,z)		XLog::Log1s(x,y,z);

// function begin and end markers
// macro for XObject classes
#define X_LOG_BEGIN_METHOD \
	XLog::Log1s( XLog::LEVEL_MEDIUM, "begin %s", GetClassName() );\
	XLog::Log1s( XLog::LEVEL_MEDIUM, "::%s\n", __FUNCTION__  ); \

// macro for non-XObject classed
#define X_LOG_BEGIN_FUNCTION \
	XLog::Log1s( XLog::LEVEL_MEDIUM, "begin %s\n", __FUNCTION__  );

#define X_LOG_END_METHOD \
	XLog::Log1s( XLog::LEVEL_MEDIUM, "end %s", GetClassName() );\
	XLog::Log1s( XLog::LEVEL_MEDIUM, "::%s\n", __FUNCTION__  );

#define X_LOG_END_FUNCTION \
	XLog::Log1s( XLog::LEVEL_MEDIUM, "end %s\n", __FUNCTION__ );

// prints binary data in Hex form
#define X_HEX_DUMP( x, y, z ) XLog::HexDump( x, y, z );
#define X_LINE_DUMP( x, y, z ) XLog::LineDump( x, y, z );

#else

#define DEBUG_LOG(x,y)
#define LogInit(x,y)
#define LogBeginFunction(x,y)
#define LogBeginFunction(x,y)
#define LogEndFunction(x,y)
#define Log0(x,y)
#define Log1(x,y,z)
#define Log1n(x,y,z)
#define Log1l(x,y,z)
#define Log1s(x,y,z)
#define X_LOG_INIT(x,y)	
#define X_LOG(x,y)	
#define X_LOG_1n(x,y,z)
#define X_LOG_1l(x,y,z)	
#define X_LOG_1s(x,y,z)	
#define X_LOG_BEGIN_METHOD
#define X_LOG_END_METHOD
#define X_LOG_BEGIN_FUNCTION
#define X_LOG_END_FUNCTION
#define X_HEX_DUMP(x,y,z)
#define X_LINE_DUMP(x,y,z)

#endif

}

#endif
