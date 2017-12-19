#include "domaintesting/testing.h"
#include "debugbreak.h"
#include "tests.h"
#include "reporter.console.h"


#if defined( _WIN32 )

#include <windows.h>
#include <intrin.h>

bool RunningUnderDebugger()
{
	if( ::IsDebuggerPresent() == TRUE )
	{
		return true;
	}
	return false;
}	


#define RaiseDebuggerBreak __debugbreak()


#elif defined( __APPLE__ )

/// Seee https://developer.apple.com/library/content/qa/qa1361/_index.html
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/sysctl.h>
#include <signal.h>

#define RaiseDebuggerBreak raise( SIGINT )

bool RunningUnderDebugger()
{
	int junk;
	int mib[4];
	struct kinfo_proc info;
	size_t size;

	// Initialize the flags so that, if sysctl fails for some bizarre 
	// reason, we get a predictable result.

	info.kp_proc.p_flag = 0;

	// Initialize mib, which tells sysctl the info we want, in this case
	// we're looking for information about a specific process ID.

	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_PID;
	mib[3] = getpid();

	// Call sysctl.

	size = sizeof(info);
	junk = sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0);
	assert(junk == 0);

	// We're being debugged if the P_TRACED flag is set.
	return ( (info.kp_proc.p_flag & P_TRACED) != 0 );
}	

#elif defined( __linux__ )

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ptrace.h>

#define RaiseDebuggerBreak raise( SIGINT )

static bool RunningUnderDebugger()
{
  int pid = fork();

  bool res = false;

	if( pid == 0 )
	{
		int ppid = getppid();
		int retCode = 0;

		/* Child */
		if( ptrace( PTRACE_ATTACH, ppid, NULL, NULL ) == 0 )
		{
			/* Wait for the parent to stop and continue it */
			waitpid( ppid, NULL, 0 );
			ptrace( PTRACE_CONT, NULL, NULL) ;

			/* Detach */
			ptrace( PTRACE_DETACH, getppid(), NULL, NULL );
		}
		else
		{
			/* Trace failed so gdb is present */
			retCode = 1;
		}
		exit( retCode );	
	}
	else if( pid != -1 ) 
	{
	  int status;
		
		waitpid( pid, &status, 0) ;
    if( WEXITSTATUS( status ) )
		{
			res = true;
		}
	}

  return res;
}	

#else

#warning Unknown platform......	

bool RunningUnderDebugger()
{
	return false;
}		

#endif


enum DomainTestsCommands {
	/// Display help
	Help = 0,
	/// Display a list of all the tests
	List,
	/// Run all or part of the tests.
	Run,
};

static const unsigned int NoGroupTestValue = ( ( unsigned int )-1 );

struct DomainTestsPrivate
{
	DomainTestsReporter *m_theReporter;
	enum DomainTestsCommands m_command;

	/// Used to see if we are to test a group
	unsigned int m_testGroup;
	/// used to see if we are to test a a test within a group
	unsigned int m_testTest;

	bool m_testDoAbortOnFirstFailed;
};

DomainTests *DomainTests::s_theUnitTests = NULL;

DomainTests::DomainTests() : m_private( NULL ), m_numberOfGroups( 1 ), m_theGroups( NULL ), m_theLastGroup( NULL ), m_theCurrentGroup( NULL ), m_runtimetheCurrentTest( NULL )
{
	DomainTests::s_theUnitTests = this;
	
	this->m_private = new struct DomainTestsPrivate;
	memset( this->m_private, 0, sizeof( struct DomainTestsPrivate ) );

	this->m_private->m_theReporter = NULL;
	this->m_private->m_command = DomainTestsCommands::Help;

	this->m_private->m_testGroup = NoGroupTestValue;
	this->m_private->m_testTest = NoGroupTestValue;

	this->m_private->m_testDoAbortOnFirstFailed = true;

	this->m_underDebugger = RunningUnderDebugger();

	this->m_theLastGroup = DomainTestsGroupCreate( NULL );
	this->m_theGroups = this->m_theLastGroup;
	this->m_theCurrentGroup = this->m_theGroups;
}

DomainTests::~DomainTests()
{
	if( this->m_private != NULL )
	{
		if( this->m_private->m_theReporter != NULL )
		{
			delete this->m_private->m_theReporter;
			this->m_private->m_theReporter = NULL;
		}

		delete this->m_private;
		this->m_private = NULL;
	}
}

struct DomainTestsGroup *DomainTests::FindGroup( const char *name )
{
	struct DomainTestsGroup *current = this->m_theGroups;
	while( current != NULL )
	{
		if( name == NULL && current->m_name == NULL )
		{
			return current;
		}
		else if( current->m_name != NULL && strcmp( current->m_name, name ) == 0 )
		{
			return current;
		}
		current = current->m_next;
	}
	return NULL;
}

