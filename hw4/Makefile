
CC = g++-4.7
CFLAGS = -O3

all: randtrack tm el global redu randtrack_list_lock

randtrack: list.h hash.h defs.h randtrack.cc
	$(CC) $(CFLAGS) randtrack.cc -o randtrack

tm: list.h hash.h defs.h randtrack_tm.cc
	$(CC) $(CFLAGS) randtrack_tm.cc -o randtrack_tm -fgnu-tm

randtrack_list_lock: list.h hash.h defs.h randtrack_list_lock.cc
	$(CC) $(CFLAGS) randtrack_list_lock.cc -o randtrack_list_lock -pthread

el: list.h hash.h defs.h randtrack_el.cc
	$(CC) $(CFLAGS) randtrack_el.cc -o randtrack_el -pthread

global: list.h hash.h defs.h randtrack_global_lock.cc
	$(CC) $(CFLAGS) randtrack_global_lock.cc -o randtrack_global_lock -pthread

redu: hash_reduction.h list_reduction.h defs.h randtrack_reduction.cc
	$(CC) $(CFLAGS) randtrack_reduction.cc -o randtrack_reduction -pthread


clean:
	rm -f *.o randtrack randtrack_global_lock randtrack_tm randtrack_list_lock
