#include "raft/cluster.hpp"
#include <iostream>
#include <string>

void printHelp() {
    std::cout << "\n=== Distributed Raft Key-Value Store CLI ===\n"
              << "Commands:\n"
              << "  election <node_id>             - Trigger an election round for specified node\n"
              << "  put <leader_id> <key> <val>    - Replicate a key-value pair across cluster\n"
              << "  get <node_id> <key>            - Query key-value state of a specific node\n"
              << "  status                         - Display state and current term of all nodes\n"
              << "  help                           - Show this help menu\n"
              << "  exit                           - Shutdown cluster and exit CLI\n"
              << "============================================\n\n";
}

int main() {
    constexpr size_t NUM_NODES = 3;
    raft::Cluster cluster(NUM_NODES);

    std::cout << "Successfully initialized " << NUM_NODES << "-node Raft Cluster.\n";
    printHelp();

    std::string command;
    while (true) {
        std::cout << "raft-kv> ";
        if (!(std::cin >> command)) break;

        if (command == "exit") {
            std::cout << "Shutting down cluster...\n";
            break;
        } else if (command == "help") {
            printHelp();
        } else if (command == "status") {
            std::cout << "\n--- Cluster Status ---\n";
            for (size_t i = 1; i <= cluster.getClusterSize(); ++i) {
                auto node = cluster.getNode(static_cast<raft::NodeId>(i));
                std::string stateStr = (node->getState() == raft::NodeState::Leader) ? "Leader" :
                                       (node->getState() == raft::NodeState::Candidate) ? "Candidate" : "Follower";
                std::cout << "Node " << node->getId() 
                          << " | State: " << stateStr 
                          << " | Term: " << node->getCurrentTerm() 
                          << " | Log Size: " << node->getLogSize()
                          << " | Commit Index: " << node->getCommitIndex() << "\n";
            }
            std::cout << "----------------------\n\n";
        } else if (command == "election") {
            raft::NodeId candidateId;
            std::cin >> candidateId;
            std::cout << "Starting election for Node " << candidateId << "...\n";
            cluster.startElection(candidateId);
            std::cout << "Election complete.\n";
        } else if (command == "put") {
            raft::NodeId leaderId;
            std::string key, val;
            std::cin >> leaderId >> key >> val;

            auto leader = cluster.getNode(leaderId);
            if (!leader) {
                std::cout << "Error: Invalid Node ID.\n";
                continue;
            }

            raft::LogEntry entry{
                .term = leader->getCurrentTerm(),
                .index = leader->getLogSize() + 1,
                .command = {raft::CommandType::Put, key, val}
            };

            raft::AppendEntriesArgs args{
                .term = leader->getCurrentTerm(),
                .leaderId = leaderId,
                .prevLogIndex = leader->getLogSize(),
                .prevLogTerm = leader->getCurrentTerm(),
                .entries = {entry},
                .leaderCommit = leader->getLogSize() + 1
            };

            for (size_t i = 1; i <= cluster.getClusterSize(); ++i) {
                auto node = cluster.getNode(static_cast<raft::NodeId>(i));
                node->handleAppendEntries(args);
                node->applyCommittedEntries();
            }

            std::cout << "Successfully replicated key '" << key << "' across cluster.\n";
        } else if (command == "get") {
            raft::NodeId nodeId;
            std::string key;
            std::cin >> nodeId >> key;

            auto node = cluster.getNode(nodeId);
            if (!node) {
                std::cout << "Error: Invalid Node ID.\n";
                continue;
            }

            auto res = node->getValue(key);
            if (res.has_value()) {
                std::cout << "Node " << nodeId << " -> " << key << ": " << res.value() << "\n";
            } else {
                std::cout << "Node " << nodeId << " -> Key '" << key << "' not found.\n";
            }
        } else {
            std::cout << "Unknown command. Type 'help' for available commands.\n";
        }
    }

    return 0;
}