void DomainTests::BeginGroup( const char *groupName )
{
	DomainTests *pTheTest = DomainTests::s_theUnitTests;
	
	struct DomainTestsGroup *foundGroup = pTheTest->FindGroup( groupName );
	if( foundGroup == NULL )
	{
		foundGroup = DomainTestsGroupCreate( groupName );
		if( foundGroup != NULL )
		{
			pTheTest->m_theLastGroup->m_next = foundGroup;
			pTheTest->m_theLastGroup = foundGroup;

			++pTheTest->m_numberOfGroups;
		}
	}
	pTheTest->m_theCurrentGroup = foundGroup;
}

void DomainTests::Register( const char *testName, DomainTestCB testCb, unsigned int numberOfCheckPoints, void *pUserData, DomainTestPreCB preCb, DomainTestPostCB postCb  )
{
	DomainTests *pTheTests = DomainTests::s_theUnitTests;

	struct DomainTest *theNewTest = DomainTestCreate( testName, testCb, numberOfCheckPoints, pUserData, preCb, postCb );
	if( theNewTest != NULL )
	{
		if( pTheTests->m_theCurrentGroup->m_theLastTest == NULL )
		{
			pTheTests->m_theCurrentGroup->m_theTests = theNewTest;			
			pTheTests->m_theCurrentGroup->m_theLastTest = theNewTest;
		}
		else
		{
			pTheTests->m_theCurrentGroup->m_theLastTest->m_next = theNewTest;
			pTheTests->m_theCurrentGroup->m_theLastTest = theNewTest;
		}
		++pTheTests->m_theCurrentGroup->m_numberOfTests;
	}
}

void DomainTests::EndGroup()
{
	DomainTests *pTheTests = DomainTests::s_theUnitTests;
	/// reset to the first group which is the unamed one.
	pTheTests->m_theCurrentGroup = pTheTests->m_theGroups;
}

bool DomainTests::CheckPoint( bool isAPass )
{
	DomainTests *pTest = DomainTests::s_theUnitTests;
	
	if( isAPass )
	{
		++pTest->m_runtimetheCurrentTest->m_runtimeCheckPoints;
	}
	else
	{
		if( pTest->m_underDebugger == true )
		{
			debug_break();
		}
	}
	return isAPass;
}

void DomainTests::Report( const char *msg )
{
	DomainTestsReporter *theReporter = DomainTests::s_theUnitTests->m_private->m_theReporter;
	theReporter->Line( msg );
}

static int FindIndexOfOption( int argc, const char **argv, const char *option, const char *optionAlt, int offset = 1 )
{
	for( int i=offset; i<argc; ++i )
	{
		int xoff = 0;
		const char *item = argv[ i ];
		if( item[ 0 ] == '/' )
		{
			xoff = 1;
		}
		else if( ( item[ 0 ] == '-' && item[ 1 ] == '-' ) )
		{
			xoff = 2;
		}

		if( xoff != 0 )
		{
			const char *x = item + xoff;

			if( strcmp( x, option ) == 0 )
			{
				return i;
			}

			if( optionAlt != NULL && strcmp( x, optionAlt ) == 0 )
			{
				return i;
			}
		}
	}
	return -1;
}

bool DomainTests::HandleCommandLineArgs( int argc, const char **argv )
{
	int offset = -1;

	/// we do these options first before we check what type of reporter we should have as these goto the default reporter (console)

	if( ( FindIndexOfOption( argc, argv, "help", "?" )  ) != -1 )
	{
		this->m_private->m_command = DomainTestsCommands::Help;
		return true;
	}
	
	if( ( FindIndexOfOption( argc, argv, "list", "l" )  ) != -1 )
	{
		this->m_private->m_command = DomainTestsCommands::List;
		return true;
	}

	this->m_private->m_command = DomainTestsCommands::Run;

	if( FindIndexOfOption( argc, argv, "mono", NULL ) != -1 )
	{
		ReporterConsole::ForceMono();
	}

	if( ( offset = FindIndexOfOption( argc, argv, "group", "g" ) ) != -1 )
	{
		// we have a file reporter
		this->m_private->m_testGroup = ( unsigned int )atoi( argv[ offset + 1 ] );
		this->m_private->m_testTest = NoGroupTestValue;

		if( ( offset = FindIndexOfOption( argc, argv, "test", "t" ) ) != -1 )
		{
			this->m_private->m_testTest = ( unsigned int )atoi( argv[ offset + 1 ] );
		}
	}

	if( FindIndexOfOption( argc, argv, "noabort", "n" ) != -1 )
	{
		this->m_private->m_testDoAbortOnFirstFailed = false;
	}

	return true;
}

void DomainTests::DisplayHelp()
{
	DomainTestsReporter *theReporter = this->m_private->m_theReporter;

	theReporter->Line( "Unit Test Help\n" );
	theReporter->Line( "\n" );

	theReporter->Line( "If a unit test fails 1 is returned.\n" );
	theReporter->Line( "\n" );

	theReporter->Line( "Options are passed with '--' or '/'\n" );
	theReporter->Line( "  --? or --help This help\n" );
	theReporter->Line( "  --l or --list  Lists all grouped tests\n" );
	theReporter->Line( "  --g number or --group number  Only test group with number\n" );
	theReporter->Line( "  --t number or --test number  You most already have passed --g Only run the test with number\n" );
	theReporter->Line( "  --n or --noabort  Don't abort testing on first failer\n" );

	theReporter->Line( "\n" );
}

