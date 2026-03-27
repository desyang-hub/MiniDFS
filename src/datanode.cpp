#include "datanode.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>

namespace minidfs {
    DataNode::DataNode(const std::string& address, const std::string& namenode_address) 
        : address_(address), 
          namenode_address_(namenode_address),
          running_(false) {
        // Create data directory if it doesn't exist
        std::filesystem::create_directories("data");
        
        // Create gRPC channel to NameNode
        channel_ = grpc::CreateChannel(namenode_address_, grpc::InsecureChannelCredentials());
        stub_ = NameNodeService::NewStub(channel_);
    }

    DataNode::~DataNode() {
        stop();
    }

    void DataNode::start() {
        running_ = true;
        
        grpc::ServerBuilder builder;
        builder.AddListeningPort(address_, grpc::InsecureServerCredentials());
        builder.RegisterService(this);
        
        server_ = builder.BuildAndStart();
        auto logger = Logger::get_logger();
        logger->info("DataNode listening on {}", address_);
        
        // Start heartbeat thread
        heartbeat_thread_ = std::thread(&DataNode::heartbeat_loop, this);
    }

    void DataNode::stop() {
        running_ = false;
        
        if (server_) {
            server_->Shutdown();
        }
        
        if (heartbeat_thread_.joinable()) {
            heartbeat_thread_.join();
        }
    }

    grpc::Status DataNode::WriteBlock(
        grpc::ServerContext* context,
        grpc::ServerReader<WriteBlockRequest>* reader,
        WriteBlockResponse* response) {
        
        auto logger = Logger::get_logger();
        try {
            WriteBlockRequest request;
            
            // Get block ID from the first message
            if (!reader->Read(&request)) {
                logger->error("Failed to read block ID");
                response->set_success(false);
                response->set_message("Failed to read block ID");
                return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid request");
            }
            
            std::string block_id = request.block_id();
            std::string filename = "data/blk_" + block_id;
            
            // Write data to file
            std::ofstream file(filename, std::ios::binary | std::ios::trunc);
            if (!file.is_open()) {
                logger->error("Failed to open file for writing: {}", filename);
                response->set_success(false);
                response->set_message("Failed to open file");
                return grpc::Status(grpc::StatusCode::INTERNAL, "Failed to open file");
            }
            
            // Read and write all data
            while (reader->Read(&request)) {
                file.write(request.data().data(), request.data().size());
            }
            
            file.close();
            
            logger->info("Block written successfully: {}", block_id);
            response->set_success(true);
            response->set_message("Block written successfully");
        } catch (const std::exception& e) {
            logger->error("Error writing block: {}", e.what());
            response->set_success(false);
            response->set_message("Failed to write block");
            return grpc::Status(grpc::StatusCode::INTERNAL, "Internal error");
        }
        
        return grpc::Status::OK;
    }

    grpc::Status DataNode::ReadBlock(
        grpc::ServerContext* context,
        const ReadBlockRequest* request,
        grpc::ServerWriter<ReadBlockResponse>* writer) {
        
        auto logger = Logger::get_logger();
        try {
            std::string block_id = request->block_id();
            std::string filename = "data/blk_" + block_id;
            
            // Check if file exists
            if (!std::filesystem::exists(filename)) {
                logger->error("Block not found: {}", filename);
                ReadBlockResponse response;
                response.set_data("");  // Empty data
                writer->Write(response);
                return grpc::Status(grpc::StatusCode::NOT_FOUND, "Block not found");
            }
            
            // Read file and send in chunks
            std::ifstream file(filename, std::ios::binary);
            if (!file.is_open()) {
                logger->error("Failed to open file for reading: {}", filename);
                return grpc::Status(grpc::StatusCode::INTERNAL, "Failed to open file");
            }
            
            const size_t chunk_size = 64 * 1024; // 64KB chunks
            std::vector<char> buffer(chunk_size);
            
            while (file.read(buffer.data(), chunk_size) || file.gcount() > 0) {
                ReadBlockResponse response;
                response.set_data(buffer.data(), file.gcount());
                writer->Write(response);
            }
            
            file.close();
            
            logger->info("Block read successfully: {}", block_id);
        } catch (const std::exception& e) {
            logger->error("Error reading block: {}", e.what());
            return grpc::Status(grpc::StatusCode::INTERNAL, "Internal error");
        }
        
        return grpc::Status::OK;
    }

    void DataNode::send_heartbeat() {
        auto logger = Logger::get_logger();
        try {
            HeartbeatRequest request;
            DataNodeInfo* info = request.mutable_datanode_info();
            info->set_id(address_);
            info->set_address(address_);
            info->set_last_heartbeat(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
            
            HeartbeatResponse response;
            grpc::ClientContext context;
            grpc::Status status = stub_->Heartbeat(&context, request, &response);
            
            if (!status.ok()) {
                logger->error("Heartbeat failed: {} - {}", status.error_code(), status.error_message());
            } else {
                logger->info("Heartbeat sent successfully");
            }
        } catch (const std::exception& e) {
            logger->error("Error sending heartbeat: {}", e.what());
        }
    }

    void DataNode::heartbeat_loop() {
        auto logger = Logger::get_logger();
        const int interval_seconds = 5; // Send heartbeat every 5 seconds
        
        while (running_) {
            send_heartbeat();
            std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
        }
        
        logger->info("Heartbeat thread stopped");
    }
}
