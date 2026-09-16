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
}

TEST(NodeTest, HeartbeatResetsCandidateToFollower) {
    raft::RaftNode node(1);
    node.handleElectionTimeout(); // Term 1 Candidate
    node.receiveHeartbeat(2);     // Receives heartbeat from term 2 leader

    EXPECT_EQ(node.getState(), raft::NodeState::Follower);
    EXPECT_EQ(node.getCurrentTerm(), 2);
}

TEST(NodeTest, RandomizedTimeoutWithinBounds) {
    raft::RaftNode node(1);
    auto timeout = node.getRandomTimeout();
    EXPECT_GE(timeout.count(), 150);
    EXPECT_LE(timeout.count(), 300);
}
