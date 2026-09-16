#include <gtest/gtest.h>
#include "raft/log_entry.hpp"

TEST(LogEntryTest, BasicEntryCreation) {
    raft::LogEntry entry{
        .term = 1,
        .index = 100,
        .command = {
            .type = raft::CommandType::Put,
            .key = "user:100",
            .value = "John"
        }
    };

    EXPECT_EQ(entry.term, 1);
    EXPECT_EQ(entry.index, 100);
    EXPECT_EQ(entry.command.key, "user:100");
}
