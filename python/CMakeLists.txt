
#set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake )

find_package( Cython REQUIRED )
find_package( PythonLibs REQUIRED )
#find_package( Boost REQUIRED COMPONENTS system thread )

set( cython_source_path ${CMAKE_CURRENT_SOURCE_DIR} )
set( cython_target_path ${CMAKE_CURRENT_BINARY_DIR} )
set( cython_libname pylibgdf )

set( cython_pyx ${cython_source_path}/${cython_libname}.pyx )
set( cython_cpp ${cython_target_path}/${cython_libname}.cpp )

add_custom_command(
  OUTPUT ${cython_cpp}
  COMMAND ${CYTHON_EXECUTABLE} --cplus -o ${cython_cpp} ${cython_pyx} 
  DEPENDS ${cython_pyx}
)

include_directories(
        ../libgdf/include
  ${PYTHON_INCLUDE_DIRS}
)

add_library( ${cython_libname} MODULE ${cython_cpp} )
target_link_libraries( ${cython_libname} GDF ) 

set_target_properties( ${cython_libname} PROPERTIES PREFIX "" )
