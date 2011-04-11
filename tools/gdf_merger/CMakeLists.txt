cmake_minimum_required( VERSION 2.8 )
project( gdf_merger )

if( UNIX )
	add_definitions( -Wall -Wextra -pedantic -Werror -fPIC)
elseif( MINGW )
	add_definitions( -Wall -Wextra -pedantic -Werror )
elseif( WIN32 )
	add_definitions( -W3 )
endif( UNIX )

find_package( Boost COMPONENTS filesystem program_options )

include_directories(
	../../libgdf/include
	${Boost_INCLUDE_DIR}
)

set( HEADERS 
	gdfmerger.h
)

set( SOURCES
	gdfmerger.cpp
	main.cpp
)

add_executable( gdf_merger ${HEADERS} ${SOURCES} )
if( BUILD_SHARED_LIBS MATCHES true )
	target_link_libraries( gdf_merger ${Boost_LIBRARIES} GDF)
endif( BUILD_SHARED_LIBS MATCHES true )	

INSTALL( TARGETS gdf_merger
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

