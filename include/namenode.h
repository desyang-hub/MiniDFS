#ifndef MINIDFS_NAMENODE_H
#define MINIDFS_NAMENODE_H

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <grpcpp/grpcpp.h>
#include <shared_mutex>
#include "dfs.grpc.pb.h"
#include "common/logger.h"

namespace minidfs {
    class NameNode final : public NameNodeService::Service {
    public:
        explicit NameNode(const std::string& address);
        ~NameNode();

        void start();
        void stop();

    private:
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

        std::string address_;
        std::unique_ptr<grpc::Server> server_;
        std::unordered_map<std::string, FileMetadata> file_metadata_;
        std::vector<DataNodeInfo> data_nodes_;
        std::shared_mutex metadata_mutex_;
    };
}

#endif // MINIDFS_NAMENODE_H
