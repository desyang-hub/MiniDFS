#ifndef MINIDFS_NAMENODE_H
#define MINIDFS_NAMENODE_H

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <grpcpp/grpcpp.h>
#include "dfs.grpc.pb.h"
#include "common/logger.h"

namespace minidfs {

/**
 * @brief NameNode - The master node that manages filesystem metadata and DataNodes
 * 
 * Responsibilities:
 * - Maintain filesystem namespace and metadata
 * - Track registered DataNodes via heartbeats
 * - Manage file-to-block mappings
 * - Handle client metadata operations
 */
class NameNode final : public NameNodeService::Service {
public:
    explicit NameNode(const std::string& address);
    ~NameNode() override;

    // Disable copy
    NameNode(const NameNode&) = delete;
    NameNode& operator=(const NameNode&) = delete;

    /**
     * @brief Start the NameNode gRPC server
     */
    void start();

    /**
     * @brief Stop the NameNode gRPC server
     */
    void stop();

private:
    // gRPC service implementations
    grpc::Status RegisterDataNode(
        grpc::ServerContext* context,
        const RegisterRequest* request,
        RegisterResponse* response) override;

    grpc::Status Heartbeat(
        grpc::ServerContext* context,
        const HeartbeatRequest* request,
        HeartbeatResponse* response) override;

    grpc::Status GetBlockLocations(
        grpc::ServerContext* context,
        const GetBlockLocationsRequest* request,
        GetBlockLocationsResponse* response) override;

    grpc::Status AddBlock(
        grpc::ServerContext* context,
        const AddBlockRequest* request,
        AddBlockResponse* response) override;

    // Member variables
    std::string address_;
    std::unique_ptr<grpc::Server> server_;
    std::unordered_map<std::string, FileMetadata> file_metadata_;
    std::vector<DataNodeInfo> data_nodes_;
    mutable std::shared_mutex metadata_mutex_;  ///< Protects file_metadata_ and data_nodes_
};

} // namespace minidfs

#endif // MINIDFS_NAMENODE_H
