This code is about deterministic parallel hash tables using phase-concurrent. It explores how certain operations, finds, insertions and deletions, can be performed concurrently.

Code modified based on code provided in quicksort project of CS214. 
Some of code obtained from https://github.com/ucrparlay/Parallel-Strong-Connectivity

hash_table_d.h --> deterministic hash table

hash_table_nd.h --> nondeterministic hash table

ht_test_sq.cpp -> test file for sequential operations

ht_test_3r.cpp -> test file that repeats 3 rounds

ht_test_12.cpp -> test file for 12 inserts example

ht_test_sp1.cpp -> test file for nondeterministic hashtable efficiency 1e8 inserts

ht_test_sp2.cpp -> test file for deterministic hashtable efficiency 1e8 inserts
