#include "reporter.console.h"

#include <stdio.h>
#include <string.h>

bool ReporterConsole::s_forceMonoColour = false;

#if defined( _WIN32 )

#include <Windows.h>

extern bool RunningUnderDebugger();

static bool s_RunningUnderDebugger = false;


static void DomainTestSinkConsoleWrite( HANDLE hTo, const char *data, DWORD len  = 0 )
{
	DWORD d;

	if( len == 0 )
	{
		len = ( DWORD )strlen( data );
	}

	while( len != 0 )
	{
		if( WriteFile( hTo, data, len, &d, NULL ) == 0 )
		{
			len = 0;
		}
		else
		{
			len -= d;
		}
	}
}


static void DomainTestSinkConsoleWriteWin32Colour( HANDLE hTo, const char *msg, WORD colour )
{
	if( colour != 0 )
	{
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

		GetConsoleScreenBufferInfo( hTo, &consoleInfo );
		SetConsoleTextAttribute( hTo, colour );
		DomainTestSinkConsoleWrite( hTo, ( char* )msg, ( DWORD )strlen( msg ) );
		SetConsoleTextAttribute( hTo, consoleInfo.wAttributes );
	}
	else
	{
		DomainTestSinkConsoleWrite( hTo, ( char* )msg, ( DWORD )strlen( msg ) );
	}
}

ReporterConsole::ReporterConsole()
{
	s_RunningUnderDebugger = RunningUnderDebugger();
}

ReporterConsole::~ReporterConsole()
{
}

void ReporterConsole::Line( const char *text )
{
	if( s_RunningUnderDebugger == true )
	{
		::OutputDebugString( text );
	}
	else
	{
		HANDLE hStdOut;
		if( ( hStdOut = ::GetStdHandle( STD_OUTPUT_HANDLE ) ) != INVALID_HANDLE_VALUE )
		{
			DomainTestSinkConsoleWrite( hStdOut, text );
		}
	}
}
	
void ReporterConsole::InfoBeginGroup( unsigned int number, const char *groupName )
{
	char x[ 255 ];

	memset( x, 0, 255 );
	snprintf( x, 254, "Group number:%d name:'%s'\n", number, groupName );

	if( s_RunningUnderDebugger == true )
	{
		::OutputDebugString( x );
		::OutputDebugString( "-------------------------------\n" );
	}
	else
	{
		HANDLE hStdOut;
		if( ( hStdOut = ::GetStdHandle( STD_OUTPUT_HANDLE ) ) != INVALID_HANDLE_VALUE )
		{
			DomainTestSinkConsoleWrite( hStdOut, x );
			DomainTestSinkConsoleWrite( hStdOut, "---------------------------------\n" );
		}
	}
}

void ReporterConsole::InfoTest( unsigned int number, const char *testName )
{
	char x[ 255 ];

	memset( x, 0, 255 );
	snprintf( x, 254, "  Test %d - '%s'\n", number, testName ); 

	if( s_RunningUnderDebugger == true )
	{
		::OutputDebugString( x );
	}
	else
	{
		HANDLE hStdOut;
		if( ( hStdOut = ::GetStdHandle( STD_OUTPUT_HANDLE ) ) != INVALID_HANDLE_VALUE )
		{
			DomainTestSinkConsoleWrite( hStdOut, x );
		}
	}
}

void ReporterConsole::InfoFinishGroup( const char *groupName )
{
	if( s_RunningUnderDebugger == true )
	{
		::OutputDebugStringA( "\n" );
	}
	else
	{
		HANDLE hStdOut;
		if( ( hStdOut = ::GetStdHandle( STD_OUTPUT_HANDLE ) ) != INVALID_HANDLE_VALUE )
		{
			DomainTestSinkConsoleWrite( hStdOut, "\n" );
		}
	}
}

void ReporterConsole::TestBeginGroup( const char *groupName )
{
	char numb[ 255 ];
	memset( numb, 0, 255 );
	snprintf( numb, 254, "Start Group '%s'\n", groupName );

	if( s_RunningUnderDebugger == true )
	{
		::OutputDebugString( numb );
		::OutputDebugString( "------------------------------------------\n" );

	}
	else
	{
		HANDLE hStdOut;
		if( ( hStdOut = ::GetStdHandle( STD_OUTPUT_HANDLE ) ) != INVALID_HANDLE_VALUE )
		{
			DomainTestSinkConsoleWrite( hStdOut, numb );
			DomainTestSinkConsoleWrite( hStdOut, "------------------------------------------\n" );
		}
	}
}

