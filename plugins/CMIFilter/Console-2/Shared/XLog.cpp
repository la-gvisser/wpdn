
#include "stdafx.h"
#include "XLog.h"
#include "ByteStream.h"



// dump utils

#ifdef OPT_LOG

int g_nDepth	= 0;
int g_nLevel	= XLog::LEVEL_MEDIUM;
FILE* g_pFile	= stderr;

#define PRINT_SPACES(x) 

#define CHECK_LEVEL(x) if (x > g_nLevel) return;

void XLog::LogInit( const char* pFilename, int nLevel )
{
	g_nLevel = nLevel;

	if (g_pFile && g_pFile != stderr)
	{
		fclose(g_pFile);
	}

	// default to stderr
	g_pFile = stderr;

	if (pFilename && strlen(pFilename))
	{
		FILE* pFile = fopen( pFilename, "w+" );
		if (pFile != 0)
		{
			g_pFile = pFile;
		}
	}
}

void XLog::LogBeginFunction(int nLevel, const char* szBuf)
{
	CHECK_LEVEL(nLevel)
	PRINT_SPACES(g_nDepth);
	fprintf( g_pFile, "begin " );
	fprintf( g_pFile, szBuf );
	fprintf( g_pFile, "\n" );
	fflush(g_pFile);
	g_nDepth++;
}

void XLog::LogEndFunction(int nLevel, const char* szBuf)
{
	CHECK_LEVEL(nLevel)
	g_nDepth--;
	PRINT_SPACES(g_nDepth);
	fprintf( g_pFile, "end   " );
	fprintf( g_pFile, szBuf );
	fprintf( g_pFile, "\n" );
	fflush(g_pFile);
}

void XLog::Log(int nLevel, const char* szBuf)
{
	CHECK_LEVEL(nLevel)
	PRINT_SPACES(g_nDepth);
	fprintf( g_pFile, szBuf );
	fflush(g_pFile);
}


void XLog::Log1n(int nLevel, const char* szBuf, const int dParam)
{
	CHECK_LEVEL(nLevel)
	PRINT_SPACES(g_nDepth);
	fprintf( g_pFile, szBuf, dParam );
	fflush(g_pFile);
}

void XLog::Log1l(int nLevel, const char* szBuf, const UInt64 dParam)
{
	CHECK_LEVEL(nLevel)
	PRINT_SPACES(g_nDepth);
	fprintf( g_pFile, szBuf, dParam );
	fflush(g_pFile);
}

void XLog::Log1s(int nLevel, const char* szBuf, const char* dParam)
{
	CHECK_LEVEL(nLevel)
	PRINT_SPACES(g_nDepth);
	fprintf( g_pFile, szBuf, dParam );
	fflush(g_pFile);
}

//=============================================================================
//		General purpose Platform neutral utilities
//=============================================================================

void
XLog::HexDump( int nLevel,  PBYTE buffer, DWORD length )
{
	CHECK_LEVEL(nLevel)

	CByteStream outStream;
	DWORD i,count,index;
	CHAR rgbDigits[]="0123456789abcdef";
	CHAR rgbLine[100];
	char cbLine;

	// Start with a linbfeed
	outStream.Write("\n");

	for(index = 0; length; length -= count, buffer += count, index += count) 
	{
		count = (length > 16) ? 16:length;

		sprintf(rgbLine, "%4.4x  ",index);
		cbLine = 6;

		for(i=0;i<count;i++) 
		{
			rgbLine[cbLine++] = rgbDigits[buffer[i] >> 4];
			rgbLine[cbLine++] = rgbDigits[buffer[i] & 0x0f];
			if(i == 7) 
			{
				rgbLine[cbLine++] = ':';
			} 
			else 
			{
				rgbLine[cbLine++] = ' ';
			}
		}
		for(; i < 16; i++) 
		{
			rgbLine[cbLine++] = ' ';
			rgbLine[cbLine++] = ' ';
			rgbLine[cbLine++] = ' ';
		}

		rgbLine[cbLine++] = ' ';

		for(i = 0; i < count; i++) 
		{
			if(buffer[i] < 32 || buffer[i] > 126 || buffer[i] == '%') 
			{
				rgbLine[cbLine++] = '.';
			} 
			else 
			{
				rgbLine[cbLine++] = buffer[i];
			}
		}

		rgbLine[cbLine++] = 0;
		outStream.Write(rgbLine);
		outStream.Write("\n");
	}

	char c = 0;
	outStream.WriteBytes( &c, 1 );
	Log1s( nLevel, "%s", (char*)outStream.GetData() );
}

//=============================================================================
//		General purpose Platform neutral utilities
//=============================================================================

void
XLog::LineDump( int nLevel, PBYTE buffer, DWORD length )
{
	CHECK_LEVEL(nLevel)

	CByteStream outStream;
	DWORD i,count,index;
	CHAR rgbLine[100];
	char cbLine;

	// Start with a linbfeed
	outStream.Write("\n");

	for(index = 0; length; length -= count, buffer += count, index += count) 
	{
		count = (length > 80) ? 80:length;

		sprintf(rgbLine, "%4.4x  ",index);
		cbLine = 6;

		rgbLine[cbLine++] = ' ';

		for(i = 0; i < count; i++) 
		{
			if (buffer[i]==10 || buffer[i]==13)
				continue;

			// only process text
			if (buffer[i] < 32 || buffer[i] > 126)
				return;

			rgbLine[cbLine++] = buffer[i];
		}

		rgbLine[cbLine++] = 0;
		outStream.Write(rgbLine);
		outStream.Write("\n");
	}

	char c = 0;
	outStream.WriteBytes( &c, 1 );
	Log1s( nLevel, "%s", (char*)outStream.GetData() );
}

#endif

