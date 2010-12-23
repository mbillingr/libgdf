cmake_minimum_required( VERSION 2.6 )
project( gdf_tests )

if( UNIX )
	add_definitions( -Wall -Wextra -pedantic -Werror -fPIC)
elseif( MINGW )
	add_definitions( -Wall -Wextra -pedantic -Werror )
elseif( WIN32 )
	add_definitions( -W3 )
endif( UNIX )

configure_file( config-tests.h.in config-tests.h )

find_package( Boost 1.36.0 COMPONENTS date_time filesystem system program_options )

include_directories(
	../libgdf/include
	${Boost_INCLUDE_DIR}
	${gdf_tests_BINARY_DIR}
)

add_executable( testok testOK.cpp )
target_link_libraries( testok ${Boost_LIBRARIES} GDF )
add_test( NAME testok COMMAND testok )

add_executable( testfail testFail.cpp )
target_link_libraries( testfail ${Boost_LIBRARIES} GDF )
add_test( NAME testfail COMMAND testfail )
set_tests_properties( testfail PROPERTIES WILL_FAIL TRUE )

add_executable( testCreateGDF testCreateGDF.cpp )
target_link_libraries( testCreateGDF ${Boost_LIBRARIES} GDF )
add_test( NAME testCreateGDF COMMAND testCreateGDF )

add_executable( testRWConsistency testRWConsistency.cpp )
target_link_libraries( testRWConsistency ${Boost_LIBRARIES} GDF )
add_test( NAME testRWConsistency COMMAND testRWConsistency )


#add_custom_target( buildtests DEPENDS testCreateGDF testRWConsistency )
#add_custom_target( check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS buildtests )
