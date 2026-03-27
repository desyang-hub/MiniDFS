#include <iostream>
#include <CLI/CLI.hpp>
#include "client.h"
#include "common/config.h"
#include "common/logger.h"

int main(int argc, char* argv[]) {
    try {
        std::string namenode_address = minidfs::Config::DEFAULT_NAMENODE_ADDRESS + ":" + 
                                      std::to_string(minidfs::Config::DEFAULT_NAMENODE_PORT);
        
        CLI::App app("MiniDFS Client");
        app.add_option("-n,--namenode", namenode_address, "NameNode address")->capture_default_str();
        
        std::string command;
        app.add_option("command", command, "Command: put or get")->required();
        
        std::string arg1, arg2;
        app.add_option("arg1", arg1, "First argument")->required();
        app.add_option("arg2", arg2, "Second argument")->required();
        
        CLI11_PARSE(app, argc, argv);
        
        auto logger = minidfs::Logger::get_logger();
        logger->info("Starting client connected to NameNode at {}", namenode_address);
        
        minidfs::Client client(namenode_address);
        
        if (command == "put") {
            if (client.put(arg1, arg2)) {
                logger->info("File uploaded successfully");
            } else {
                logger->error("Failed to upload file");
                return 1;
            }
        } else if (command == "get") {
            if (client.get(arg1, arg2)) {
                logger->info("File downloaded successfully");
            } else {
                logger->error("Failed to download file");
                return 1;
            }
        } else {
            logger->error("Unknown command: {}", command);
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
