#include <gtest/gtest.h>
#include "raft/node.hpp"

TEST(NodeTest, InitialStateIsFollower) {
    raft::RaftNode node(1);
    EXPECT_EQ(node.getState(), raft::NodeState::Follower);
    EXPECT_EQ(node.getCurrentTerm(), 0);
}

TEST(NodeTest, ElectionTimeoutTransitionsToCandidate) {
    raft::RaftNode node(1);
    node.handleElectionTimeout();

    EXPECT_EQ(node.getState(), raft::NodeState::Candidate);
    EXPECT_EQ(node.getCurrentTerm(), 1);
    EXPECT_EQ(node.getVotedFor(), 1);
}

TEST(NodeTest, GrantVoteOnValidTerm) {
    raft::RaftNode node(1);
    raft::RequestVoteArgs args{
        .term = 1,
        .candidateId = 2,
        .lastLogIndex = 0,
        .lastLogTerm = 0
    };

    auto reply = node.handleRequestVote(args);
    EXPECT_TRUE(reply.voteGranted);
    EXPECT_EQ(node.getVotedFor(), 2);
}

TEST(NodeTest, AppendEntriesAcceptsValidLeaderHeartbeat) {
    raft::RaftNode node(1);
    node.handleElectionTimeout(); // Become Candidate in Term 1

    raft::AppendEntriesArgs args{
        .term = 2, // Higher term leader
        .leaderId = 3,
        .prevLogIndex = 0,
        .prevLogTerm = 0,
        .entries = {},
        .leaderCommit = 0
    };

    auto reply = node.handleAppendEntries(args);
    EXPECT_TRUE(reply.success);
    EXPECT_EQ(node.getState(), raft::NodeState::Follower);
    EXPECT_EQ(node.getCurrentTerm(), 2);
}

TEST(NodeTest, AppendEntriesRejectsStaleLeader) {
    raft::RaftNode node(1);
    node.handleElectionTimeout(); // Term 1
    node.handleElectionTimeout(); // Term 2

    raft::AppendEntriesArgs args{
        .term = 1, // Stale leader
        .leaderId = 3,
        .prevLogIndex = 0,
        .prevLogTerm = 0,
        .entries = {},
        .leaderCommit = 0
    };

    auto reply = node.handleAppendEntries(args);
    EXPECT_FALSE(reply.success);
    EXPECT_EQ(node.getState(), raft::NodeState::Candidate);
    EXPECT_EQ(node.getCurrentTerm(), 2);
}
