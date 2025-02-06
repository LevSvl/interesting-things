#define BSIZE     4096
#define BNUM      64
#define SECSIZE   512
#define SECNUM    ((BNUM*BSIZE)/SECSIZE)
#define MAXFNAME  32
#define NDIRECT   12

#define DIR_TYPE  1
#define FILE_TYPE 2

#define FSMAGIC 0x10203040

#define ROOTINO 1

/* 
types
*/
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

struct superblock {
  uint32 magic;        // Must be FSMAGIC
  uint32 size;         // Size of file system image (blocks)
  uint32 nblocks;      // Number of data blocks
  uint32 ninodes;      // Number of inodes.
  uint32 nlog;         // Number of log blocks
  uint32 logstart;     // Block number of first log block
  uint32 inodestart;   // Block number of first inode block
  uint32 bmapstart;    // Block number of first free map block
};

// sizeof(inode) == 16 bytes
typedef struct inode
{
  uint16 type;    // 1 - file, 2 - dir
  uint16 nlink;
  uint32 size;
  uint32 addr[NDIRECT];
} inode_t;

typedef struct dirent
{
  uint8 inum;
  uint32 reclen;
  uint32 strlen;
  char name[MAXFNAME];
} dirent_t;
/*
*/

#define superBlkAddr      0UL

// bitmaps
#define inodeBitMapAddr   BSIZE
#define dataBitMapAddr    2*BSIZE

#define inodeStartAddr    3*BSIZE
#define dataStartAddr     8*BSIZE

#define blk(inum)         (inum*sizeof(inode_t)/BSIZE)
#define sector(blk)       ((blk*BSIZE + inodeStartAddr)/SECSIZE)

#define MAXFILES          ((dataStartAddr - inodeStartAddr)/sizeof(inode_t))

#define inum(off)         ((off - inodeStartAddr)/sizeof(inode_t))