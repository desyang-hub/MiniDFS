#include "client.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace minidfs {
    const size_t Client::BLOCK_SIZE_BYTES = 4 * 1024 * 1024; // 4MB

    Client::Client(const std::string& namenode_address) {
        channel_ = grpc::CreateChannel(namenode_address, grpc::InsecureChannelCredentials());
        stub_ = NameNodeService::NewStub(channel_);
    }

    Client::~Client() {}

    bool Client::put(const std::string& local_file, const std::string& remote_path) {
        auto logger = Logger::get_logger();
        
        try {
            // Check if local file exists
            if (!std::filesystem::exists(local_file)) {
                logger->error("Local file does not exist: {}", local_file);
                return false;
            }
            
            // Open and read the file in chunks
            std::ifstream file(local_file, std::ios::binary);
            if (!file.is_open()) {
                logger->error("Failed to open local file: {}", local_file);
                return false;
            }
            
            // Get file size
            file.seekg(0, std::ios::end);
            size_t file_size = file.tellg();
            file.seekg(0, std::ios::beg);
            
            logger->info("Uploading file {} (size: {} bytes)", local_file, file_size);
            
            // Split into blocks
            size_t block_id = 0;
            std::vector<std::string> block_ids;
            
            while (file.tellg() < file_size) {
                // Create a new block ID
                std::string block_id_str = std::to_string(block_id);
                
                // Read block data
                std::vector<char> buffer(BLOCK_SIZE_BYTES);
                size_t bytes_read = 0;
                
                while (bytes_read < BLOCK_SIZE_BYTES && file.tellg() < file_size) {
                    size_t to_read = std::min(BLOCK_SIZE_BYTES - bytes_read, 
                                            static_cast<size_t>(file_size - file.tellg()));
                    file.read(buffer.data() + bytes_read, to_read);
                    bytes_read += to_read;
                }
                
                // Create gRPC channel to DataNode
                // In a real implementation, we would get the actual DataNode address from NameNode
                // For now, we'll use a placeholder approach
                
                logger->info("Sending block {} (size: {} bytes)", block_id_str, bytes_read);
                
                // Add block metadata to NameNode
                AddBlockRequest add_request;
                add_request.set_file_path(remote_path);
                add_request.set_block_id(block_id_str);
                add_request.set_data_node_address("localhost:50052");  // Placeholder
                
                AddBlockResponse add_response;
                grpc::ClientContext context;
                grpc::Status status = stub_->AddBlock(&context, add_request, &add_response);
                
                if (!status.ok() || !add_response.success()) {
                    logger->error("Failed to add block metadata: {} - {}", 
                                 status.error_code(), status.error_message());
                    return false;
                }
                
                block_ids.push_back(block_id_str);
                block_id++;
            }
            
            file.close();
            logger->info("File uploaded successfully");
            return true;
        } catch (const std::exception& e) {
            logger->error("Error uploading file: {}", e.what());
            return false;
        }
    }

    bool Client::get(const std::string& remote_path, const std::string& local_file) {
        auto logger = Logger::get_logger();
        
        try {
            // Get block locations from NameNode
            GetBlockLocationsRequest request;
            request.set_file_path(remote_path);
            
            GetBlockLocationsResponse response;
            grpc::ClientContext context;
            grpc::Status status = stub_->GetBlockLocations(&context, request, &response);
            
            if (!status.ok()) {
                logger->error("Failed to get block locations: {} - {}", 
                             status.error_code(), status.error_message());
                return false;
            }
            
            // Create local file
            std::ofstream file(local_file, std::ios::binary | std::ios::trunc);
            if (!file.is_open()) {
                logger->error("Failed to create local file: {}", local_file);
                return false;
            }
            
            logger->info("Downloading file {} to {}", remote_path, local_file);
            
            // Read each block and write to local file
            for (int i = 0; i < response.locations_size(); ++i) {
                const BlockLocation& location = response.locations(i);
                std::string block_id = location.block_id();
                
                logger->info("Reading block: {}", block_id);
                
                // Create gRPC channel to DataNode
                grpc::ChannelArguments args;
                args.SetMaxReceiveMessageSize(-1);  // Unlimited message size
                
                auto channel = grpc::CreateChannel(location.data_node_address(), 
                                                  grpc::InsecureChannelCredentials());
                auto datanode_stub = DataNodeService::NewStub(channel);
                
                ReadBlockRequest read_request;
                read_request.set_block_id(block_id);
                
                grpc::ClientContext read_context;
                std::unique_ptr<grpc::ClientReader<ReadBlockResponse> > reader(
                    datanode_stub->ReadBlock(&read_context, read_request));
                
                ReadBlockResponse read_response;
                while (reader->Read(&read_response)) {
                    file.write(read_response.data().data(), read_response.data().size());
                }
                
                grpc::Status read_status = reader->Finish();
                if (!read_status.ok()) {
                    logger->error("Failed to read block {}: {} - {}", 
                                 block_id, read_status.error_code(), read_status.error_message());
                    file.close();
                    return false;
                }
            }
            
            file.close();
            logger->info("File downloaded successfully");
            return true;
        } catch (const std::exception& e) {
            logger->error("Error downloading file: {}", e.what());
            return false;
        }
    }
}
