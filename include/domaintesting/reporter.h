#ifndef __DOMAIN_UNITTEST_REPORTER_H__
#define __DOMAIN_UNITTEST_REPORTER_H__

class DomainTestsReporter
{
public:
	DomainTestsReporter() {};
	virtual ~DomainTestsReporter() = 0 {}; 

	/// Used for info (of help, version info or general text)
	//@{
	/// Renders a line of text 
	virtual void Line( const char *text ) = 0;
	/// Renders the start of a test group
	virtual void InfoBeginGroup( unsigned int number, const char *groupName ) = 0;
	/// Renders a test number and name
	virtual void InfoTest( unsigned int number, const char *testName ) = 0;
	/// Renders the end of a test group
	virtual void InfoFinishGroup( const char *groupName ) = 0;
	//@}

	/// Used for test reporting
	//@{
	/// Renders start of test group
	virtual void TestBeginGroup( const char *groupName ) = 0;
	/// Renders a passed test
	virtual void TestPass( unsigned int number, const char *testName ) = 0;
	/// Renders a failed test
	virtual void TestFailed( unsigned int number, const char *testName, unsigned int checkPointFailedAt ) = 0;
	/// Renders the end of a test group
	virtual void TestEndGroup( const char *groupName ) = 0;
	//@}

};

#endif /* __DOMAIN_UNITTEST_REPORTER_H__ */
