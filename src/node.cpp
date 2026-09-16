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

int32_t RaftNode::getVotedFor() const {
    std::lock_guard<std::mutex> lock(nodeMutex_);
    return votedFor_;
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

RequestVoteReply RaftNode::handleRequestVote(const RequestVoteArgs& args) {
    std::lock_guard<std::mutex> lock(nodeMutex_);
    RequestVoteReply reply{.term = currentTerm_, .voteGranted = false};

    if (args.term < currentTerm_) {
        return reply;
    }

    if (args.term > currentTerm_) {
        currentTerm_ = args.term;
        state_ = NodeState::Follower;
        votedFor_ = -1;
    }

    if (votedFor_ == -1 || votedFor_ == static_cast<int32_t>(args.candidateId)) {
        votedFor_ = static_cast<int32_t>(args.candidateId);
        reply.voteGranted = true;
    }

    reply.term = currentTerm_;
    return reply;
}

AppendEntriesReply RaftNode::handleAppendEntries(const AppendEntriesArgs& args) {
    std::lock_guard<std::mutex> lock(nodeMutex_);
    AppendEntriesReply reply{.term = currentTerm_, .success = false};

    // Rule 1: Reply false if term < currentTerm
    if (args.term < currentTerm_) {
        return reply;
    }

    // Rule 2: If term is greater or equal, accept authority and step down to Follower
    if (args.term >= currentTerm_) {
        currentTerm_ = args.term;
        state_ = NodeState::Follower;
        votedFor_ = -1;
    }

    reply.success = true;
    reply.term = currentTerm_;
    return reply;
}

} // namespace raft
