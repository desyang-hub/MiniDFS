#include <gtest/gtest.h>
#include "datanode.h"
#include "common/config.h"

class DataNodeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test DataNode instances
        address_ = std::string(minidfs::Config::DEFAULT_DATANODE_ADDRESS) + ":" + 
                  std::to_string(minidfs::Config::DEFAULT_DATANODE_PORT);
        namenode_address_ = std::string(minidfs::Config::DEFAULT_NAMENODE_ADDRESS) + ":" + 
                           std::to_string(minidfs::Config::DEFAULT_NAMENODE_PORT);
        datanode_ = std::make_unique<minidfs::DataNode>(address_, namenode_address_);
    }

    void TearDown() override {
        // Clean up
        datanode_.reset();
    }

    std::string address_;
    std::string namenode_address_;
    std::unique_ptr<minidfs::DataNode> datanode_;
};

TEST_F(DataNodeTest, ConstructorTest) {
    EXPECT_NE(datanode_, nullptr);
}

TEST_F(DataNodeTest, StartStopTest) {
    // Test that we can start and stop the DataNode
    datanode_->start();
    datanode_->stop();
    
    // Should not crash
    EXPECT_TRUE(true);
}
