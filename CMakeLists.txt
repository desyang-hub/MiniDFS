cmake_minimum_required(VERSION 3.20)
project(MiniDFS CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent module
include(FetchContent)

# Fetch Protobuf
# FetchContent_Declare(
#     protobuf
#     GIT_REPOSITORY git@github.com:protocolbuffers/protobuf.git
#     GIT_TAG        v3.21.12
# )
# FetchContent_MakeAvailable(protobuf)

# Fetch gRPC
FetchContent_Declare(
    grpc
    GIT_REPOSITORY git@github.com:grpc/grpc.git
    GIT_TAG        v1.50.1
)
FetchContent_MakeAvailable(grpc)

# Fetch spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY git@github.com:gabime/spdlog.git
    GIT_TAG        v1.11.0
)
FetchContent_MakeAvailable(spdlog)

# Fetch CLI11
FetchContent_Declare(
    CLI11
    GIT_REPOSITORY git@github.com:CLIUtils/CLI11.git
    GIT_TAG        v2.3.2
)
FetchContent_MakeAvailable(CLI11)

# --- gRPC (包含 protobuf) ---
# 关键设置：关闭 gRPC 的测试构建，避免 gtest 冲突
set(gRPC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(gRPC_INSTALL OFF CACHE BOOL "" FORCE)

# Fetch GoogleTest
# FetchContent_Declare(
#     googletest
#     GIT_REPOSITORY git@github.com:google/googletest.git
#     GIT_TAG        release-1.12.1
# )
# FetchContent_MakeAvailable(googletest)

# Find Protobuf
# find_package(Protobuf REQUIRED)
# find_package(gRPC CONFIG REQUIRED)
# find_package(Protobuf CONFIG REQUIRED) # 这会自动指向 gRPC 内部的 protobuf

# Generate protobuf files
# set(PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/proto/dfs.proto)
# protobuf_generate(
#     LANGUAGE cpp
#     OUT_VAR proto_sources
#     GRPC_OUT_VAR grpc_sources
#     IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/proto
#     FILES ${PROTO_FILES}
# )

# ==========================================
# 3. 编译 Proto 文件
# ==========================================

# 1. 找到编译工具
# FetchContent 编译出的可执行文件通常在 CMAKE_RUNTIME_OUTPUT_DIRECTORY 中
find_program(_PROTOBUF_PROTOC protoc)
find_program(_GRPC_CPP_PLUGIN grpc_cpp_plugin)

# 2. 如果没找到，尝试去 _deps 目录硬找 (针对 FetchContent 的特殊路径)
if(NOT _PROTOBUF_PROTOC)
    set(_PROTOBUF_PROTOC "${CMAKE_BINARY_DIR}/_deps/grpc-build/third_party/protobuf/protoc")
    if(NOT EXISTS ${_PROTOBUF_PROTOC})
        # 再试一个路径 (取决于 CMake 版本和构建方式)
        set(_PROTOBUF_PROTOC "${CMAKE_BINARY_DIR}/_deps/grpc-src/third_party/protobuf/cmake/protoc")
    endif()
endif()

if(NOT _GRPC_CPP_PLUGIN)
    set(_GRPC_CPP_PLUGIN "${CMAKE_BINARY_DIR}/_deps/grpc-build/grpc_cpp_plugin")
    if(NOT EXISTS ${_GRPC_CPP_PLUGIN})
         set(_GRPC_CPP_PLUGIN "${CMAKE_BINARY_DIR}/_deps/grpc-src/grpc_cpp_plugin")
    endif()
endif()

# 3. 最终检查
if(NOT EXISTS ${_PROTOBUF_PROTOC})
    message(FATAL_ERROR "protoc not found at ${_PROTOBUF_PROTOC}. \nPlease check your build directory structure.")
endif()
if(NOT EXISTS ${_GRPC_CPP_PLUGIN})
    message(FATAL_ERROR "grpc_cpp_plugin not found at ${_GRPC_CPP_PLUGIN}.")
endif()

message(STATUS "Found protoc: ${_PROTOBUF_PROTOC}")
message(STATUS "Found grpc_cpp_plugin: ${_GRPC_CPP_PLUGIN}")

# 2. 定义路径
set(PROTO_PATH ${CMAKE_SOURCE_DIR}/proto)
set(PROTO_FILES ${PROTO_PATH}/dfs.proto)
set(PROTO_SRC_DIR ${CMAKE_BINARY_DIR}/generated)

# 3. 创建输出目录
file(MAKE_DIRECTORY ${PROTO_SRC_DIR})

# 4. 定义生成的文件名
set(PROTO_SRCS ${PROTO_SRC_DIR}/dfs.pb.cc)
set(PROTO_HDRS ${PROTO_SRC_DIR}/dfs.pb.h)
set(GRPC_SRCS ${PROTO_SRC_DIR}/dfs.grpc.pb.cc)
set(GRPC_HDRS ${PROTO_SRC_DIR}/dfs.grpc.pb.h)

# 5. 定义生成规则 (替代 protobuf_generate)
# add_custom_command(
#       OUTPUT ${PROTO_SRCS} ${PROTO_HDRS} ${GRPC_SRCS} ${GRPC_HDRS}
#       COMMAND ${_PROTOBUF_PROTOC}
#       ARGS --cpp_out=${PROTO_SRC_DIR}
#            --grpc_out=${PROTO_SRC_DIR}
#            --plugin=protoc-gen-grpc=${_GRPC_CPP_PLUGIN}
#            -I ${PROTO_PATH}
#            ${PROTO_FILES}
#       DEPENDS ${PROTO_FILES}
#       COMMENT "Generating C++ gRPC code..."
#       VERBATIM
# )

add_custom_command(
      OUTPUT ${PROTO_SRCS} ${PROTO_HDRS} ${GRPC_SRCS} ${GRPC_HDRS}
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --cpp_out=${PROTO_SRC_DIR}
           --grpc_out=${PROTO_SRC_DIR}
           --plugin=protoc-gen-grpc=${_GRPC_CPP_PLUGIN}
           -I ${PROTO_PATH}
           ${PROTO_FILES}
      # 确保这里依赖了 protoc 工具本身，防止工具没编译好就运行
      DEPENDS ${PROTO_FILES} ${_PROTOBUF_PROTOC} ${_GRPC_CPP_PLUGIN}
      COMMENT "Generating C++ gRPC code..."
      VERBATIM
)

add_library(proto_lib STATIC
    ${PROTO_SRCS}   # 对应 dfs.pb.cc
    ${GRPC_SRCS}    # 对应 dfs.grpc.pb.cc
    # 注意：头文件 (.h) 不需要写在这里，写 .cc 就够了
)

# 7. 链接 Proto 库
target_link_libraries(proto_lib
    PUBLIC
        grpc++
        libprotobuf
)

# 8. 包含目录
# 【关键修改】确保生成的头文件目录被包含进来
message(STATUS "PROTO_SRC_DIR: ${PROTO_SRC_DIR}")
target_include_directories(proto_lib PUBLIC ${PROTO_SRC_DIR})

# Create library for common components
add_library(common_lib
    ${proto_sources}
    ${grpc_sources}
    src/namenode.cpp
    src/datanode.cpp
    src/client.cpp
    src/common/config.cpp
    src/common/logger.cpp
)

target_include_directories(common_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(common_lib PRIVATE ${PROTO_SRC_DIR})

set(LIB_DEPENDENCIES
    grpc++
    # gRPC::grpc++
    grpc++_reflection
    # gRPC::grpc++_reflection
    libprotobuf
    # Protobuf::libprotobuf
    spdlog
    # spdlog::spdlog
    CLI11
    # CLI11::CLI11
)

target_link_libraries(common_lib
    grpc++
    # gRPC::grpc++
    grpc++_reflection
    # gRPC::grpc++_reflection
    libprotobuf
    # Protobuf::libprotobuf
    spdlog
    # spdlog::spdlog
    CLI11
    # CLI11::CLI11
    proto_lib
)

# Create executables
add_executable(minidfs-namenode src/main_namenode.cpp)
target_link_libraries(minidfs-namenode common_lib)

add_executable(minidfs-datanode src/main_datanode.cpp)
target_link_libraries(minidfs-datanode common_lib)

add_executable(minidfs-client src/main_client.cpp)
target_link_libraries(minidfs-client common_lib)

# Create test executable
enable_testing()
add_executable(minidfs_tests
    tests/test_namenode.cpp
    tests/test_datanode.cpp
    tests/test_client.cpp
    tests/main_test.cpp
)

target_include_directories(minidfs_tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(minidfs_tests
    common_lib
    gtest
    gtest_main
    ${LIB_DEPENDENCIES}
    # gRPC::grpc++
    # gRPC::grpc++_reflection
    # Protobuf::libprotobuf
    # spdlog::spdlog
    # CLI11::CLI11
)

add_test(NAME minidfs_tests COMMAND minidfs_tests)
