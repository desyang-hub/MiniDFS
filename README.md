# 分布式文件系统项目说明

## 项目概述

这是一个基于C++的分布式文件系统实现，采用主从架构设计，包含名称节点(Namenode)、数据节点(Datanode)和客户端(Client)三个核心组件。该系统实现了分布式文件系统的典型功能，包括文件存储、目录管理、数据复制等。

## 项目结构

```
.
├── CMakeLists.txt              # CMake构建配置文件
├── data/                       # 数据存储目录
├── include/                    # 头文件目录
│   ├── client.h                # 客户端接口头文件
│   ├── datanode.h              # 数据节点接口头文件
│   ├── namenode.h              # 名称节点接口头文件
│   └── common/                 # 公共组件头文件
│       ├── config.h            # 配置管理头文件
│       └── logger.h            # 日志管理头文件
├── proto/                      # 协议定义目录
│   └── dfs.proto               # Protocol Buffers协议定义文件
├── src/                        # 源代码目录
│   ├── client.cpp              # 客户端实现
│   ├── datanode.cpp            # 数据节点实现
│   ├── main_client.cpp         # 客户端主程序
│   ├── main_datanode.cpp       # 数据节点主程序
│   ├── main_namenode.cpp       # 名称节点主程序
│   ├── namenode.cpp            # 名称节点实现
│   └── common/                 # 公共组件实现
│       ├── config.cpp          # 配置管理实现
│       └── logger.cpp          # 日志管理实现
├── tests/                      # 测试目录
│   ├── CMakeLists.txt          # 测试构建配置文件
│   ├── main_test.cpp           # 测试主程序
│   ├── test_client.cpp         # 客户端测试
│   ├── test_datanode.cpp       # 数据节点测试
│   └── test_namenode.cpp       # 名称节点测试
└── README.md                   # 项目说明文件（当前文件）
```

## 核心组件

### 1. 名称节点 (Namenode)
- 负责管理文件系统的命名空间和元数据
- 维护文件到数据块的映射关系
- 管理数据节点的心跳和状态
- 处理客户端的元数据操作请求

### 2. 数据节点 (Datanode)
- 存储实际的数据块
- 定期向名称节点发送心跳信息
- 响应名称节点的数据块读写请求
- 执行数据块的复制和删除操作

### 3. 客户端 (Client)
- 提供文件系统接口给用户程序
- 与名称节点交互获取文件元数据
- 直接与数据节点交互进行数据读写
- 处理文件的创建、读取、删除等操作

## 技术特点

### 架构设计
- **分布式架构**：采用主从式架构，名称节点负责管理，数据节点负责存储
- **模块化设计**：各组件职责清晰，便于维护和扩展
- **高可用性**：通过心跳机制监控节点状态

### 技术栈
- **编程语言**：C++
- **协议处理**：Protocol Buffers (protobuf)
- **构建工具**：CMake
- **测试框架**：自定义测试框架

## 构建与运行

### 编译要求
- C++11或更高版本
- CMake 3.0或更高版本
- Protocol Buffers编译器

### 构建步骤
```bash
mkdir build
cd build
cmake ..
make
```

### 运行方式
1. 启动名称节点：
   ```bash
   ./bin/namenode
   ```

2. 启动数据节点：
   ```bash
   ./bin/datanode
   ```

3. 使用客户端：
   ```bash
   ./bin/client
   ```

## 测试说明

项目包含完整的单元测试，覆盖所有核心组件的功能测试：
- `test_client.cpp`：客户端功能测试
- `test_datanode.cpp`：数据节点功能测试  
- `test_namenode.cpp`：名称节点功能测试

运行测试：
```bash
cd build
make test
```

## 配置文件

配置信息通过 `include/common/config.h` 和 `src/common/config.cpp` 管理，包括：
- 节点端口配置
- 数据存储路径
- 日志级别设置
- 心跳间隔等参数

## 扩展性

该系统设计具有良好的扩展性：
- 支持添加新的数据节点
- 可以轻松扩展文件系统功能
- 模块化设计便于功能重构和优化

## 未来改进方向

1. 增加故障恢复机制
2. 实现负载均衡策略
3. 添加数据备份和快照功能
4. 支持更多文件系统操作
5. 提升系统性能和并发处理能力

## 贡献指南

欢迎贡献代码和改进建议。请提交Issue或Pull Request。

---
*本项目为分布式文件系统学习与实践项目*