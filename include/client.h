#ifndef MINIDFS_CLIENT_H
#define MINIDFS_CLIENT_H

#include <string>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "dfs.grpc.pb.h"
#include "common/logger.h"

namespace minidfs {
    class Client {
    public:
        explicit Client(const std::string& namenode_address);
        ~Client();

        bool put(const std::string& local_file, const std::string& remote_path);
        bool get(const std::string& remote_path, const std::string& local_file);

    private:
        std::shared_ptr<grpc::Channel> channel_;
        std::unique_ptr<NameNodeService::Stub> stub_;
        
    public:
        static const size_t BLOCK_SIZE_BYTES;
    };
}

#endif // MINIDFS_CLIENT_H
