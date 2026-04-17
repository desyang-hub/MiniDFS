#ifndef MINIDFS_CLIENT_H
#define MINIDFS_CLIENT_H

#include <string>
#include <memory>
#include <cstddef>
#include <grpcpp/grpcpp.h>
#include "dfs.grpc.pb.h"
#include "common/logger.h"

namespace minidfs {

/**
 * @brief Client - DFS client for file operations
 * 
 * Provides interface for uploading and downloading files to/from the distributed filesystem.
 * Files are split into blocks and stored across multiple DataNodes.
 */
class Client {
public:
    explicit Client(const std::string& namenode_address);
    ~Client();

    // Disable copy
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    /**
     * @brief Upload a local file to the distributed filesystem
     * @param local_file Path to the local file
     * @param remote_path Path in the distributed filesystem
     * @return true if upload succeeded, false otherwise
     */
    bool put(const std::string& local_file, const std::string& remote_path);

    /**
     * @brief Download a file from the distributed filesystem
     * @param remote_path Path in the distributed filesystem
     * @param local_file Path to save the local file
     * @return true if download succeeded, false otherwise
     */
    bool get(const std::string& remote_path, const std::string& local_file);

    /// Default block size: 4MB
    static constexpr size_t BLOCK_SIZE_BYTES = 4 * 1024 * 1024;

private:
    std::shared_ptr<grpc::Channel> channel_;
    std::unique_ptr<NameNodeService::Stub> stub_;
};

} // namespace minidfs

#endif // MINIDFS_CLIENT_H
