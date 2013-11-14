
CC = g++-4.7
CFLAGS = -O3

all: randtrack 

randtrack: list.h hash.h defs.h randtrack.cc
	$(CC) $(CFLAGS) randtrack.cc -o randtrack

tm: list.h hash.h defs.h randtrack_tm.cc
	$(CC) $(CFLAGS) randtrack_tm.cc -o randtrack

randtrack_list_lock: list.h hash.h defs.h randtrack_list_lock.cc
	$(CC) $(CFLAGS) randtrack.cc -o randtrack

global: list.h hash.h defs.h randtrack_global_lock.cc
	$(CC) $(CFLAGS) randtrack_global_lock.cc -o randtrack


clean:
	rm -f *.o randtrack randtrack_global_lock randtrack_tm randtrack_list_lock
