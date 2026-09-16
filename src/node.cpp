#include "raft/node.hpp"

namespace raft {

RaftNode::RaftNode(NodeId id) 
    : id_(id), rng_(id + std::chrono::steady_clock::now().time_since_epoch().count()) {}

NodeId RaftNode::getId() const {
    return id_;
}

NodeState RaftNode::getState() const {
    std::lock_guard<std::mutex> lock(nodeMutex_);
    return state_;
}

Term RaftNode::getCurrentTerm() const {
    std::lock_guard<std::mutex> lock(nodeMutex_);
    return currentTerm_;
}

std::chrono::milliseconds RaftNode::getRandomTimeout() const {
    std::lock_guard<std::mutex> lock(nodeMutex_);
    return std::chrono::milliseconds(timeoutDist_(rng_));
}

void RaftNode::handleElectionTimeout() {
    std::lock_guard<std::mutex> lock(nodeMutex_);
    if (state_ == NodeState::Follower || state_ == NodeState::Candidate) {
        state_ = NodeState::Candidate;
        currentTerm_++;
        votedFor_ = static_cast<int32_t>(id_);
    }
}

void RaftNode::receiveHeartbeat(Term leaderTerm) {
    std::lock_guard<std::mutex> lock(nodeMutex_);
    if (leaderTerm >= currentTerm_) {
        currentTerm_ = leaderTerm;
        state_ = NodeState::Follower;
        votedFor_ = -1;
    }
}

} // namespace raft
