#include <gtest/gtest.h>
#include "raft/node.hpp"
#include "raft/transport.hpp"

TEST(TransportTest, NodeRoutingAndRPCCommunication) {
    auto transport = std::make_shared<raft::NetworkTransport>();
    auto node1 = std::make_shared<raft::RaftNode>(1);
    auto node2 = std::make_shared<raft::RaftNode>(2);

    transport->registerNode(1, node1);
    transport->registerNode(2, node2);

    raft::RequestVoteArgs args{
        .term = 1,
        .candidateId = 1,
        .lastLogIndex = 0,
        .lastLogTerm = 0
    };

    auto reply = transport->sendRequestVote(2, args);
    EXPECT_TRUE(reply.voteGranted);
    EXPECT_EQ(node2->getVotedFor(), 1);
}
