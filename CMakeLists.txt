CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(ui)

FIND_PACKAGE(statismo REQUIRED)

FIND_PACKAGE(ITK REQUIRED)
include (${ITK_USE_FILE})


set(thrift_INCLUDE_DIR "THRIFT_INCLUDE_DIR" CACHE PATH "")
set(thrift_LIBRARY_DIR "THRIFT_LIBRARY_DIR" CACHE PATH "")
include_directories(${statismo_INCLUDE_DIRS} ${thrift_INCLUDE_DIR})
link_directories( ${ITK_LIBRARY_DIRS} ${statismo_LIBRARY_DIRS} ${thrift_LIBRARY_DIR})

add_library (statismo-ui STATIC gen-cpp/UI.cpp gen-cpp/ui_types.cpp gen-cpp/ui_constants.cpp StatismoUI.cpp)

target_link_libraries (statismo-ui ${ITK_LIBRARIES} ${statismo_LIBRARIES} thrift)

add_executable (client client.cpp)
target_link_libraries (client thrift statismo-ui)


install( FILES
    "StatismoUI.h"
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ COMPONENT dev
)

install( DIRECTORY
    "gen-cpp"
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ COMPONENT dev
)


install( TARGETS statismo-ui DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ COMPONENT dev)
