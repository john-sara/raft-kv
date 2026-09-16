#pragma once
#include "types.hpp"

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

} // namespace raft
