#include <gtest/gtest.h>
#include "namenode.h"
#include "common/config.h"

class NameNodeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a test NameNode instance
        namenode_address_ = minidfs::Config::DEFAULT_NAMENODE_ADDRESS + ":" + 
                           std::to_string(minidfs::Config::DEFAULT_NAMENODE_PORT);
        namenode_ = std::make_unique<minidfs::NameNode>(namenode_address_);
    }

    void TearDown() override {
        // Clean up
        namenode_.reset();
    }

    std::string namenode_address_;
    std::unique_ptr<minidfs::NameNode> namenode_;
};

TEST_F(NameNodeTest, ConstructorTest) {
    EXPECT_NE(namenode_, nullptr);
}

TEST_F(NameNodeTest, StartStopTest) {
    // Test that we can start and stop the NameNode
    namenode_->start();
    namenode_->stop();
    
    // Should not crash
    EXPECT_TRUE(true);
}
