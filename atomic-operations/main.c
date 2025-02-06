#include "stdio.h"
#include "unistd.h"
#include "sys/wait.h"
#include "stdlib.h"
#include "pthread.h"

unsigned long long cnt = 0;

void foo()
{
  cnt++;
}

void bar()
{
  __sync_add_and_fetch(&cnt, 1);
}

void *qwerty(void *xpos)
{
  int pos = xpos;

  cnt |= (1<<pos);

  return NULL;
}

void *_sync_qwerty(void *xpos)
{
  int pos = xpos;
  __sync_fetch_and_or_8(&cnt, (1ULL << pos));

  return NULL;
}


#define NTH 31

int main(int argc, char const *argv[])
{
  pthread_t *p = (pthread_t *)malloc(sizeof(pthread_t)*NTH);

  for(int i = 0; i < NTH; i++)
    pthread_create(&p[i], NULL, _sync_qwerty, (void*)i);

  for(int i = 0; i < NTH; i++)
    pthread_join(p[i], NULL);
  
  free(p);

  printf("cnt = %.31b\n", cnt);

  foobar();

  return 0;
}

// Атомарная test-and-set с отдельными битами
#define atomic_bit_test_and_set(ptr, pos) \ 
        (((__sync_fetch_and_or(ptr, (1 << pos))) >> pos) & 0x1)

void foobar()
{
  int a = 10;
  a = atomic_bit_test_and_set(&cnt, 31);
  printf("cnt = %d\n", a);
  a = atomic_bit_test_and_set(&cnt, 31);
  printf("cnt = %d\n", a);

  cnt = 0;
  a = atomic_bit_test_and_set(&cnt, 31);
  printf("cnt = %d\n", a);
}