#pragma once
#include "types.hpp"
#include <string>

namespace raft {

struct Command {
    CommandType type;
    std::string key;
    std::string value;
};

struct LogEntry {
    Term term;
    LogIndex index;
    Command command;
};

} // namespace raft
