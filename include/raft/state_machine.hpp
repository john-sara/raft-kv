#pragma once
#include "log_entry.hpp"
#include <unordered_map>
#include <string>
#include <optional>
#include <mutex>

namespace raft {

class KeyValueStateMachine {
public:
    KeyValueStateMachine() = default;

    // Applies a committed command to the state machine
    std::optional<std::string> apply(const Command& cmd);

    // Read-only state query
    std::optional<std::string> get(const std::string& key) const;

    // Inspection helper
    size_t size() const;

private:
    std::unordered_map<std::string, std::string> store_;
    mutable std::mutex storeMutex_;
};

} // namespace raft
