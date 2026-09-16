#pragma once
#include "types.hpp"
#include "rpc.hpp"
#include "log_entry.hpp"
#include "state_machine.hpp"
#include <chrono>
#include <random>
#include <mutex>
#include <vector>
#include <memory>

namespace raft {

class RaftNode {
public:
    explicit RaftNode(NodeId id);

    NodeId getId() const;
    NodeState getState() const;
    Term getCurrentTerm() const;
    int32_t getVotedFor() const;
    LogIndex getCommitIndex() const;
    LogIndex getLastApplied() const;
    size_t getLogSize() const;

    void handleElectionTimeout();
    void promoteToLeader();
    void receiveHeartbeat(Term leaderTerm);
    std::chrono::milliseconds getRandomTimeout() const;

    // Leader entry proposal
    LogIndex propose(const Command& cmd);

    RequestVoteReply handleRequestVote(const RequestVoteArgs& args);
    AppendEntriesReply handleAppendEntries(const AppendEntriesArgs& args);

    std::optional<std::string> getValue(const std::string& key) const;
    size_t applyCommittedEntries();

private:
    NodeId id_;
    Term currentTerm_{0};
    int32_t votedFor_{-1};
    NodeState state_{NodeState::Follower};

    LogIndex commitIndex_{0};
    LogIndex lastApplied_{0};

    std::vector<LogEntry> log_;
    KeyValueStateMachine stateMachine_;

    mutable std::mutex nodeMutex_;
    mutable std::mt19937 rng_;
    mutable std::uniform_int_distribution<int> timeoutDist_{150, 300};
};

} // namespace raft
