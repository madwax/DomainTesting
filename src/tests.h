#ifndef __DOMAIN_UNITTEST_TESTS_H__
#define __DOMAIN_UNITTEST_TESTS_H__

#include "domaintesting/testing.h"

struct DomainTest
{
	char *m_name;
	
	unsigned int m_numberOfCheckPoints;

	DomainTestPreCB m_preCb;
	DomainTestCB m_testCb;
	DomainTestPostCB m_postCb;
	
	struct DomainTest *m_next;

	bool m_runtimePassed;
	unsigned int m_runtimeCheckPoints;
};


struct DomainTestsGroup
{
	char *m_name;

	struct DomainTest *m_theTests;
	struct DomainTest *m_theLastTest;

	unsigned int m_numberOfTests;

	unsigned int m_runtimePassed;

	struct DomainTestsGroup *m_next;
};

extern struct DomainTest *DomainTestCreate( const char *testName, DomainTestCB testCb, unsigned int numberOfCheckPoints, void *pUserData, DomainTestPreCB preCb, DomainTestPostCB postCb );
extern struct DomainTest *DomainTestDestroy( struct DomainTest *theTest );
extern struct DomainTestsGroup *DomainTestsGroupCreate( const char *name );
extern struct DomainTestsGroup* DomainTestsGroupDestroy( struct DomainTestsGroup *theGroup );


#endif /* __DOMAIN_UNITTEST_TESTS_H__ */

