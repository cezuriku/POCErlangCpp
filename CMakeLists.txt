cmake_minimum_required(VERSION 3.4)

project(pocErlCpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CPP_SERVER cpp_server)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -W -Wall -Wextra")
set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS}")
file(GLOB SOURCE_FILES "server_src/*.cpp")
file(GLOB PROTO_FILES "proto/*.proto")

# Generate protobuf
find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# Executable
add_executable(${CPP_SERVER} ${SOURCE_FILES}
               ${PROTO_SRCS} ${PROTO_HDRS} ${PROTO_FILES})

# Include protobuf
include_directories(${Protobuf_INCLUDE_DIRS})
target_link_libraries(${CPP_SERVER} ${Protobuf_LIBRARIES})
target_include_directories(${CPP_SERVER}
                           PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                           PUBLIC ${PROTOBUF_INCLUDE_DIRS})

find_package(Erlang REQUIRED)
include_directories(${ERLANG_EI_INCLUDE_PATH}
                    ${ERLANG_ERTS_INCLUDE_PATH})

find_library(ERLANG_EI_LIBRARY NAMES ei
             PATHS ${ERLANG_EI_LIBRARY_PATH})
find_library(ERLANG_ERL_INTERFACE_LIBRARY NAMES erl_interface
             PATHS ${ERLANG_EI_LIBRARY_PATH})

target_link_libraries(${CPP_SERVER} ${ERLANG_ERL_INTERFACE_LIBRARY}
                      ${ERLANG_EI_LIBRARY})
