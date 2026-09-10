cmake_minimum_required(VERSION 3.10)
project(csae295_client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_compile_options(-Wall -O0 -g -rdynamic)

set(_UDEBUG 0 CACHE STRING "Enable debug macro (1 or 0)")
if(_UDEBUG)
    add_compile_definitions(_UDEBUG)
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../src SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../src/protocol SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../src/util SRCS)

add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(${PROJECT_NAME} pthread rt)
