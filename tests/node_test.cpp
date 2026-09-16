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

TEST(NodeTest, HeartbeatResetsCandidateToFollower) {
    raft::RaftNode node(1);
    node.handleElectionTimeout();
    node.receiveHeartbeat(2);

    EXPECT_EQ(node.getState(), raft::NodeState::Follower);
    EXPECT_EQ(node.getCurrentTerm(), 2);
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

TEST(NodeTest, DenyVoteOnOutdatedTerm) {
    raft::RaftNode node(1);
    node.handleElectionTimeout(); // Node is on Term 1

    raft::RequestVoteArgs args{
        .term = 0, // Stale term candidate
        .candidateId = 2,
        .lastLogIndex = 0,
        .lastLogTerm = 0
    };

    auto reply = node.handleRequestVote(args);
    EXPECT_FALSE(reply.voteGranted);
    EXPECT_EQ(node.getVotedFor(), 1); // Remains voted for itself
}

TEST(NodeTest, DenySecondVoteInSameTerm) {
    raft::RaftNode node(1);
    raft::RequestVoteArgs args1{.term = 1, .candidateId = 2, .lastLogIndex = 0, .lastLogTerm = 0};
    raft::RequestVoteArgs args2{.term = 1, .candidateId = 3, .lastLogIndex = 0, .lastLogTerm = 0};

    auto reply1 = node.handleRequestVote(args1);
    auto reply2 = node.handleRequestVote(args2);

    EXPECT_TRUE(reply1.voteGranted);
    EXPECT_FALSE(reply2.voteGranted);
    EXPECT_EQ(node.getVotedFor(), 2);
}
