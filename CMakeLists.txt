cmake_minimum_required(VERSION 3.20)
project(MiniDFS CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include FetchContent module
include(FetchContent)

# ==========================================
# Fetch Dependencies
# ==========================================

# Fetch gRPC
set(gRPC_BUILD_TESTS OFF CACHE BOOL "Disable gRPC tests" FORCE)
set(gRPC_INSTALL OFF CACHE BOOL "Disable gRPC install" FORCE)
FetchContent_Declare(
    grpc
    GIT_REPOSITORY https://github.com/grpc/grpc.git
    GIT_TAG        v1.50.1
)
FetchContent_MakeAvailable(grpc)

# Fetch spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.11.0
)
FetchContent_MakeAvailable(spdlog)

# Fetch CLI11
FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG        v2.3.2
)
FetchContent_MakeAvailable(CLI11)

# Fetch GoogleTest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# ==========================================
# Compile Proto files
# ==========================================

find_program(PROTOC protoc)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)

if(NOT PROTOC)
    set(PROTOC "${CMAKE_BINARY_DIR}/_deps/grpc-build/third_party/protobuf/protoc")
    if(NOT EXISTS ${PROTOC})
        set(PROTOC "${CMAKE_BINARY_DIR}/_deps/grpc-src/third_party/protobuf/cmake/protoc")
    endif()
endif()

if(NOT GRPC_CPP_PLUGIN)
    set(GRPC_CPP_PLUGIN "${CMAKE_BINARY_DIR}/_deps/grpc-build/grpc_cpp_plugin")
    if(NOT EXISTS ${GRPC_CPP_PLUGIN})
         set(GRPC_CPP_PLUGIN "${CMAKE_BINARY_DIR}/_deps/grpc-src/grpc_cpp_plugin")
    endif()
endif()

if(NOT EXISTS ${PROTOC})
    message(FATAL_ERROR "protoc not found at ${PROTOC}")
endif()
if(NOT EXISTS ${GRPC_CPP_PLUGIN})
    message(FATAL_ERROR "grpc_cpp_plugin not found at ${GRPC_CPP_PLUGIN}")
endif()

message(STATUS "Found protoc: ${PROTOC}")
message(STATUS "Found grpc_cpp_plugin: ${GRPC_CPP_PLUGIN}")

# Define paths
set(PROTO_PATH ${CMAKE_SOURCE_DIR}/proto)
set(PROTO_FILES ${PROTO_PATH}/dfs.proto)
set(PROTO_SRC_DIR ${CMAKE_BINARY_DIR}/generated)

# Create output directory
file(MAKE_DIRECTORY ${PROTO_SRC_DIR})

# Define generated file names
set(PROTO_SRCS ${PROTO_SRC_DIR}/dfs.pb.cc)
set(PROTO_HDRS ${PROTO_SRC_DIR}/dfs.pb.h)
set(GRPC_SRCS ${PROTO_SRC_DIR}/dfs.grpc.pb.cc)
set(GRPC_HDRS ${PROTO_SRC_DIR}/dfs.grpc.pb.h)

# Generate C++ gRPC code
add_custom_command(
    OUTPUT ${PROTO_SRCS} ${PROTO_HDRS} ${GRPC_SRCS} ${GRPC_HDRS}
    COMMAND ${PROTOC}
    ARGS --cpp_out=${PROTO_SRC_DIR}
         --grpc_out=${PROTO_SRC_DIR}
         --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
         -I ${PROTO_PATH}
         ${PROTO_FILES}
    DEPENDS ${PROTO_FILES} ${PROTOC} ${GRPC_CPP_PLUGIN}
    COMMENT "Generating C++ gRPC code..."
    VERBATIM
)

# Create proto library
add_library(proto_lib STATIC
    ${PROTO_SRCS}
    ${GRPC_SRCS}
)

target_link_libraries(proto_lib
    PUBLIC
        grpc++
        libprotobuf
)

target_include_directories(proto_lib PUBLIC ${PROTO_SRC_DIR})

# ==========================================
# Common Library
# ==========================================

add_library(common_lib STATIC
    src/namenode.cpp
    src/datanode.cpp
    src/client.cpp
    src/common/config.cpp
    src/common/logger.cpp
)

target_include_directories(common_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROTO_SRC_DIR}
)

target_link_libraries(common_lib
    PUBLIC
        proto_lib
        spdlog::spdlog
)

# ==========================================
# Executables
# ==========================================

add_executable(minidfs-namenode src/main_namenode.cpp)
target_link_libraries(minidfs-namenode PRIVATE common_lib)

add_executable(minidfs-datanode src/main_datanode.cpp)
target_link_libraries(minidfs-datanode PRIVATE common_lib)

add_executable(minidfs-client src/main_client.cpp)
target_link_libraries(minidfs-client PRIVATE common_lib CLI11::CLI11)

# ==========================================
# Tests
# ==========================================

enable_testing()

add_executable(minidfs_tests
    tests/test_namenode.cpp
    tests/test_datanode.cpp
    tests/test_client.cpp
    tests/main_test.cpp
)

target_include_directories(minidfs_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROTO_SRC_DIR}
)

target_link_libraries(minidfs_tests
    PRIVATE
        common_lib
        GTest::gtest
        GTest::gtest_main
)

add_test(NAME minidfs_tests COMMAND minidfs_tests)
