#include <gtest/gtest.h>
#include "client.h"
#include "common/config.h"

class ClientTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test client
        namenode_address_ = minidfs::Config::DEFAULT_NAMENODE_ADDRESS + ":" + 
                           std::to_string(minidfs::Config::DEFAULT_NAMENODE_PORT);
        client_ = std::make_unique<minidfs::Client>(namenode_address_);
    }

    void TearDown() override {
        // Clean up
        client_.reset();
    }

    std::string namenode_address_;
    std::unique_ptr<minidfs::Client> client_;
};

TEST_F(ClientTest, ConstructorTest) {
    EXPECT_NE(client_, nullptr);
}

TEST_F(ClientTest, BlockSizeTest) {
    // Test that the block size is set correctly
    EXPECT_EQ(minidfs::Client::BLOCK_SIZE_BYTES, 4 * 1024 * 1024);
}