void ReporterConsole::TestPass( unsigned int number, const char *testName )
{
	char numb[ 255 ];
	memset( numb, 0, 255 );
	snprintf( numb, 254, "%d '%s'\n", number, testName );

	if( s_RunningUnderDebugger == true )
	{
		::OutputDebugStringA( "[ PASSED ] " );
		::OutputDebugStringA( numb );
	}
	else
	{
		HANDLE hStdOut;
		if( ( hStdOut = ::GetStdHandle( STD_OUTPUT_HANDLE ) ) != INVALID_HANDLE_VALUE )
		{
			if( ReporterConsole::s_forceMonoColour == true )
			{
				DomainTestSinkConsoleWrite( hStdOut, "[ PASSED ] " );
			}
			else
			{
				DomainTestSinkConsoleWriteWin32Colour( hStdOut, "[ PASSED ] ", FOREGROUND_GREEN );
			}

			DomainTestSinkConsoleWrite( hStdOut, numb );
		}
	}
}

void ReporterConsole::TestFailed( unsigned int number, const char *testName, unsigned int checkPointFailedAt )
{
	char numb[ 255 ];
	memset( numb, 0, 255 );
	snprintf( numb, 254, "@Checkpoint:%d %d '%s'\n", checkPointFailedAt, number, testName );

	if( s_RunningUnderDebugger == true )
	{
		::OutputDebugStringA( "[ FAILED ] " );
		::OutputDebugStringA( numb );
	}
	else
	{
		HANDLE hStdOut;
		if( ( hStdOut = ::GetStdHandle( STD_OUTPUT_HANDLE ) ) != INVALID_HANDLE_VALUE )
		{
			if( ReporterConsole::s_forceMonoColour == true )
			{
				DomainTestSinkConsoleWrite( hStdOut, "[ FAILED ] " );
			}
			else
			{
				DomainTestSinkConsoleWriteWin32Colour( hStdOut, "[ FAILED ] ", FOREGROUND_RED );
			}
			DomainTestSinkConsoleWrite( hStdOut, numb );
		}
	}
}
	
void ReporterConsole::TestEndGroup( const char *groupName )
{
	if( s_RunningUnderDebugger == true )
	{
		::OutputDebugString( "\n" );
	}
	else
	{
		HANDLE hStdOut;
		if( ( hStdOut = ::GetStdHandle( STD_OUTPUT_HANDLE ) ) != INVALID_HANDLE_VALUE )
		{
			DomainTestSinkConsoleWrite( hStdOut, "\n" );
		}
	}
}

#else 

ReporterConsole::ReporterConsole()
{
}

ReporterConsole::~ReporterConsole()
{
}

void ReporterConsole::Line( const char *text )
{
	printf( "%s", text );
}
	
void ReporterConsole::InfoBeginGroup( unsigned int number, const char *groupName )
{
	printf( "Group number:%d name:'%s'\n", number, groupName );
	printf( "---------------------------------------------------------------\n" );
}

void ReporterConsole::InfoTest( unsigned int number, const char *testName )
{
	printf( "  Test number:%d name:'%s'\n", number, testName );
}

void ReporterConsole::InfoFinishGroup( const char *groupName )
{
	printf( "\n" );
}

void ReporterConsole::TestBeginGroup( const char *groupName )
{
	printf( "Testing Group name:'%s'\n", groupName );
	printf( "---------------------------------------------------------------\n" );
}

void ReporterConsole::TestPass( unsigned int number, const char *testName )
{
	printf( "[ PASSED ] %d '%s'\n", number, testName );
}

void ReporterConsole::TestFailed( unsigned int number, const char *testName, unsigned int checkPointFailedAt )
{
	printf( "[ FAILED ] @checkpoint:%d - %d '%s' \n", checkPointFailedAt, number, testName );
}
	
void ReporterConsole::TestEndGroup( const char *groupName )
{
	printf( "\n" );
}

#endif

