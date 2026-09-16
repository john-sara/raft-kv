#include <gtest/gtest.h>
#include "raft/state_machine.hpp"

TEST(StateMachineTest, PutAndGetOperations) {
    raft::KeyValueStateMachine kv;

    raft::Command putCmd{
        .type = raft::CommandType::Put,
        .key = "user:1",
        .value = "Alice"
    };

    auto res = kv.apply(putCmd);
    EXPECT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), "Alice");

    auto val = kv.get("user:1");
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), "Alice");
}

TEST(StateMachineTest, DeleteOperation) {
    raft::KeyValueStateMachine kv;

    kv.apply({.type = raft::CommandType::Put, .key = "temp", .value = "data"});
    EXPECT_EQ(kv.size(), 1);

    auto deletedVal = kv.apply({.type = raft::CommandType::Delete, .key = "temp", .value = ""});
    EXPECT_TRUE(deletedVal.has_value());
    EXPECT_EQ(deletedVal.value(), "data");
    EXPECT_EQ(kv.size(), 0);
    EXPECT_FALSE(kv.get("temp").has_value());
}

TEST(StateMachineTest, GetNonExistentKeyReturnsNullopt) {
    raft::KeyValueStateMachine kv;
    EXPECT_FALSE(kv.get("missing").has_value());
}
