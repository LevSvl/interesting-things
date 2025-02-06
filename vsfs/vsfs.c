#include "stdio.h"
#include "vsfs.h"
#include "string.h"
#include "unistd.h"
#include "fcntl.h"
#include "assert.h"
#include <stdlib.h>

unsigned char data[BSIZE*BNUM];

uint32 ialloc();
uint32 balloc();
void create_root();
int appenddirent(uint8 inumparent, dirent_t *dstdirent);

int main(int argc, char const *argv[])
{
  // создание файла для образа
  int imgfd = open("vsfs.img", O_CREAT | O_WRONLY);
  assert(imgfd > 0);

  struct superblock sb;
  sb.nblocks = BNUM;
  sb.inodestart = inodeStartAddr;
  sb.logstart = 0;
  sb.bmapstart = inodeBitMapAddr;
  sb.ninodes = 64UL/sizeof(inode_t);
  sb.nlog = 0;
  sb.size = BNUM*BSIZE;
  sb.magic = FSMAGIC;
  memcpy(data, &sb, sizeof(sb));

  create_root();
  // изначальные файлы в vsfs
  for(int filecnt = 0; filecnt < argc;  filecnt++){
    if(filecnt > MAXFILES)
      break;
    
    const char *dstFname = argv[filecnt];

    // открытие целевых файлов
    int dstfd = open(dstFname, O_RDONLY);
    assert(dstfd > 0);

    // буфер с содержимым файла
    char datatmp[BSIZE*BNUM];
    memset(datatmp, 0, BSIZE*BNUM*sizeof(char));

    // вычисление размера файла
    int dstsize = 0;
    while(read(dstfd, datatmp, BSIZE) > 0){
      dstsize += strlen(datatmp);
    }
    // файл не должен занимать больше блоков, 
    // чем способен адресовать inode
    assert(dstsize < BSIZE*NDIRECT);

    // сколько блоков нужно для размещения файла?
    int blksfordst = dstsize / BSIZE;
    if (blksfordst == 0 && dstsize > 0)
      blksfordst = 1;
    
    // сохранить inode
    uint32 dstNodeOffset = ialloc();
    assert(dstNodeOffset > 0);

    inode_t *dstinode = (inode_t*)malloc(sizeof(inode_t));
    dstinode->size = dstsize;
    dstinode->type = FILE_TYPE;
    dstinode->nlink = 1;
    for(int i = 0; i < NDIRECT; i++)
      dstinode->addr[i] = 0;
    
    // найти первый свободный блок
    for(int i = 0; i < blksfordst; i++){
      dstinode->addr[i] = balloc();
      assert(dstinode->addr[i] > 0);
    }

    // скопировать структуру inode в массив индексных дескрипторов
    memcpy(data + dstNodeOffset, dstinode, sizeof(inode_t));
    
    // скопировать данные из файла в блоки на диске 
    for(int i = 0; i < NDIRECT; i++){
      if(dstinode->addr[i] == 0)
        continue;
      uint32 dataoff = dstinode->addr[i];
      memcpy(data + dataoff, datatmp, BSIZE);
    }

    dirent_t dstdirent;
    dstdirent.inum = inum(dstNodeOffset);
    strcpy(dstdirent.name, dstFname);
    dstdirent.strlen = strlen(dstFname);
    dstdirent.reclen = dstsize + dstdirent.strlen;
    appenddirent(ROOTINO, &dstdirent);

    free(dstinode);
  }

  write(imgfd, data, BSIZE*BNUM);
  close(imgfd);

  printf("%s\n", data + dataStartAddr + BSIZE*3);
  // printf("%u\n", strlen(datatmp));
  // printf("%u\n", MAXFILES);

  return 0;
}

// на самом деле заполняет битовую карту для индексных дескрипторов
// инициализация тоже должна быть, возможно отдельно
uint32 ialloc()
{
  uint64 *p;
  int i;
  
  p = (uint64*)(data + inodeBitMapAddr);
  
  for(i = 0; i < MAXFILES; i++){
    if(*p & (0x1UL << i))
      continue;
    *p |= (1 << i);
  
    return i*sizeof(inode_t) + inodeStartAddr; 
  }

  return 0; 
}

// заполняет битовую карту и возвращает указатель на блок данных для файла
uint32 balloc(){
  uint64 *p;
  int i;
  
  p = (uint64*)(data + dataBitMapAddr);
  
  for(i = 0; i < MAXFILES; i++){
    if(*p & (0x1UL << i))
      continue;
    *p |= (1 << i);

    printf("balloc: %d\n", i);
    return i*BSIZE + dataStartAddr;
  }

  return 0;
}

void create_root()
{
  inode_t rootNode;
  uint64* p;

  rootNode.nlink = 1;
  rootNode.size = 0;
  rootNode.type = DIR_TYPE;
  for(int i = 0; i < NDIRECT; i++){
    rootNode.addr[i] = balloc();
    assert(rootNode.addr[i] > 0);
  }

  // скопировать структуру inode в массив индексных дескрипторов
  p = (uint64*)(data + inodeBitMapAddr);
  *p |= (0x1UL << ROOTINO);
  uint32 rootioff = BSIZE*ROOTINO + inodeStartAddr; 
  memcpy(data + rootioff, &rootNode, sizeof(inode_t));
}

int appenddirent(uint8 inumparent, dirent_t *dstdirent)
{
  inode_t *parentinode;

  parentinode = (inode_t*)(data + inodeStartAddr + inumparent*sizeof(inode_t));
  assert(parentinode->type != FILE_TYPE);

  uint32 direntoff = parentinode->addr[0];
  while(*(data + direntoff))
    direntoff += sizeof(dirent_t);
  
  memcpy(data + direntoff, dstdirent, sizeof(dirent_t));
  return 0; 
}