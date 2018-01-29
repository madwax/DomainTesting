cmake_minimum_required(VERSION 3.1.0)

project( DomainTesting )

option( DomainTestingEnableTest "Build the DomainTesting Test" ON )

set( TARGET_INCLUDE_DIR ${DomainTesting_SOURCE_DIR}/include )
set( TARGET_SOURCE_DIR "${DomainTesting_SOURCE_DIR}/src" )

set( TARGET_SOURCES ${TARGET_INCLUDE_DIR}/domaintesting/testing.h
	${TARGET_SOURCE_DIR}/debugbreak.h
	${TARGET_SOURCE_DIR}/tests.h
	${TARGET_SOURCE_DIR}/tests.cpp
	${TARGET_INCLUDE_DIR}/domaintesting/reporter.h
	${TARGET_SOURCE_DIR}/reporter.console.h
	${TARGET_SOURCE_DIR}/reporter.console.cpp
	${TARGET_SOURCE_DIR}/testing.cpp
)



set( TARGET_LINK "" )
set( TESTS_LINK "" )
set( DEMO_LINK "" )

set( TARGET_DEFINES "" )
set( TARGET_DEFINES_PRIVATE "" )

if( DEFINED DomainTestingPlatform )

	message( "User passed ${DomainTestingPlatform}" )

else()

	set( DomainTestingSubPlatform "" )

	# need to work out what we have.
	if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )

		set( DomainTestingPlatform "Win32" )

	elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )

		set( DomainTestingPlatform "Darwin" )

		set( DomainTestingSubPlatform "OSX" )

	elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )

		set( DomainTestingPlatform "Linux" )

		if( ANDROID_NDK )

			set( DomainTestingSubPlatform "Android" )

		else()

			set( DomainTestingSubPlatform "Desktop" )

		endif()

	elseif( ${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" )

		set( DomainTestingPlatform "FreeBSD" )
		set( DomainTestingSubPlatform "" )

	endif()

endif()


if( ${DomainTestingPlatform} MATCHES "Win32" )
	
	list( APPEND TARGET_DEFINES "DT_PLATFORM_IS_WIN32=1" )

	# If you want to build without replying on a C Runtime under Win32
	if( DEFINED DomainTestingPlatformNoCLib )

		list( APPEND TARGET_DEFINES "DT_PLATFORM_NO_CLIB=1" )
	else()

		list( APPEND TARGET_DEFINES "DT_PLATFORM_NO_CLIB=0" )

	endif()

else()

	list( APPEND TARGET_DEFINES "DT_PLATFORM_IS_UNIX=1" )

	if( ${DomainTestingPlatform} MATCHES "Darwin" )

		list( APPEND TARGET_DEFINES "DT_PLATFORM_IS_BSD=1" )
		list( APPEND TARGET_DEFINES "DT_PLATFORM_IS_DARWIN=1" )

		if( ${DomainTestingSubPlatform} MATCHES "OSX" )
			
			list( APPEND TARGET_DEFINES "DT_PLATFORM_IS_OSX=1" )

		elseif( ${DomainTestingSubPlatform} MATCHES "iOS" )

			list( APPEND TARGET_DEFINES "DT_PLATFORM_IS_IOS=1" )

		endif()

	elseif( ${DomainTestingPlatform} MATCHES "Linux" )

		list( APPEND TARGET_DEFINES "DT_PLATFORM_IS_LINUX=1" )

		if( ${DomainTestingSubPlatform} MATCHES "Android" )

			list( APPEND TARGET_DEFINES "DT_PLATFORM_IS_ANDROID=1" )

		endif()

	elseif( ${DomainTestingPlatform} MATCHES "FreeBSD" )

		list( APPEND TARGET_DEFINES "DT_PLATFORM_IS_BSD=1" )
		list( APPEND TARGET_DEFINES "DT_PLATFORM_IS_FREEBSD=1" )

	else()

	endif()
	
endif()

if( ${Base_Platform_UNIX} )
	
	message( "Using pthreads as were on a Unix platform" )
	
	find_package( Threads REQUIRED )

	list( APPEND TARGET_LINK Threads::Threads )
	list( APPEND TESTS_LINLK Threads::Threads )

else()

endif()

list( APPEND TESTS_LINLK  DomainTesting )

add_library( DomainTesting STATIC ${TARGET_SOURCES} )

set_target_properties( DomainTesting PROPERTIES ARCHIVE_OUTPUT_NAME "DomainTesting" )

target_include_directories( DomainTesting PRIVATE ${TARGET_INCLUDE_DIR} ${TARGET_SOURCE_DIR} )
target_compile_definitions( DomainTesting PRIVATE ${TARGET_DEFINES} ${TARGET_DEFINES_PRIVATE} )
target_link_libraries( DomainTesting PRIVATE ${TARGET_LINK} )

target_include_directories( DomainTesting INTERFACE ${TARGET_INCLUDE_DIR} )
target_compile_definitions( DomainTesting INTERFACE ${TARGET_DEFINES} )
target_link_libraries( DomainTesting INTERFACE ${TARGET_LINK} )

if( ${DomainTestingEnableTest} )

set( TESTS_SOURCE_DIR ${DomainTesting_SOURCE_DIR}/tests )

set( TESTS_INCLUDE_DIR ${DomainTesting_SOURCE_DIR}/tests/src )

set( TEST_SOURCES ${TESTS_SOURCE_DIR}/main.cpp )

add_executable( DomainTesting.UnitTests ${TEST_SOURCES} )
target_include_directories( DomainTesting.UnitTests PUBLIC ${TARGET_INCLUDE_DIR} ${TARGET_SOURCE_DIR} ${TESTS_INCLUDE_DIR} ${TESTS_SOURCE_DIR} )
target_link_libraries( DomainTesting.UnitTests ${TESTS_LINLK} )

endif( ${DomainTestingEnableTest} )

