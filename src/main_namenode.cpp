#include <iostream>
#include "namenode.h"
#include "common/config.h"
#include "common/logger.h"

int main(int argc, char* argv[]) {
    try {
        std::string address = minidfs::Config::DEFAULT_NAMENODE_ADDRESS + ":" + 
                             std::to_string(minidfs::Config::DEFAULT_NAMENODE_PORT);
        
        if (argc > 1) {
            address = argv[1];
        }
        
        auto logger = minidfs::Logger::get_logger();
        logger->info("Starting NameNode at {}", address);
        
        minidfs::NameNode namenode(address);
        namenode.start();
        
        // Wait for the server to stop
        std::cout << "Press Enter to stop the NameNode..." << std::endl;
        std::cin.get();
        
        namenode.stop();
        logger->info("NameNode stopped");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
