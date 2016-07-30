CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(ui)

FIND_PACKAGE(statismo REQUIRED)

FIND_PACKAGE(ITK REQUIRED)
include (${ITK_USE_FILE})
	
include_directories(${statismo_INCLUDE_DIRS}) 
link_directories( ${ITK_LIBRARY_DIRS} ${statismo_LIBRARY_DIRS})

add_executable (thrift-client client.cpp gen-cpp/UI.cpp gen-cpp/ui_types.cpp gen-cpp/ui_constants.cpp)
target_link_libraries (thrift-client ${ITK_LIBRARIES} ${statismo_LIBRARIES} thrift)