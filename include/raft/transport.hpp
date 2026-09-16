#pragma once
#include "types.hpp"
#include "rpc.hpp"
#include <unordered_map>
#include <memory>
#include <mutex>

namespace raft {

class RaftNode;

class NetworkTransport {
public:
    NetworkTransport() = default;

    void registerNode(NodeId id, std::shared_ptr<RaftNode> node);
    RequestVoteReply sendRequestVote(NodeId targetNode, const RequestVoteArgs& args);
    AppendEntriesReply sendAppendEntries(NodeId targetNode, const AppendEntriesArgs& args);

private:
    std::unordered_map<NodeId, std::shared_ptr<RaftNode>> nodes_;
    std::mutex transportMutex_;
};

} // namespace raft
