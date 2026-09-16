#include "raft/node.hpp"
#include <algorithm>

namespace raft {

RaftNode::RaftNode(NodeId id) 
    : id_(id), rng_(id + std::chrono::steady_clock::now().time_since_epoch().count()) {
    // Pad log index 0 with a dummy entry so true log entries start at index 1
    log_.push_back(LogEntry{.term = 0, .index = 0, .command = {CommandType::Get, "", ""}});
}

NodeId RaftNode::getId() const { return id_; }

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

LogIndex RaftNode::getCommitIndex() const {
    std::lock_guard<std::mutex> lock(nodeMutex_);
    return commitIndex_;
}

size_t RaftNode::getLogSize() const {
    std::lock_guard<std::mutex> lock(nodeMutex_);
    return log_.size() - 1; // Exclude dummy entry at index 0
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

    // Step down if receiving from valid leader of equal/higher term
    if (args.term >= currentTerm_) {
        currentTerm_ = args.term;
        state_ = NodeState::Follower;
        votedFor_ = -1;
    }

    // Rule 2: Reply false if log doesn't contain an entry at prevLogIndex matching prevLogTerm
    if (args.prevLogIndex >= log_.size() || log_[args.prevLogIndex].term != args.prevLogTerm) {
        reply.term = currentTerm_;
        return reply;
    }

    // Rule 3 & 4: Append new entries, overwriting conflicting entries
    LogIndex insertIndex = args.prevLogIndex + 1;
    for (const auto& entry : args.entries) {
        if (insertIndex < log_.size()) {
            if (log_[insertIndex].term != entry.term) {
                log_.erase(log_.begin() + insertIndex, log_.end());
                log_.push_back(entry);
            }
        } else {
            log_.push_back(entry);
        }
        insertIndex++;
    }

    // Rule 5: Update commitIndex if leaderCommit > commitIndex
    if (args.leaderCommit > commitIndex_) {
        commitIndex_ = std::min(args.leaderCommit, static_cast<LogIndex>(log_.size() - 1));
    }

    reply.success = true;
    reply.term = currentTerm_;
    return reply;
}

} // namespace raft
