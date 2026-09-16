#include "raft/transport.hpp"
#include "raft/node.hpp"

namespace raft {

void NetworkTransport::registerNode(NodeId id, std::shared_ptr<RaftNode> node) {
    std::lock_guard<std::mutex> lock(transportMutex_);
    nodes_[id] = node;
}

RequestVoteReply NetworkTransport::sendRequestVote(NodeId targetNode, const RequestVoteArgs& args) {
    std::shared_ptr<RaftNode> target;
    {
        std::lock_guard<std::mutex> lock(transportMutex_);
        auto it = nodes_.find(targetNode);
        if (it != nodes_.end()) {
            target = it->second;
        }
    }

    if (target) {
        return target->handleRequestVote(args);
    }
    return RequestVoteReply{.term = args.term, .voteGranted = false};
}

AppendEntriesReply NetworkTransport::sendAppendEntries(NodeId targetNode, const AppendEntriesArgs& args) {
    std::shared_ptr<RaftNode> target;
    {
        std::lock_guard<std::mutex> lock(transportMutex_);
        auto it = nodes_.find(targetNode);
        if (it != nodes_.end()) {
            target = it->second;
        }
    }

    if (target) {
        return target->handleAppendEntries(args);
    }
    return AppendEntriesReply{.term = args.term, .success = false};
}

} // namespace raft
