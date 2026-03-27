#include <iostream>
#include "datanode.h"
#include "common/config.h"
#include "common/logger.h"

int main(int argc, char* argv[]) {
    try {
        std::string address = minidfs::Config::DEFAULT_DATANODE_ADDRESS + ":" + 
                             std::to_string(minidfs::Config::DEFAULT_DATANODE_PORT);
        std::string namenode_address = minidfs::Config::DEFAULT_NAMENODE_ADDRESS + ":" + 
                                      std::to_string(minidfs::Config::DEFAULT_NAMENODE_PORT);
        
        if (argc > 1) {
            address = argv[1];
        }
        
        if (argc > 2) {
            namenode_address = argv[2];
        }
        
        auto logger = minidfs::Logger::get_logger();
        logger->info("Starting DataNode at {} connected to NameNode at {}", address, namenode_address);
        
        minidfs::DataNode datanode(address, namenode_address);
        datanode.start();
        
        // Wait for the server to stop
        std::cout << "Press Enter to stop the DataNode..." << std::endl;
        std::cin.get();
        
        datanode.stop();
        logger->info("DataNode stopped");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
