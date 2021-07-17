// USAGE:  model-checker ./target
//  (OR: ./target  [for native execution]
// target.c completely transparent except for:  #include "simulator.h"
// If target.c doesn't use mc_sched_yield (e.g., simple mutexes),
//    then we don't need:  #include "simulator.h"
//    simulator.h is only to define mc_sched_yield() for native execution
// If we have shared variables, then we need mc_sched_yield() to allow
//    the simulator to stop after reading/writing a shared variable.

#define _GNU_SOURCE
#include <unistd.h>
#include<stdlib.h>
#include<stdio.h>

int main(int argc, char *argv[]) {
  setenv("LD_PRELOAD", "./libsimulator.so",0);
  int rc = execvp( argv[1], argv+1 ); 
  // argv+1 will be an array starting at argv[1]
  if(rc == -1) perror("execvp");
  return 0;
}
