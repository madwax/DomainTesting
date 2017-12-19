#include <stdio.h>
#include <stdlib.h>

#include "domaintesting/testing.h"

void TestAdd1( void *pUser )
{
	int a = 2;
	int b = 2;
	
	int c = a + b;
	
	DomainTests::CheckPoint( a == 2 );
	DomainTests::CheckPoint( b == 2 );
	DomainTests::CheckPoint( c == 4 );
}

void TestAdd2( void *pUser )
{
	int a = 2;
	int b = 2;
	
	int c = a + b;
	
	DomainTests::CheckPoint( a == 2 );
	DomainTests::CheckPoint( b == 2 );
	DomainTests::CheckPoint( c == 5 );
}

void TestAdd3( void *pUser )
{
	DomainTests::CheckPoint( true );
	
	printf( "ho" );

	DomainTests::CheckPoint( true );
	
	printf( "ho" );

	DomainTests::CheckPoint( true );

	printf( "ho" );

	DomainTests::CheckPoint( true );

	printf( "ho" );
}


int main(int argc, const char **argv ) 
{
	int r = 1;

	DomainTests theTests;

	DomainTests::Register( "Test 1", &TestAdd1, 3 );
	DomainTests::Register( "Test 2", &TestAdd1, 3 );
	
	DomainTests::BeginGroup( "Set 1" );

	DomainTests::Register( "Test 1", &TestAdd1, 3 );
	DomainTests::Register( "Test 2", &TestAdd1, 3 );
	DomainTests::Register( "Test 3", &TestAdd3, 4 );
	
	DomainTests::EndGroup();

	DomainTests::BeginGroup( "Set 2" );

	DomainTests::Register( "Test 1", &TestAdd1, 3 );
	DomainTests::Register( "Test 2", &TestAdd1, 3 );

	DomainTests::EndGroup();

	theTests.HandleCommandLineArgs( argc, argv );

	if( theTests.Run() )
	{
		r = 0;
	}
	
	return r;
}
