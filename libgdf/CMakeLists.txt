cmake_minimum_required( VERSION 2.6 )
project( GDF )

if( UNIX )
	add_definitions( -Wall -Wextra -pedantic -Werror -fPIC)
elseif( WIN32 )
	add_definitions( -W3 )
endif( UNIX )

#set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${GDF_SOURCE_DIR}/lib )
#set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GDF_SOURCE_DIR}/lib )
#set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GDF_SOURCE_DIR}/bin )

find_package( Boost REQUIRED )

include_directories(
	${GDF_SOURCE_DIR}/include
	${Boost_INCLUDE_DIR}
)

set( HEADERS 
	include/GDF/ChannelDataBase.h
	include/GDF/ChannelData.h
	include/GDF/Channel.h
	include/GDF/EventHeader.h
	include/GDF/Exceptions.h
	include/GDF/GDFHeaderAccess.h
	include/GDF/HeaderItem.h
	include/GDF/MainHeader.h
	include/GDF/Modifier.h
	include/GDF/pointerpool.h
	include/GDF/Reader.h
	include/GDF/RecordBuffer.h
	include/GDF/Record.h
	include/GDF/SignalHeader.h
	include/GDF/TagHeader.h
	include/GDF/tools.h
	include/GDF/Types.h
	include/GDF/Writer.h
)

set( SOURCES
	src/Channel.cpp
	src/EventHeader.cpp
	src/GDFHeaderAccess.cpp
	src/MainHeader.cpp
	src/Modifier.cpp
	src/Reader.cpp
	src/RecordBuffer.cpp
	src/Record.cpp
	src/SignalHeader.cpp
	src/Types.cpp
	src/Writer.cpp
)

add_library( GDF ${HEADERS} ${SOURCES} ${Boost_LIBRARIES} )

INSTALL( FILES ${HEADERS} DESTINATION include/GDF )
INSTALL( TARGETS GDF
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

