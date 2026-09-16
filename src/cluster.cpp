#include "raft/cluster.hpp"

namespace raft {

Cluster::Cluster(size_t numNodes) {
    transport_ = std::make_shared<NetworkTransport>();
    for (size_t i = 1; i <= numNodes; ++i) {
        NodeId id = static_cast<NodeId>(i);
        auto node = std::make_shared<RaftNode>(id);
        nodes_[id] = node;
        transport_->registerNode(id, node);
    }
}

std::shared_ptr<RaftNode> Cluster::getNode(NodeId id) {
    auto it = nodes_.find(id);
    if (it != nodes_.end()) {
        return it->second;
    }
    return nullptr;
}

size_t Cluster::getClusterSize() const {
    return nodes_.size();
}

void Cluster::startElection(NodeId candidateId) {
    auto candidate = getNode(candidateId);
    if (!candidate) return;

    candidate->handleElectionTimeout();
    Term currentTerm = candidate->getCurrentTerm();

    size_t votesGranted = 1;

    RequestVoteArgs args{
        .term = currentTerm,
        .candidateId = candidateId,
        .lastLogIndex = 0,
        .lastLogTerm = 0
    };

    for (const auto& [id, node] : nodes_) {
        if (id == candidateId) continue;
        auto reply = transport_->sendRequestVote(id, args);
        if (reply.voteGranted) {
            votesGranted++;
        }
    }

    if (votesGranted > nodes_.size() / 2) {
        candidate->promoteToLeader();

        AppendEntriesArgs heartbeat{
            .term = currentTerm,
            .leaderId = candidateId,
            .prevLogIndex = 0,
            .prevLogTerm = 0,
            .entries = {},
            .leaderCommit = 0
        };

        for (const auto& [id, node] : nodes_) {
            if (id == candidateId) continue;
            transport_->sendAppendEntries(id, heartbeat);
        }
    }
}

} // namespace raft
