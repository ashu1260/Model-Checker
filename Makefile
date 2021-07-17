# simulator.c -- contains wrapper func of pthread_create  & sem_wait
# target.c -- contains the end use target code
# generate_schedule_ctor -- file containing ctor for generating schedules
# wrappers.c -- contains wrapper for mc_sched_yield

# man ldd -- show dependencies on dynamic libraries

# $<  input to recipe
# $@  output of recipe

# run: all
# 	LD_PRELOAD=$$PWD/libsimulator.so ./target
# gdb: all
# 	gdb --args env LD_PRELOAD=$$PWD/libsimulator.so ./target

all: libsimulator.so  Producer_consumer Leader_election dining_philosopher2 model-checker

Producer_consumer: Producer_consumer.c
	gcc -g3 -o $@ $< -pthread

dining_philosopher2: dining_philosopher2.c
	gcc -g3 -o $@ $< -pthread

Leader_election: Leader_election.c
	gcc -g3 -o $@ $< -pthread

model-checker: model-checker.c
	gcc -g3 -o $@ $< 

# wrappers.o: wrappers.c
# 	gcc -g3 -fPIC -c $<

simulator.o: simulator.c
	gcc -g3 -fPIC -c $<

libsimulator.so: simulator.o 
	gcc -shared -fPIC $< -ldl  -o $@ -pthread

clean:
	rm -f a.out simulator model-checker target target_2 lib*.so *.o

dist: clean
	dir=`basename $$PWD` && cd .. && tar zcvf $$dir.tar.gz ./$$dir
	dir=`basename $$PWD` && ls -l ../$$dir.tar.gz
