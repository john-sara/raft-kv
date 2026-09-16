#include "raft/state_machine.hpp"

namespace raft {

std::optional<std::string> KeyValueStateMachine::apply(const Command& cmd) {
    std::lock_guard<std::mutex> lock(storeMutex_);
    switch (cmd.type) {
        case CommandType::Put:
            store_[cmd.key] = cmd.value;
            return cmd.value;
        case CommandType::Get: {
            auto it = store_.find(cmd.key);
            if (it != store_.end()) {
                return it->second;
            }
            return std::nullopt;
        }
        case CommandType::Delete: {
            auto it = store_.find(cmd.key);
            if (it != store_.end()) {
                std::string oldVal = it->second;
                store_.erase(it);
                return oldVal;
            }
            return std::nullopt;
        }
    }
    return std::nullopt;
}

std::optional<std::string> KeyValueStateMachine::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(storeMutex_);
    auto it = store_.find(key);
    if (it != store_.end()) {
        return it->second;
    }
    return std::nullopt;
}

size_t KeyValueStateMachine::size() const {
    std::lock_guard<std::mutex> lock(storeMutex_);
    return store_.size();
}

} // namespace raft
