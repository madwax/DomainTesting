#ifndef __DOMAIN_UNITTEST_H__
#define __DOMAIN_UNITTEST_H__

#include <stdio.h>

/** this returns the user data which is passed onto the test and post test callbacks.
*/
typedef void* ( *DomainTestPreCB )();
typedef void ( *DomainTestCB )( void *pUserData );
typedef void ( *DomainTestPostCB )( void *pUserData );

struct DomainTest;
struct DomainTestsGroup;

// internal
struct DomainTestsPrivate;

class DomainTests 
{
	static DomainTests *s_theUnitTests;

	struct DomainTestsPrivate *m_private;

	unsigned int m_numberOfGroups;
	struct DomainTestsGroup *m_theGroups;
	struct DomainTestsGroup *m_theLastGroup;
	struct DomainTestsGroup *m_theCurrentGroup;

	struct DomainTest *m_runtimetheCurrentTest;

	bool m_underDebugger;

	struct DomainTestsGroup *FindGroup( const char *name );

	void DisplayHelp();
	void ListTests();
	bool RunTests();

public:
	DomainTests();
	~DomainTests();

	static void BeginGroup( const char *groupName );

	static void Register( const char *testName, DomainTestCB testCb, unsigned int numberOfCheckPoints, void *pUserData = NULL, DomainTestPreCB preCb = NULL, DomainTestPostCB postCb = NULL );
	
	static void EndGroup();

	static bool CheckPoint( bool isAPass );

	static void Report( const char *msg );
	
	/** Use to parse the command line passed to the test app.
	* \return false means exit the app.
	*/
	bool HandleCommandLineArgs( int argc, const char **argv );

	bool Run();
	
};


#endif /* __DOMAIN_UNITTEST_H__ */

