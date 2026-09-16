#include <gtest/gtest.h>
#include "raft/cluster.hpp"

TEST(ClusterTest, ClusterInitialization) {
    raft::Cluster cluster(3);
    EXPECT_EQ(cluster.getClusterSize(), 3);
    EXPECT_NE(cluster.getNode(1), nullptr);
    EXPECT_NE(cluster.getNode(2), nullptr);
    EXPECT_NE(cluster.getNode(3), nullptr);
}

TEST(ClusterTest, ElectionReachesMajorityQuorum) {
    raft::Cluster cluster(3);
    
    // Node 1 triggers election
    cluster.startElection(1);

    auto node1 = cluster.getNode(1);
    auto node2 = cluster.getNode(2);
    auto node3 = cluster.getNode(3);

    // Node 2 & Node 3 should step down to Followers under Term 1
    EXPECT_EQ(node2->getCurrentTerm(), 1);
    EXPECT_EQ(node3->getCurrentTerm(), 1);
    EXPECT_EQ(node2->getState(), raft::NodeState::Follower);
    EXPECT_EQ(node3->getState(), raft::NodeState::Follower);
}
