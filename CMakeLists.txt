cmake_minimum_required( VERSION 2.6 )
project( GDF )

option( BUILD_SHARED_LIBS on )

add_subdirectory( libgdf )
add_subdirectory( tools )
