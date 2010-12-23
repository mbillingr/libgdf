cmake_minimum_required( VERSION 2.8 )
#project( GDF )

option( BUILD_TESTING "Build tests" OFF )

set( BUILD_SHARED_LIBS true CACHE BOOL "Wether we shall build shared or dynamic libraries." )

set( GDF_SOURCE_ROOT ${Project_SOURCE_DIR} )

add_subdirectory( libgdf )
add_subdirectory( tools )

# shared library API versioning (soversion) -- NOT the same as the release version
# it follows first number
set(GDF_VERSION "0.1.1")
string( REGEX MATCH "^[0-9]+" GDF_SHAREDLIB_SOVERSION ${GDF_VERSION})

set_target_properties(GDF PROPERTIES
	VERSION ${GDF_VERSION}
	SOVERSION ${GDF_SHAREDLIB_SOVERSION})

if (BUILD_TESTING)
	include( CTest )
	enable_testing( )
	add_subdirectory( test )
endif()
