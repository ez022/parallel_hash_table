ifdef CLANG
CC = clang++
else
CC = g++
endif

CPPFLAGS = -std=c++17 -Wall -Werror

ifdef CILKPLUS
CPPFLAGS += -DPARLAY_CILKPLUS -DCILK -fcilkplus
else ifdef SERIAL
CPPFLAGS += -DPARLAY_SEQUENTIAL
else
CPPFLAGS += -pthread
endif

ifdef DEBUG
CPPFLAGS += -Og -mcx16 -DDEBUG
else ifdef PERF
CPPFLAGS += -Og -mcx16 -march=native -g
else ifdef MEMCHECK
CPPFLAGS += -Og -mcx16 -DPARLAY_SEQUENTIAL
else ifdef MXMAC
CPPFLAGS += -O3 -mcpu=apple-m1
else
CPPFLAGS += -O3 -mcx16 -march=native
endif

all: test1

test1: hash_table.h ht_test.cpp
	$(CC) $(CPPFLAGS) ht_test.cpp -o test1

clean:
	rm test1
