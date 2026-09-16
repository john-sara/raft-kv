#pragma once
#include "types.hpp"
#include "rpc.hpp"
#include <chrono>
#include <random>
#include <mutex>
#include <atomic>

namespace raft {

class RaftNode {
public:
    explicit RaftNode(NodeId id);

    NodeId getId() const;
    NodeState getState() const;
    Term getCurrentTerm() const;
    int32_t getVotedFor() const;

    void handleElectionTimeout();
    void receiveHeartbeat(Term leaderTerm);
    std::chrono::milliseconds getRandomTimeout() const;

    RequestVoteReply handleRequestVote(const RequestVoteArgs& args);
    AppendEntriesReply handleAppendEntries(const AppendEntriesArgs& args);

private:
    NodeId id_;
    Term currentTerm_{0};
    int32_t votedFor_{-1};
    NodeState state_{NodeState::Follower};

    mutable std::mutex nodeMutex_;
    mutable std::mt19937 rng_;
    mutable std::uniform_int_distribution<int> timeoutDist_{150, 300};
};

} // namespace raft
