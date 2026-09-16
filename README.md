# Raft-KV: Distributed Key-Value Store in C++20

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Build & Test](https://img.shields.io/badge/GoogleTest-v1.14.0-green.svg)](https://github.com/google/googletest)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A high-performance, thread-safe, distributed consensus key-value store built in C++20 based on the **Raft Consensus Protocol**.

## Key Features
* **Raft Consensus Engine**: Implements Leader Election, Log Matching safety rules, and Heartbeat synchronization.
* **Thread-Safe Key-Value Store**: Decoupled state machine supporting atomic `Put`, `Get`, and `Delete` operations.
* **Network Transport Layer**: In-memory RPC router simulating network communication between nodes.
* **Unit & Integration Test Suite**: 100% test coverage for log entry creation, state transitions, RPC safety, and cluster quorums via **GoogleTest**.

---

## Architecture Overview
## Architecture Overview

```text
                  +-----------------------------------+
                  |      Interactive CLI / User       |
                  +-----------------------------------+
                                    |
                                    v
                  +-----------------------------------+
                  |        Cluster Orchestrator       |
                  +-----------------------------------+
                                    |
            +-----------------------+-----------------------+
            |                       |                       |
            v                       v                       v
     +--------------+        +--------------+        +--------------+
     | Raft Node 1  |        | Raft Node 2  |        | Raft Node 3  |
     |  (Leader)    |        |  (Follower)  |        |  (Follower)  |
     +--------------+        +--------------+        +--------------+
            |                       |                       |
            +----------+------------+-----------+-----------+
                       |                        |
                       v                        v
            +--------------------+    +--------------------+
            | Network Transport  |    |  Key-Value Engine  |
            | (Simulated RPCs)   |    |  (State Machine)   |
            +--------------------+    +--------------------+
## Quick Start & Usage

### Prerequisites
* GCC 10+ / Clang 11+ (C++20 support)
* CMake 3.20+
* Make / Build tools

### Build and Run

```bash
# Clone the repository
git clone [https://github.com/your-username/raft-kv.git](https://github.com/your-username/raft-kv.git)
cd raft-kv

# Build project
mkdir build && cd build
cmake ..
make

# Run GoogleTest suite
ctest --output-on-failure

# Launch Interactive CLI
./raft_kv_cli

raft-kv> election 1             # Triggers Node 1 to run for election and collect quorum
raft-kv> put 1 session_id 99x   # Leader replicates entry across followers
raft-kv> get 2 session_id       # Returns '99x' from Node 2's applied state machine
raft-kv> status                 # Displays terms, states, log sizes, and commit indices