cmake_minimum_required( VERSION 2.8 )
project( GDF )

set( BUILD_SHARED_LIBS true CACHE BOOL "Wether we shall build shared or dynamic libraries." )

add_subdirectory( libgdf )
add_subdirectory( tools )

set_target_properties( GDF PROPERTIES VERSION 0.1.1 )

