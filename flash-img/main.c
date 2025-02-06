#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "fcntl.h"
#include "assert.h"
#include <stdlib.h>
#include <stdint.h>

#define BSIZE   1024
#define BNUM    4096 

unsigned char data[1024*4096];

int main(int argc, char const *argv[])
{
  // создание файла для образа
  int imgfd = open("flash.img", O_CREAT | O_WRONLY,777);
  assert(imgfd > 0);

  for(size_t i = 0; i < 0x1000; i++){
      data[i] = (i%2) ? 0xF: 0xA;
  }

  for(size_t i = 0x1000; i < BSIZE*BNUM; i++){
    data[i] = 0;
  }

  write(imgfd, data, BSIZE*BNUM);
  close(imgfd);

  return 0;
}
