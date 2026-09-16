#pragma once
#include <cstdint>
#include <string>

namespace raft {

using NodeId = uint32_t;
using Term = uint64_t;
using LogIndex = uint64_t;

enum class NodeState {
    Follower,
    Candidate,
    Leader
};

enum class CommandType {
    Put,
    Get,
    Delete
};

} // namespace raft