static const char *defaultGroup = "Ungrouped";

void DomainTests::ListTests()
{
	DomainTestsReporter *theReporter = this->m_private->m_theReporter;

	theReporter->Line( "List of Unit Tests\n\n" );

	DomainTestsGroup *theGroup = this->m_theGroups;

	unsigned int grpNum = 0;

	while( theGroup != NULL )
	{
		char *n = theGroup->m_name;
		if( n == NULL )
		{
			n = ( char* )defaultGroup;
		}

		theReporter->InfoBeginGroup( grpNum, n );

		DomainTest *theTest = theGroup->m_theTests;

		unsigned int i=0;
		while( theTest != NULL )
		{
			theReporter->InfoTest( i, theTest->m_name );
			++i;

			theTest = theTest->m_next;
		}
		theReporter->InfoFinishGroup( n );

		theGroup = theGroup->m_next;
		++grpNum;
	}
	theReporter->Line( "\n" );
}

bool DomainTests::RunTests()
{
	DomainTestsReporter *theReporter = this->m_private->m_theReporter;

	theReporter->Line( "Starting Test(s)\n\n" );

	DomainTestsGroup *theGroup = this->m_theGroups;

	bool noneFailed = true;

	unsigned int grpNum = 0;
	unsigned int testNumber = 0;

	bool doGroupTests = true;
	bool doTest = true;

	while( theGroup != NULL )
	{
		char *n = theGroup->m_name;
		if( n == NULL )
		{
			n = ( char* )defaultGroup;
		}

		doGroupTests = false;
		if( this->m_private->m_testGroup == NoGroupTestValue )
		{
			doGroupTests = true;
		}
		else if( this->m_private->m_testGroup == grpNum )
		{
			doGroupTests = true;
		}
		
		if( doGroupTests == true )
		{
			testNumber = 0;

			theReporter->TestBeginGroup( n );

			DomainTest *theTest = theGroup->m_theTests;

			while( theTest != NULL )
			{
				doTest = false;

				if( this->m_private->m_testTest == NoGroupTestValue )
				{
					doTest = true;
				}
				else if( this->m_private->m_testTest == testNumber )
				{
					doTest = true;
				}

				if( doTest == true )
				{
					bool passed = false;
					int whichCheckPoint = -1;

					this->m_runtimetheCurrentTest = theTest;

					try
					{
						void *pUserData = NULL;
					
						theTest->m_runtimeCheckPoints = 0;

						if( theTest->m_preCb != NULL )
						{
							pUserData = ( *( theTest->m_preCb ) )();
						}

						if( theTest->m_testCb != NULL )
						{
							( *( theTest->m_testCb ) )( pUserData );
						}

						if( theTest->m_postCb != NULL )
						{
							( *( theTest->m_postCb ) )( pUserData );
						}

						if( theTest->m_numberOfCheckPoints == theTest->m_runtimeCheckPoints )
						{
							theTest->m_runtimePassed = true;
							passed = true;
						}
					}
					catch( ... )
					{
						theReporter->Line( "Failed due to unhandled exception\n" );
						theTest->m_runtimeCheckPoints = -2;
					}

					if( passed == false )
					{
						noneFailed = true;

						// we have failed the test so abort!
						theReporter->TestFailed( testNumber, theTest->m_name, theTest->m_runtimeCheckPoints );
						if( this->m_private->m_testDoAbortOnFirstFailed == true )
						{
							theReporter->TestEndGroup( n );
							return false;
						}
					}
					else
					{
						theReporter->TestPass( testNumber, theTest->m_name );
					}
				}

				++testNumber;
				theTest = theTest->m_next;
			}
		}

		theReporter->TestEndGroup( n );

		++grpNum;
		theGroup = theGroup->m_next;
	}
	return noneFailed;
}

bool DomainTests::Run()
{
	bool r = true;

	char *s = NULL;
	int i = 0;
	do
	{
		s = environ[ i ];
		OutputDebugStringA( s );
		OutputDebugStringA( "\n" );
		++i;
	} while( s != NULL );

	// do we have a reporter set?
	if( this->m_private->m_theReporter == NULL )
	{
		// create the default console one
		this->m_private->m_theReporter = new ReporterConsole();
	}

	switch( this->m_private->m_command )
	{
		case DomainTestsCommands::Help:
		{
			this->DisplayHelp();	
		}break;

		case DomainTestsCommands::List:
		{
			this->ListTests();
		}break;

		case DomainTestsCommands::Run:
		{
			r = this->RunTests();
		}break;
	}

	if( r == true )
	{
		this->m_private->m_theReporter->Line( "[ UNIT PASSED ]\n" );
	}
	else
	{
		this->m_private->m_theReporter->Line( "[ UNIT FAILED ]\n" );
	}
	return r;
}






