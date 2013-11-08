cmake_minimum_required( VERSION 2.8 )
#project( GDF )

option( BUILD_TESTING "Build tests" OFF )

if( WIN32 )
	set( BUILD_SHARED_LIBS false CACHE BOOL "Whether we shall build shared or dynamic libraries." )
	option( BUILD_PYTHON_MODULES "Build python modules" OFF )
else( WIN32 )
	set( BUILD_SHARED_LIBS true CACHE BOOL "Whether we shall build shared or dynamic libraries." )
	option( BUILD_PYTHON_MODULES "Build python modules" OFF )
endif( WIN32 )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set( GDF_SOURCE_ROOT ${Project_SOURCE_DIR} )

add_subdirectory( libgdf )
add_subdirectory( tools )

# Make sure the version is in sync with
# libgdf/include/GDF/Version.h
set(GDF_VERSION "0.1.4")

# shared library API versioning (soversion) -- NOT the same as the release version
# it follows first number
string( REGEX MATCH "^[0-9]+" GDF_SHAREDLIB_SOVERSION ${GDF_VERSION})

set_target_properties(GDF PROPERTIES
	VERSION ${GDF_VERSION}
	SOVERSION ${GDF_SHAREDLIB_SOVERSION})

if (BUILD_TESTING)
	include( CTest )
	enable_testing( )
	add_subdirectory( test )
endif()

if( BUILD_PYTHON_MODULES )
	add_subdirectory( python )
endif()
