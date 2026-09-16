add_test([=[LogEntryTest.BasicEntryCreation]=]  /home/johns/raft-kv/build/tests/log_entry_test [==[--gtest_filter=LogEntryTest.BasicEntryCreation]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LogEntryTest.BasicEntryCreation]=]  PROPERTIES DEF_SOURCE_LINE /home/johns/raft-kv/tests/log_entry_test.cpp:4 WORKING_DIRECTORY /home/johns/raft-kv/build/tests SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  log_entry_test_TESTS LogEntryTest.BasicEntryCreation)
