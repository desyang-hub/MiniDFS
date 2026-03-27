#include "common/config.h"

namespace minidfs {
    const std::string Config::DEFAULT_NAMENODE_ADDRESS = "localhost";
    const std::string Config::DEFAULT_DATANODE_ADDRESS = "localhost";
    const int Config::DEFAULT_NAMENODE_PORT = 50051;
    const int Config::DEFAULT_DATANODE_PORT = 50052;
    const int Config::DEFAULT_HEARTBEAT_INTERVAL_SECONDS = 5;
    const size_t Config::DEFAULT_BLOCK_SIZE_BYTES = 4 * 1024 * 1024; // 4MB
}
