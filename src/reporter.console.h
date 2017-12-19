#ifndef __DOMAIN_UNITTEST_REPORTER_CONSOLE_H__
#define __DOMAIN_UNITTEST_REPORTER_CONSOLE_H__

#include "domaintesting/reporter.h"

class ReporterConsole : public DomainTestsReporter
{
	static bool s_forceMonoColour;

public:
	ReporterConsole();
	~ReporterConsole();

	static void ForceMono() { ReporterConsole::s_forceMonoColour = true; };

	void Line( const char *text );
	/// Renders the start of a test group
	void InfoBeginGroup( unsigned int number, const char *groupName );
	/// Renders a test number and name
	void InfoTest( unsigned int number, const char *testName );
	/// Renders the end of a test group
	void InfoFinishGroup( const char *groupName );
	//@}

	/// Used for test reporting
	//@{
	/// Renders start of test group
	void TestBeginGroup( const char *groupName );
	/// Renders a passed test
	void TestPass( unsigned int number, const char *testName );
	/// Renders a failed test
	void TestFailed( unsigned int number, const char *testName, unsigned int checkPointFailedAt );
	/// Renders the end of a test group
	void TestEndGroup( const char *groupName );

};


#endif /* __DOMAIN_UNITTEST_REPORTER_CONSOLE_H__ */

