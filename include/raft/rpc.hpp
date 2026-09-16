#pragma once
#include "types.hpp"
#include "log_entry.hpp"
#include <vector>

namespace raft {

struct RequestVoteArgs {
    Term term;
    NodeId candidateId;
    LogIndex lastLogIndex;
    Term lastLogTerm;
};

struct RequestVoteReply {
    Term term;
    bool voteGranted;
};

struct AppendEntriesArgs {
    Term term;
    NodeId leaderId;
    LogIndex prevLogIndex;
    Term prevLogTerm;
    std::vector<LogEntry> entries;
    LogIndex leaderCommit;
};

struct AppendEntriesReply {
    Term term;
    bool success;
};

} // namespace raft
