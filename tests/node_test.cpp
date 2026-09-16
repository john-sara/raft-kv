#include <gtest/gtest.h>
#include "raft/node.hpp"

TEST(NodeTest, InitialStateIsFollower) {
    raft::RaftNode node(1);
    EXPECT_EQ(node.getState(), raft::NodeState::Follower);
    EXPECT_EQ(node.getCurrentTerm(), 0);
    EXPECT_EQ(node.getLogSize(), 0);
}

TEST(NodeTest, AppendEntriesReplicatesAndCommits) {
    raft::RaftNode node(1);

    raft::LogEntry entry1{.term = 1, .index = 1, .command = {raft::CommandType::Put, "session_token", "abc123xyz"}};
    raft::AppendEntriesArgs args{
        .term = 1,
        .leaderId = 2,
        .prevLogIndex = 0,
        .prevLogTerm = 0,
        .entries = {entry1},
        .leaderCommit = 1
    };

    auto reply = node.handleAppendEntries(args);
    EXPECT_TRUE(reply.success);
    EXPECT_EQ(node.getCommitIndex(), 1);
    EXPECT_EQ(node.getLastApplied(), 0);

    size_t applied = node.applyCommittedEntries();
    EXPECT_EQ(applied, 1);
    EXPECT_EQ(node.getLastApplied(), 1);

    auto val = node.getValue("session_token");
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), "abc123xyz");
}
