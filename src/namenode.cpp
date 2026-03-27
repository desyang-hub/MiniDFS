#include "namenode.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>

namespace minidfs {
    NameNode::NameNode(const std::string& address) : address_(address) {
        // Create data directory if it doesn't exist
        std::filesystem::create_directories("data");
    }

    NameNode::~NameNode() {
        stop();
    }

    void NameNode::start() {
        grpc::ServerBuilder builder;
        builder.AddListeningPort(address_, grpc::InsecureServerCredentials());
        builder.RegisterService(this);
        
        server_ = builder.BuildAndStart();
        auto logger = Logger::get_logger();
        logger->info("NameNode listening on {}", address_);
    }

    void NameNode::stop() {
        if (server_) {
            server_->Shutdown();
        }
    }

    grpc::Status NameNode::RegisterDataNode(
        grpc::ServerContext* context,
        const RegisterRequest* request,
        RegisterResponse* response) {
        
        auto logger = Logger::get_logger();
        try {
            std::lock_guard<std::shared_mutex> lock(metadata_mutex_);
            
            // Check if DataNode already exists
            bool exists = false;
            for (const auto& node : data_nodes_) {
                if (node.id() == request->datanode_info().id()) {
                    exists = true;
                    break;
                }
            }
            
            if (!exists) {
                // Add new DataNode
                data_nodes_.push_back(request->datanode_info());
                logger->info("Registered new DataNode: {}", request->datanode_info().id());
            } else {
                logger->info("DataNode already registered: {}", request->datanode_info().id());
            }
            
            response->set_success(true);
            response->set_message("Successfully registered DataNode");
            return grpc::Status::OK;
        } catch (const std::exception& e) {
            logger->error("Error registering DataNode: {}", e.what());
            response->set_success(false);
            response->set_message("Failed to register DataNode");
            return grpc::Status(grpc::StatusCode::INTERNAL, "Internal error");
        }
    }

    grpc::Status NameNode::Heartbeat(
        grpc::ServerContext* context,
        const HeartbeatRequest* request,
        HeartbeatResponse* response) {
        
        auto logger = Logger::get_logger();
        try {
            std::lock_guard<std::shared_mutex> lock(metadata_mutex_);
            
            // Update last heartbeat time for the DataNode
            bool found = false;
            for (auto& node : data_nodes_) {
                if (node.id() == request->datanode_info().id()) {
                    node.set_last_heartbeat(std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count());
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                logger->warn("Heartbeat from unknown DataNode: {}", request->datanode_info().id());
                response->set_success(false);
                response->set_message("Unknown DataNode");
                return grpc::Status(grpc::StatusCode::NOT_FOUND, "Unknown DataNode");
            }
            
            logger->debug("Received heartbeat from DataNode: {}", request->datanode_info().id());
            response->set_success(true);
            response->set_message("Heartbeat received");
            return grpc::Status::OK;
        } catch (const std::exception& e) {
            logger->error("Error processing heartbeat: {}", e.what());
            response->set_success(false);
            response->set_message("Failed to process heartbeat");
            return grpc::Status(grpc::StatusCode::INTERNAL, "Internal error");
        }
    }

    grpc::Status NameNode::GetBlockLocations(
        grpc::ServerContext* context,
        const GetBlockLocationsRequest* request,
        GetBlockLocationsResponse* response) {
        
        auto logger = Logger::get_logger();
        try {
            std::lock_guard<std::shared_mutex> lock(metadata_mutex_);
            
            // Find file metadata
            auto it = file_metadata_.find(request->file_path());
            if (it == file_metadata_.end()) {
                logger->warn("File not found: {}", request->file_path());
                // Return empty response instead of setting success/message
                return grpc::Status(grpc::StatusCode::NOT_FOUND, "File not found");
            }
            
            const FileMetadata& file_meta = it->second;
            
            // Populate response with block locations
            for (const auto& location : file_meta.block_locations()) {
                BlockLocation* block_location = response->add_locations();
                block_location->set_block_id(location);
                // Note: We don't have the actual DataNode address here, 
                // so we'll need to implement a mechanism to track this
                block_location->set_data_node_address("");  // Placeholder
            }
            
            return grpc::Status::OK;
        } catch (const std::exception& e) {
            logger->error("Error getting block locations: {}", e.what());
            return grpc::Status(grpc::StatusCode::INTERNAL, "Internal error");
        }
    }

    grpc::Status NameNode::AddBlock(
        grpc::ServerContext* context,
        const AddBlockRequest* request,
        AddBlockResponse* response) {
        
        auto logger = Logger::get_logger();
        try {
            std::lock_guard<std::shared_mutex> lock(metadata_mutex_);
            
            // Update file metadata
            auto& file_meta = file_metadata_[request->file_path()];
            file_meta.set_name(request->file_path());
            
            // Add block location to the list
            file_meta.add_block_locations(request->block_id());
            
            logger->info("Added block {} for file {}", request->block_id(), request->file_path());
            response->set_success(true);
            response->set_message("Block added successfully");
            return grpc::Status::OK;
        } catch (const std::exception& e) {
            logger->error("Error adding block: {}", e.what());
            response->set_success(false);
            response->set_message("Failed to add block");
            return grpc::Status(grpc::StatusCode::INTERNAL, "Internal error");
        }
    }
}
