#ifndef MINIDFS_CONFIG_H
#define MINIDFS_CONFIG_H

#include <string>

namespace minidfs {

/**
 * @brief Configuration constants for MiniDFS
 */
struct Config {
    /// Default NameNode bind address
    static constexpr const char* DEFAULT_NAMENODE_ADDRESS = "localhost";
    
    /// Default DataNode bind address
    static constexpr const char* DEFAULT_DATANODE_ADDRESS = "localhost";
    
    /// Default NameNode port
    static constexpr int DEFAULT_NAMENODE_PORT = 50051;
    
    /// Default DataNode port
    static constexpr int DEFAULT_DATANODE_PORT = 50052;
    
    /// Heartbeat interval in seconds
    static constexpr int DEFAULT_HEARTBEAT_INTERVAL_SECONDS = 5;
    
    /// Default block size: 4MB
    static constexpr size_t DEFAULT_BLOCK_SIZE_BYTES = 4 * 1024 * 1024;
};

} // namespace minidfs

#endif // MINIDFS_CONFIG_H
