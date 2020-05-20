cmake_minimum_required(VERSION 3.12)
project(statismo-ui)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

option(BUILD_EXAMPLES "Build client examples" ON)

# Dependencies
find_package(Thrift REQUIRED)

find_package(statismo 0.12.0 REQUIRED)
include(${STATISMO_USE_FILE})

# Thrift files generation and target construction
set(_generated_dir ${PROJECT_SOURCE_DIR}/src/thrift)

set(_generated_src 
  ${_generated_dir}/UI.cpp 
  ${_generated_dir}/ui_types.cpp 
  ${_generated_dir}/ui_constants.cpp
)

set(_src 
  ${PROJECT_SOURCE_DIR}/src/StatismoUI.h
  ${PROJECT_SOURCE_DIR}/src/StatismoUI.cpp
)

file(MAKE_DIRECTORY ${_generated_dir})
add_custom_command(
  OUTPUT ${_generated_src}
  COMMAND ${THRIFT_COMPILER} -gen cpp -out ${_generated_dir} ${PROJECT_SOURCE_DIR}/src/ui.thrift
)

# _ notation for coherence with statismo
add_library(statismo_ui ${_src} ${_generated_src})
target_include_directories(statismo_ui 
  PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include/>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp
    ${THRIFT_INCLUDE_DIR}
)
target_compile_features(statismo_ui PUBLIC cxx_std_17)
target_link_libraries(statismo_ui 
  PUBLIC ${STATISMO_LIBRARIES} ${ITK_LIBRARIES}
  PRIVATE ${THRIFT_STATIC_LIB})

# Examples
if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

# Install
install(FILES
  "src/StatismoUI.h"
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ COMPONENT dev
)

install(TARGETS statismo_ui DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ COMPONENT dev)
