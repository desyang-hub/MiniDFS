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
    class DataNode final : public DataNodeService::Service {
    public:
        DataNode(const std::string& address, const std::string& namenode_address);
        ~DataNode();

        void start();
        void stop();

    private:
        grpc::Status WriteBlock(
            grpc::ServerContext* context,
            grpc::ServerReader<WriteBlockRequest>* reader,
            WriteBlockResponse* response) override;

        grpc::Status ReadBlock(
            grpc::ServerContext* context,
            const ReadBlockRequest* request,
            grpc::ServerWriter<ReadBlockResponse>* writer) override;

        void send_heartbeat();
        void heartbeat_loop();

        std::string address_;
        std::string namenode_address_;
        std::unique_ptr<grpc::Server> server_;
        std::thread heartbeat_thread_;
        std::atomic<bool> running_;
        std::shared_ptr<grpc::Channel> channel_;
        std::unique_ptr<NameNodeService::Stub> stub_;
    };
}

#endif // MINIDFS_DATANODE_H
