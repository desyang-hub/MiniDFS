# MiniDFS 分布式文件系统 - 开发环境配置指南

## 目录

1. [项目概述](#项目概述)
2. [系统要求](#系统要求)
3. [依赖项说明](#依赖项说明)
4. [环境配置步骤](#环境配置步骤)
5. [构建与编译](#构建与编译)
6. [运行说明](#运行说明)
7. [测试指南](#测试指南)
8. [开发工具配置](#开发工具配置)
9. [常见问题排查](#常见问题排查)
10. [项目结构详解](#项目结构详解)

---

## 项目概述

MiniDFS 是一个基于 C++17 的分布式文件系统实现，采用经典的主从架构设计：

- **NameNode（名称节点）**：管理文件系统命名空间和元数据
- **DataNode（数据节点）**：存储实际数据块
- **Client（客户端）**：提供用户接口进行文件操作

本项目使用 gRPC 进行组件间通信，Protocol Buffers 定义数据格式，spdlog 进行日志记录。

---

## 系统要求

### 硬件要求
- **CPU**: 多核处理器（推荐 4 核以上）
- **内存**: 最低 2GB，推荐 4GB 以上
- **磁盘**: 至少 1GB 可用空间（用于构建和存储数据）

### 操作系统支持
- **Linux**: Ubuntu 18.04+, CentOS 7+, Debian 9+
- **macOS**: 10.14+
- **Windows**: Windows 10+ (需 WSL2 或 MinGW)

### 基础软件要求
| 软件 | 最低版本 | 推荐版本 | 用途 |
|------|---------|---------|------|
| CMake | 3.20 | 3.25+ | 构建系统 |
| GCC | 7.0 | 11.0+ | C++ 编译器 |
| Clang | 6.0 | 14.0+ | C++ 编译器（可选） |
| Git | 2.0 | 2.30+ | 版本控制 |
| Protocol Buffers | 3.15 | 3.21+ | 序列化协议 |
| Python | 3.6 | 3.9+ | 构建脚本（可选） |

---

## 依赖项说明

本项目通过 CMake FetchContent 自动管理以下依赖：

### 核心依赖

| 依赖 | 版本 | 用途 | 获取方式 |
|------|------|------|---------|
| **gRPC** | v1.50.1 | RPC 通信框架 | 自动下载 |
| **Protocol Buffers** | (随 gRPC) | 数据序列化 | 自动下载 |
| **spdlog** | v1.11.0 | 日志库 | 自动下载 |
| **CLI11** | v2.3.2 | 命令行解析 | 自动下载 |
| **GoogleTest** | release-1.12.1 | 单元测试框架 | 自动下载 |

### 系统级依赖

构建前需要安装以下系统包：

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libssl-dev \
    pkg-config \
    autoconf \
    automake \
    libtool \
    curl \
    unzip
```

#### CentOS/RHEL
```bash
sudo yum groupinstall -y "Development Tools"
sudo yum install -y \
    cmake \
    git \
    openssl-devel \
    pkgconfig \
    autoconf \
    automake \
    libtool \
    curl \
    unzip
```

#### macOS
```bash
brew install \
    cmake \
    git \
    openssl \
    autoconf \
    automake \
    libtool \
    curl
```

---

## 环境配置步骤

### 步骤 1: 克隆项目

```bash
git clone <repository-url>
cd MiniDFS
```

### 步骤 2: 验证系统环境

```bash
# 检查 CMake 版本
cmake --version  # 应 >= 3.20

# 检查 C++ 编译器
g++ --version    # 应支持 C++17

# 检查 Git
git --version
```

### 步骤 3: 创建构建目录

```bash
mkdir -p build
cd build
```

### 步骤 4: 配置 CMake

#### 基本配置
```bash
cmake ..
```

#### 高级配置选项

```bash
# 指定编译器
CC=gcc CXX=g++ cmake ..

# 指定构建类型
cmake -DCMAKE_BUILD_TYPE=Release ..

# 指定安装路径
cmake -DCMAKE_INSTALL_PREFIX=/opt/minidfs ..

# 启用编译数据库（IDE 支持）
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

# 自定义依赖版本（不推荐，除非有特殊需求）
cmake -DgRPC_TAG=v1.50.1 ..
```

### 步骤 5: 首次构建（下载依赖）

```bash
# 使用所有 CPU 核心并行编译
make -j$(nproc)

# 或使用 ninja（如果已安装）
ninja -j$(nproc)
```

**注意**: 首次构建会下载所有依赖，可能需要较长时间（5-15 分钟），具体时间取决于网络状况。

---

## 构建与编译

### 构建目标

| 目标 | 命令 | 说明 |
|------|------|------|
| 全部构建 | `make` | 编译所有组件 |
| NameNode | `make minidfs-namenode` | 仅编译名称节点 |
| DataNode | `make minidfs-datanode` | 仅编译数据节点 |
| Client | `make minidfs-client` | 仅编译客户端 |
| 测试 | `make minidfs_tests` | 编译测试程序 |
| 清理 | `make clean` | 删除构建文件 |
| 完全清理 | `make distclean` | 删除整个 build 目录 |

### 构建类型

```bash
# Debug 模式（包含调试信息）
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release 模式（优化发布）
cmake -DCMAKE_BUILD_TYPE=Release ..

# RelWithDebInfo 模式（优化 + 调试信息）
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..

# MinSizeRel 模式（最小体积）
cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
```

### 输出目录结构

构建完成后，`build` 目录结构如下：

```
build/
├── bin/                    # 可执行文件
│   ├── minidfs-namenode    # NameNode 程序
│   ├── minidfs-datanode    # DataNode 程序
│   ├── minidfs-client      # Client 程序
│   └── minidfs_tests       # 测试程序
├── lib/                    # 库文件
│   ├── libproto_lib.a      # Proto 静态库
│   └── libcommon_lib.a     # 公共库
├── generated/              # 生成的代码
│   ├── dfs.pb.cc/.h        # Proto 消息类
│   └── dfs.grpc.pb.cc/.h   # gRPC 服务类
├── _deps/                  # 下载的依赖源码
└── compile_commands.json   # 编译数据库（IDE 使用）
```

---

## 运行说明

### 启动顺序

1. **首先启动 NameNode**
2. **然后启动一个或多个 DataNode**
3. **最后使用 Client 进行操作**

### 启动 NameNode

```bash
cd build/bin
./minidfs-namenode [选项]
```

**选项**:
- `--address`: 绑定地址（默认：localhost）
- `--port`: 监听端口（默认：50051）
- `--log-level`: 日志级别（debug/info/warn/error）

**示例**:
```bash
./minidfs-namenode --address 0.0.0.0 --port 50051 --log-level info
```

### 启动 DataNode

```bash
./minidfs-datanode [选项]
```

**选项**:
- `--address`: 绑定地址（默认：localhost）
- `--port`: 监听端口（默认：50052）
- `--namenode-address`: NameNode 地址（默认：localhost）
- `--namenode-port`: NameNode 端口（默认：50051）
- `--data-dir`: 数据存储目录（默认：../data）
- `--heartbeat-interval`: 心跳间隔秒数（默认：5）

**示例**:
```bash
./minidfs-datanode \
    --address 0.0.0.0 \
    --port 50052 \
    --namenode-address localhost \
    --namenode-port 50051 \
    --data-dir /tmp/minidfs_data \
    --heartbeat-interval 5
```

### 使用 Client

```bash
./minidfs-client [命令] [选项]
```

**可用命令**:
- `put <local_file> <remote_path>`: 上传文件
- `get <remote_path> <local_file>`: 下载文件
- `ls <path>`: 列出目录
- `rm <path>`: 删除文件
- `mkdir <path>`: 创建目录
- `cat <path>`: 查看文件内容

**选项**:
- `--namenode-address`: NameNode 地址（默认：localhost）
- `--namenode-port`: NameNode 端口（默认：50051）

**示例**:
```bash
# 上传文件
./minidfs-client put ./test.txt /files/test.txt

# 下载文件
./minidfs-client get /files/test.txt ./downloaded.txt

# 列出目录
./minidfs-client ls /files

# 查看文件
./minidfs-client cat /files/test.txt
```

### 多节点部署示例

```bash
# 终端 1: 启动 NameNode
./minidfs-namenode --port 50051

# 终端 2: 启动 DataNode 1
./minidfs-datanode --port 50052 --data-dir /tmp/data1

# 终端 3: 启动 DataNode 2
./minidfs-datanode --port 50053 --data-dir /tmp/data2

# 终端 4: 使用 Client
./minidfs-client put file.txt /test/file.txt
```

---

## 测试指南

### 运行所有测试

```bash
cd build
ctest
```

### 运行测试并显示详细信息

```bash
ctest --verbose
```

### 运行特定测试

```bash
# 运行 NameNode 测试
./bin/minidfs_tests --gtest_filter="NameNodeTest.*"

# 运行 DataNode 测试
./bin/minidfs_tests --gtest_filter="DataNodeTest.*"

# 运行 Client 测试
./bin/minidfs_tests --gtest_filter="ClientTest.*"

# 排除特定测试
./bin/minidfs_tests --gtest_filter="-*SlowTest*"
```

### 测试覆盖率（需要 gcov）

```bash
# 配置时启用覆盖率
cmake -DCMAKE_BUILD_TYPE=Debug -DCODE_COVERAGE=ON ..
make -j$(nproc)

# 运行测试
ctest

# 生成覆盖率报告
gcovr --root .. --html --html-details -o coverage.html
```

---

## 开发工具配置

### Visual Studio Code

1. **安装扩展**:
   - C/C++ (Microsoft)
   - CMake Tools (Microsoft)
   - CMake (twxs)

2. **配置 settings.json**:
```json
{
    "C_Cpp.default.compileCommands": "${workspaceFolder}/build/compile_commands.json",
    "cmake.configureOnOpen": true,
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.cmakePath": "cmake"
}
```

3. **配置 tasks.json** (可选):
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build MiniDFS",
            "type": "shell",
            "command": "cmake --build ${workspaceFolder}/build -j${env:NUMBER_OF_PROCESSORS}",
            "group": "build"
        },
        {
            "label": "Run Tests",
            "type": "shell",
            "command": "ctest --test-dir ${workspaceFolder}/build",
            "group": "test"
        }
    ]
}
```

### CLion

1. 打开项目时自动识别 CMakeLists.txt
2. 设置 → Build, Execution, Deployment → CMake
3. 确保 "Build directory" 设置为 `build`
4. CLion 会自动生成 compile_commands.json

### Vim/Neovim

使用 vim-lsp 或 coc.nvim:

```vim
" .vimrc 配置
set compilationdb=${workspaceFolder}/build/compile_commands.json
```

### 调试配置

#### GDB 调试

```bash
# 调试 NameNode
gdb ./bin/minidfs-namenode

# 调试 DataNode
gdb ./bin/minidfs-datanode

# 调试 Client
gdb ./bin/minidfs-client
```

#### VSCode 调试配置 (.vscode/launch.json)

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug NameNode",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/bin/minidfs-namenode",
            "args": ["--log-level", "debug"],
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        },
        {
            "name": "Debug DataNode",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/bin/minidfs-datanode",
            "args": ["--log-level", "debug"],
            "cwd": "${workspaceFolder}",
            "environment": []
        },
        {
            "name": "Debug Client",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/bin/minidfs-client",
            "args": ["put", "test.txt", "/test.txt"],
            "cwd": "${workspaceFolder}",
            "environment": []
        }
    ]
}
```

---

## 常见问题排查

### 问题 1: CMake 版本过低

**错误**: `CMake 3.20 or higher is required`

**解决**:
```bash
# Ubuntu/Debian
sudo apt-get remove cmake
wget https://github.com/Kitware/CMake/releases/download/v3.25.0/cmake-3.25.0-linux-x86_64.sh
sudo sh cmake-3.25.0-linux-x86_64.sh --prefix=/usr/local --skip-license

# 或使用 snap
sudo snap install cmake --classic
```

### 问题 2: 依赖下载失败

**错误**: `Failed to download dependency`

**解决**:
```bash
# 清除缓存重新下载
rm -rf build/_deps
cd build && cmake ..

# 使用国内镜像（中国用户）
export https_proxy=http://proxy-server:port
# 或手动下载依赖后放置到对应目录
```

### 问题 3: protoc 未找到

**错误**: `protoc not found`

**解决**:
```bash
# 等待 gRPC 构建完成，protoc 会在 _deps/grpc-build 中生成
# 或者安装系统 protoc
sudo apt-get install protobuf-compiler

# 验证
which protoc
protoc --version
```

### 问题 4: 编译错误 - C++17 特性不支持

**错误**: `error: 'filesystem' is not a namespace-name`

**解决**:
```bash
# 确保使用支持 C++17 的编译器
g++ --version  # 应 >= 7.0

# 重新配置
cmake -DCMAKE_CXX_STANDARD=17 ..
make clean
make -j$(nproc)
```

### 问题 5: 链接错误 - 找不到 spdlog

**错误**: `undefined reference to spdlog::...`

**解决**:
```bash
# 清理并重新构建
rm -rf build
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 问题 6: 运行时连接失败

**错误**: `Connection refused`

**解决**:
1. 确保 NameNode 已启动
2. 检查端口是否被占用：`netstat -tlnp | grep 50051`
3. 检查防火墙设置
4. 确认地址配置正确

### 问题 7: 测试失败

**错误**: 各种测试断言失败

**解决**:
```bash
# 查看详细错误信息
ctest --verbose

# 单独运行失败的测试
./bin/minidfs_tests --gtest_filter="具体测试名"

# 检查是否有残留数据
rm -rf /tmp/minidfs_*
```

---

## 项目结构详解

```
MiniDFS/
├── CMakeLists.txt              # 主构建配置文件
│                               # - 定义项目、编译器标准
│                               # - FetchContent 管理依赖
│                               # - 编译 proto 文件
│                               # - 定义库和可执行文件
│
├── README.md                   # 项目说明文档
│
├── data/                       # 默认数据存储目录
│                               # - DataNode 在此存储数据块
│
├── include/                    # 公共头文件目录
│   ├── client.h                # Client 类定义
│   │                           # - 文件操作方法声明
│   │                           # - gRPC 客户端接口
│   │
│   ├── datanode.h              # DataNode 类定义
│   │                           # - 数据块存储接口
│   │                           # - gRPC 服务实现
│   │
│   ├── namenode.h              # NameNode 类定义
│   │                           # - 元数据管理接口
│   │                           # - DataNode 注册管理
│   │
│   └── common/                 # 公共组件
│       ├── config.h            # 配置常量定义
│       │                       # - 默认地址、端口
│       │                       # - 块大小、心跳间隔
│       │
│       └── logger.h            # 日志工具定义
│                               # - spdlog 封装
│
├── proto/                      # Protocol Buffers 定义
│   └── dfs.proto               # DFS 服务协议
│                               # - 消息定义（FileMetadata, BlockLocation等）
│                               # - 服务定义（NameNodeService, DataNodeService）
│
├── src/                        # 源代码目录
│   ├── main_namenode.cpp       # NameNode 入口
│   │                           # - 参数解析
│   │                           # - 服务启动
│   │
│   ├── main_datanode.cpp       # DataNode 入口
│   │                           # - 参数解析
│   │                           # - 连接 NameNode
│   │
│   ├── main_client.cpp         # Client 入口
│   │                           # - CLI11 命令行解析
│   │                           # - 命令分发
│   │
│   ├── namenode.cpp            # NameNode 实现
│   │                           # - 元数据管理
│   │                           # - DataNode 心跳处理
│   │
│   ├── datanode.cpp            # DataNode 实现
│   │                           # - 数据块读写
│   │                           # - 心跳发送
│   │
│   ├── client.cpp              # Client 实现
│   │                           # - 文件操作逻辑
│   │                           # - gRPC 调用
│   │
│   └── common/                 # 公共组件实现
│       └── logger.cpp          # Logger 实现
│
└── tests/                      # 测试代码
    ├── main_test.cpp           # 测试入口
    ├── test_namenode.cpp       # NameNode 单元测试
    ├── test_datanode.cpp       # DataNode 单元测试
    └── test_client.cpp         # Client 单元测试
```

### 关键文件说明

#### CMakeLists.txt
- **行 1-6**: 项目定义和 C++ 标准设置
- **行 14-52**: FetchContent 声明和下载依赖
- **行 56-125**: Proto 文件编译和 proto_lib 库定义
- **行 128-148**: common_lib 库定义
- **行 151-161**: 可执行文件定义
- **行 164-188**: 测试配置

#### proto/dfs.proto
- **行 6-10**: FileMetadata 消息 - 文件元数据
- **行 13-17**: DataNodeInfo 消息 - 节点信息
- **行 93-105**: NameNodeService - NameNode RPC 服务
- **行 108-114**: DataNodeService - DataNode RPC 服务

#### include/common/config.h
- 所有配置均为 `constexpr`，零运行时开销
- 可根据需要修改默认值

---

## 性能调优建议

### 编译优化

```bash
# 启用 LTO (Link Time Optimization)
cmake -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON ..

# 针对本机优化
CXXFLAGS="-march=native -O3" cmake ..
```

### 运行时优化

1. **调整心跳间隔**: 根据网络状况调整
2. **增加块大小**: 大文件传输更高效
3. **使用 SSD**: 提升 DataNode I/O 性能

---

## 贡献指南

### 代码风格

- 遵循 Google C++ Style Guide
- 使用 clang-format 格式化代码
- 添加必要的注释和文档

### 提交规范

```bash
# Feature
git commit -m "feat: add new block replication feature"

# Bug fix
git commit -m "fix: resolve connection timeout issue"

# Documentation
git commit -m "docs: update configuration guide"

# Refactor
git commit -m "refactor: simplify NameNode metadata management"
```

### 测试要求

- 新功能必须包含单元测试
- 确保所有现有测试通过
- 代码覆盖率不低于 80%

---

## 许可证

本项目采用 MIT 许可证。

---

## 联系方式

如有问题，请提交 Issue 或联系维护者。

---

*最后更新: 2024 年*
