cmake_minimum_required( VERSION 2.8 )
project( GDF )

option(VERSION_251 "Allow support of GDF 2.51" OFF)

if (VERSION_251)
        add_compile_definitions(ALLOW_GDF_V_251)
endif()

if( UNIX )
        add_definitions( -Wall -Wextra -pedantic -Werror -fPIC )
elseif( MINGW )
        add_definitions( -Wall -Wextra -pedantic -Werror )
elseif( WIN32 )
        add_definitions( -W3 )
        # Default Windows installs go to "C:\Program files" which may fail
        # if user has insufficient privelege. This if-clause puts the 
        # compile results under GDF project folder.
        set( CMAKE_INSTALL_PREFIX ${GDF_SOURCE_DIR} )
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
	include/GDF/EventConverter.h
	include/GDF/EventHeader.h
	include/GDF/EventDescriptor.h
	include/GDF/Exceptions.h
	include/GDF/GDFHeaderAccess.h
	include/GDF/HeaderItem.h
	include/GDF/MainHeader.h
	include/GDF/Modifier.h
	include/GDF/pointerpool.h
	include/GDF/Reader.h
	include/GDF/RecordBuffer.h
	include/GDF/RecordFullHandler.h
	include/GDF/Record.h
	include/GDF/SignalHeader.h
	include/GDF/TagHeader.h
	include/GDF/tools.h
	include/GDF/Types.h
	include/GDF/Version.h
	include/GDF/Writer.h
)

set( SOURCES
	src/Channel.cpp
	src/EventHeader.cpp
	src/EventDescriptor.cpp
	src/GDFHeaderAccess.cpp
	src/MainHeader.cpp
	src/Modifier.cpp
	src/Reader.cpp
	src/RecordBuffer.cpp
	src/Record.cpp
	src/SignalHeader.cpp
	src/TagHeader.cpp
	src/Types.cpp
	src/Writer.cpp
	src/EventConverter.cpp
)

add_library( GDF ${HEADERS} ${SOURCES} ${Boost_LIBRARIES} )

install( FILES ${HEADERS} DESTINATION include/GDF )

if( MSVC )
        install( FILES ${PROJECT_BINARY_DIR}/Debug/gdf.pdb
                DESTINATION lib
                CONFIGURATIONS Debug
                )
endif( MSVC )

install( TARGETS GDF
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)



