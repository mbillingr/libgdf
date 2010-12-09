cmake_minimum_required( VERSION 2.6 )
project( gdf_merger )

if( UNIX )
	add_definitions( -Wall -Wextra -pedantic -Werror -fPIC)
elseif( MINGW )
	add_definitions( -Wall -Wextra -pedantic -Werror )
elseif( WIN32 )
	add_definitions( -W3 )
endif( UNIX )

find_package( Boost 1.36.0 COMPONENTS date_time filesystem system program_options )

include_directories(
	../libgdf/include
	${Boost_INCLUDE_DIR}
)

set( SOURCES
	testtest.cpp
)

add_executable( testok EXCLUDE_FROM_ALL testOK.cpp )
target_link_libraries( testok ${Boost_LIBRARIES} GDF )

add_executable( testfail EXCLUDE_FROM_ALL testFail.cpp )
target_link_libraries( testfail ${Boost_LIBRARIES} GDF )

add_executable( testCreateGDF EXCLUDE_FROM_ALL testCreateGDF.cpp )
target_link_libraries( testCreateGDF ${Boost_LIBRARIES} GDF )

add_test( NAME testok COMMAND testok )
add_test( NAME testfail COMMAND testfail )
add_test( NAME testCreateGDF COMMAND testCreateGDF )

add_custom_target( check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS testok testfail testCreateGDF )
