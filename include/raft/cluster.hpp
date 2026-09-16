#pragma once
#include "types.hpp"
#include "node.hpp"
#include "transport.hpp"
#include <vector>
#include <memory>

namespace raft {

class Cluster {
public:
    explicit Cluster(size_t numNodes);

    std::shared_ptr<RaftNode> getNode(NodeId id);
    void startElection(NodeId candidateId);
    size_t getClusterSize() const;

private:
    std::shared_ptr<NetworkTransport> transport_;
    std::unordered_map<NodeId, std::shared_ptr<RaftNode>> nodes_;
};

} // namespace raft
