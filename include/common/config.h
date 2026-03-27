#ifndef MINIDFS_CONFIG_H
#define MINIDFS_CONFIG_H

#include <string>

namespace minidfs {
    struct Config {
        static const std::string DEFAULT_NAMENODE_ADDRESS;
        static const std::string DEFAULT_DATANODE_ADDRESS;
        static const int DEFAULT_NAMENODE_PORT;
        static const int DEFAULT_DATANODE_PORT;
        static const int DEFAULT_HEARTBEAT_INTERVAL_SECONDS;
        static const size_t DEFAULT_BLOCK_SIZE_BYTES;
    };
}

#endif // MINIDFS_CONFIG_H
