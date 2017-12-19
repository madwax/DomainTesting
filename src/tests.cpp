#include "tests.h"

#include <string.h>


struct DomainTest *DomainTestCreate( const char *testName, DomainTestCB testCb, unsigned int numberOfCheckPoints, void *pUserData, DomainTestPreCB preCb, DomainTestPostCB postCb )
{
	struct DomainTest *theNewTest = new struct DomainTest;
	if( theNewTest != NULL )
	{
		size_t sz = strlen( testName );
	
		theNewTest->m_name = new char[ sz + 1 ];
	
		memset( theNewTest->m_name, 0, sz + 1 );
		memcpy( theNewTest->m_name, testName, sz );
	
		theNewTest->m_testCb = testCb;
		theNewTest->m_preCb = preCb;
		theNewTest->m_postCb = postCb;
		theNewTest->m_next = NULL;
	
		theNewTest->m_numberOfCheckPoints = numberOfCheckPoints;
		theNewTest->m_runtimeCheckPoints = 0;
	}
	return theNewTest;
}

struct DomainTest *DomainTestDestroy( struct  DomainTest *theTest )
{
	struct DomainTest *r = NULL;
	if( theTest != NULL )
	{
		r = theTest->m_next;
		delete [] theTest->m_name;
		delete theTest;
	}
	return r;
} 

struct DomainTestsGroup *DomainTestsGroupCreate( const char *name )
{
	struct DomainTestsGroup *r;

	r = new struct DomainTestsGroup;
	if( r != NULL )
	{
		memset( r, 0, sizeof( struct DomainTestsGroup ) );
		if( name != NULL )
		{
			size_t len = strlen( name );
			r->m_name = new char[ len + 1 ];
			if( r->m_name == NULL )
			{
				delete r;
				return NULL;
			}

			memset( r->m_name, 0, len + 1 );
			memcpy( r->m_name, name, len );
		}
		else
		{
			r->m_name = NULL;
		}

		r->m_numberOfTests = 0;
		r->m_runtimePassed = 0;
		r->m_theTests = NULL;
		r->m_theLastTest = NULL;
		r->m_next = NULL;
	}
	return r;
}

// cleans up the group object and returns the next one.
struct DomainTestsGroup* DomainTestsGroupDestroy( struct DomainTestsGroup *theGroup )
{
	struct DomainTestsGroup *r = NULL;

	if( theGroup != NULL )
	{
		struct DomainTest *theTest = theGroup->m_theTests;

		while( ( theTest = DomainTestDestroy( theTest) ) != NULL )
		{
		}

		r = theGroup->m_next;

		if( theGroup->m_name != NULL )
		{
			delete [] theGroup->m_name;
		}

		delete theGroup;
	}

	return r;
}