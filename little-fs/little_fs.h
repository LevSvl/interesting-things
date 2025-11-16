#ifndef LITTLE_FS_H
#define LITTLE_FS_H

/* ======================================================================== */
/* Image config                                                             */
/* ======================================================================== */

#define TARGET_IMAGE_NAME   "little-fs.img"
#define IMG(base, x)  (base + x)

/* ======================================================================== */
/* Filesystem config                                                        */
/* ======================================================================== */

#define FS_TOTAL_SIZE   (64)
#define FS_MAGIC        (0x4eb14eb1U)
#define FS_BLOCK_SIZE   (4*1024)


/* ======================================================================== */
/* Filesystem layout                                                        */
/* ======================================================================== */

#define BSIZE     (FS_BLOCK_SIZE)
#define BNUM      FS_TOTAL_SIZE
#define SECSIZE   512
#define SECNUM    ((BNUM*BSIZE)/SECSIZE)
#define MAXFNAME  32
#define NDIRECT   11
#define ROOTINO   2

#define inodeBitMapAddr   BSIZE
#define dataBitMapAddr    2*BSIZE
#define BITS_PER_BITMAP(T)  (T) (sizeof(T)*CHAR_BIT)

#define inodeStartAddr    3*BSIZE
#define dataStartAddr     8*BSIZE

#define blk(inum)         (inum*sizeof(inode_t)/BSIZE)
#define sector(blk)       ((blk*BSIZE + inodeStartAddr)/SECSIZE)

#define MAXFILES          ((dataStartAddr - inodeStartAddr)/sizeof(inode_t))
#define MAX_BLOCKS_PER_FILE NDIRECT

#define inum(off)         ((off - inodeStartAddr)/sizeof(inode_t))
#define inode_off(inum)   (inum*sizeof(inode_t))

#define BLOCK_NUM(addr)   (addr/BSIZE)

/* ======================================================================== */
/* Filesystem descriptors                                                   */
/* ======================================================================== */

typedef struct superblock {
  uint32_t magic;               // Magic bytes
  uint32_t total_blocks_num;    // Size of file system
  uint32_t dblocks_num;         // Number of data blocks
  uint32_t iblocks_num;         // Number of inodes.
  uint32_t iblocks_start;       // Block number of first inode block
  uint32_t dbmap_start;         // Block number of first data map block
  uint32_t ibmap_start;         // Block number of first inode map block
} superblock_t;

enum {
    TYPE_DIR  = 1,
    TYPE_FILE = 2,
};

/* Size of inode - 16 bytes */
typedef struct inode
{
  uint16_t type;
  uint16_t nlink;
  uint32_t size;
  uint32_t addr[NDIRECT];
} inode_t;

typedef struct dirent
{
  uint8_t inum;
  uint32_t reclen;
  uint32_t strlen;
  char name[MAXFNAME];
} dirent_t;

struct file {
  enum { FD_NONE, FD_INODE, FD_DEVICE } type;
  int ref; // reference count
  char readable;
  char writable;
  struct inode *ip;  // FD_INODE and FD_DEVICE
  uint32_t off;          // FD_INODE
  short major;       // FD_DEVICE
};

#endif /* LITTLE_FS_H */