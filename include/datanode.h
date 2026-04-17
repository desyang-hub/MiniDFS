#ifndef MINIDFS_DATANODE_H
#define MINIDFS_DATANODE_H

#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <grpcpp/grpcpp.h>
#include "dfs.grpc.pb.h"
#include "common/logger.h"

namespace minidfs {

/**
 * @brief DataNode - Worker node that stores actual data blocks
 * 
 * Responsibilities:
 * - Store and retrieve data blocks
 * - Send periodic heartbeats to NameNode
 * - Handle block read/write requests from clients
 */
class DataNode final : public DataNodeService::Service {
public:
    DataNode(const std::string& address, const std::string& namenode_address);
    ~DataNode() override;

    // Disable copy
    DataNode(const DataNode&) = delete;
    DataNode& operator=(const DataNode&) = delete;

    /**
     * @brief Start the DataNode gRPC server and heartbeat thread
     */
    void start();

    /**
     * @brief Stop the DataNode gRPC server and heartbeat thread
     */
    void stop();

private:
    // gRPC service implementations
    grpc::Status WriteBlock(
        grpc::ServerContext* context,
        grpc::ServerReader<WriteBlockRequest>* reader,
        WriteBlockResponse* response) override;

    grpc::Status ReadBlock(
        grpc::ServerContext* context,
        const ReadBlockRequest* request,
        grpc::ServerWriter<ReadBlockResponse>* writer) override;

    // Heartbeat methods
    void send_heartbeat();
    void heartbeat_loop();

    // Member variables
    std::string address_;
    std::string namenode_address_;
    std::unique_ptr<grpc::Server> server_;
    std::thread heartbeat_thread_;
    std::atomic<bool> running_;
    std::shared_ptr<grpc::Channel> channel_;
    std::unique_ptr<NameNodeService::Stub> stub_;
};

} // namespace minidfs

#endif // MINIDFS_DATANODE_H
