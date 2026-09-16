#include <gtest/gtest.h>
#include "raft/node.hpp"

TEST(NodeTest, InitialStateIsFollower) {
    raft::RaftNode node(1);
    EXPECT_EQ(node.getState(), raft::NodeState::Follower);
    EXPECT_EQ(node.getCurrentTerm(), 0);
    EXPECT_EQ(node.getLogSize(), 0);
}

TEST(NodeTest, AppendEntriesRejectsUnmatchedPrevLogIndex) {
    raft::RaftNode node(1);

    raft::AppendEntriesArgs args{
        .term = 1,
        .leaderId = 2,
        .prevLogIndex = 5, // Non-existent index
        .prevLogTerm = 1,
        .entries = {},
        .leaderCommit = 0
    };

    auto reply = node.handleAppendEntries(args);
    EXPECT_FALSE(reply.success);
}

TEST(NodeTest, AppendEntriesReplicatesAndCommits) {
    raft::RaftNode node(1);

    raft::LogEntry entry1{.term = 1, .index = 1, .command = {raft::CommandType::Put, "key1", "val1"}};
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
    EXPECT_EQ(node.getLogSize(), 1);
    EXPECT_EQ(node.getCommitIndex(), 1);
}
