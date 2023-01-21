cmake_minimum_required( VERSION 3.12 )
project( gdf_merger )

if( UNIX )
	add_definitions( -Wall -Wextra -pedantic -Werror -fPIC)
elseif( MINGW )
	add_definitions( -Wall -Wextra -pedantic -Werror )
elseif( WIN32 )
	add_definitions( -W3 )
endif( UNIX )

if( WIN32 )
	set(Boost_USE_STATIC_LIBS        ON)
	set(Boost_USE_MULTITHREADED      ON)
	set(Boost_USE_STATIC_RUNTIME    OFF)
endif( WIN32 )
find_package( Boost COMPONENTS program_options filesystem system )

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

#message( ${Boost_LIBRARIES}  )
add_executable( gdf_merger ${HEADERS} ${SOURCES} )
target_link_libraries( gdf_merger ${Boost_LIBRARIES} GDF)	

INSTALL( TARGETS gdf_merger
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

