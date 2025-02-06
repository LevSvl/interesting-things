#include "pthread.h"
#include "stdio.h"
#include "unistd.h"
#include <stdlib.h>
#include <string.h>

#define N 12

void *nop(){  
  while(1)
    ;
  return 0;
}

void
allocStacksTest()
{
  int pid1, pid2, pid3;
  int tid;

  if((pid1 = fork()) < 0){
    printf("fork 1: failed");
    exit(1);
  }

  if(pid1 == 0){
    pthread_t tasks1[N/3];
    for(int i = 0; i < N/3; i++){
      tid = pthread_create(tasks1 + i, NULL, nop, 0);
      printf("pid: %d: tid: %d\n", getpid(), tid);
    }
    sleep(5);
    exit(0);
  }

  if((pid2 = fork()) < 0){
    printf("fork 2: failed");
    exit(1);
  }

  if(pid2 == 0){
    pthread_t tasks2[N/3];
    for(int i = 0; i < N/3; i++){
      tid = pthread_create(tasks2 + i,NULL , nop, 0);
      printf("pid: %d: tid: %d\n", getpid(), tid);
    }
    sleep(5);
    exit(0);
  }

  if((pid3 = fork()) < 0){
    printf("fork 3: failed");
    exit(1);
  }

  if(pid3 == 0){
    pthread_t tasks3[N/3];
    for(int i = 0; i < N/3; i++){
      tid = pthread_create(tasks3 + i, NULL, nop, 0);
      printf("pid: %d: tid: %d\n", getpid(), tid);
    }
    sleep(5);
    exit(0);
  }
  sleep(10);

  pthread_t last_task;
  tid = pthread_create(&last_task, NULL, nop, 0); 
  printf("pid: %d: tid: %d\n", getpid(), tid);
}

void
allocStacksTest1()
{
  int pid1;
  int tid;

  for(int i = 0; i < 10000; i++ ){
    if((pid1 = fork()) < 0){
      printf("fork %d: failed\n", i);
      exit(1);
    }

    if(pid1 == 0){
      pthread_t tasks1[4];
      for(int i = 0; i < N/4; i++){
        tid = pthread_create(tasks1 + i,NULL, nop, 0);
        printf("pid: %d: tid: %d\n", getpid(), tid);
      }
      // wait(0);
      exit(0);
    }
  }

  sleep(10);

  // pthread_t last_task;
  // tid = pthread_create(&last_task, nop, 0, 0); 
  // printf("pid: %d: tid: %d\n", getpid(), tid);

  // pthread_t last_task1;
  // tid = pthread_create(&last_task1, nop, 0, 0); 
  // printf("pid: %d: tid: %d\n", getpid(), tid);
}

int main()
{
  allocStacksTest1();
  return 0;
}