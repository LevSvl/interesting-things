#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "fsconfig.h"

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

struct superblock {
    uint32 magic;        // Must be FSMAGIC
    uint32 size;         // Size of file system image (blocks)
    uint32 nblocks;      // Number of data blocks
    uint32 ninodes;      // Number of inodes.
    uint32 inodestart;   // Block number of first inode block
    uint32 bmapstart;    // Block number of first free map block
};

#define TYPE_FILE   1
#define TYPE_DIR    2

typedef struct inode
{
    uint16 type;
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

/*   Нумерация в блоках   */
#define SB_NUM      0
#define IBMP_NUM    1
#define DBMP_NUM    2
#define I_NUM       3
#define D_NUM       8

/*   Размеры в блоках   */
#define SB_SZ      IBMP_NUM - SB_NUM
#define IBMP_SZ    DBMP_NUM - SB_SZ
#define DBMP_SZ    I_NUM - DBMP_NUM
#define I_NUM_SZ   D_NUM - I_NUM
#define D_NUM_SZ   BNUM - D_NUM




#define BLK_ADDR(bnum)  (bnum*BSIZE)

#define get_inode_blk_num(inum)        (inum*sizeof(inode_t)/BSIZE)
#define get_inode_sector_num(bnum)     ((bnum*BSIZE+BLK_ADDR(I_NUM))/SECSIZE)

#define MAXFILES          ((BLK_ADDR(D_NUM) - BLK_ADDR(I_NUM))/sizeof(inode_t))

#define inum(off)         ((off - BLK_ADDR(I_NUM))/sizeof(inode_t))



// Содержимое образа
uint8 img_content[BSIZE*BNUM];


struct superblock create_sb()
{
    struct superblock sb;
    sb.nblocks = BNUM;
    sb.inodestart = BLK_ADDR(I_NUM);
    sb.bmapstart = BLK_ADDR(IBMP_NUM);
    sb.ninodes = 0;
    sb.size = BNUM*BSIZE;
    sb.magic = FSMAGIC;

    return sb;
}

// Заполняет битовую карту
// и возвращает указатель
// на блок данных для файла
// 
// Представление блока битовой карты:
//
// inode: Первые 64                             Последние 64              
//       ____________________________________________________
//      |            |            |            |            |
//      |            |           ...           |            |
//      |            |           ...           |            |
//      |____________|____________|____________|____________|
//
uint32 balloc(){
  uint32 *p;
  uint32 bmp_first_addr = BLK_ADDR(DBMP_NUM);

  p = (uint32*)(img_content + bmp_first_addr);
  
  for(int i = 0; i < MAXFILES; i++) {
    if(*p & (1UL << i))
      continue;
    *p |= (1 << i);
    return i*BSIZE + BLK_ADDR(D_NUM);
  }

  return 0;
}

uint32 ialloc()
{
  uint32 *p;
  
  p = (uint32*)(img_content + BLK_ADDR(IBMP_NUM));
  
  for(int i = 0; i < MAXFILES; i++){
    if(*p & (1UL << i))
      continue;
    *p |= (1 << i);
  
    return i*sizeof(inode_t) + BLK_ADDR(IBMP_NUM); 
  }

  return 0; 
}

void iinit(inode_t *inode, uint16 type, uint32 sz, uint32 bnum)
{
    inode->size = sz;
    inode->type = type;
    inode->nlink = 1;
    for(int i = 0; i < NDIRECT; i++)
        inode->addr[i] = 0;

    for(int i = 0; i < bnum; i++){
        inode->addr[i] = balloc();
        assert(inode->addr[i] > 0);
    }
}

void add_dirent(uint8 inum, const dirent_t *dirent)
{
    inode_t *parent_inode;
    uint32 ioff, doff; 

    ioff = inum*sizeof(inode_t);
    parent_inode = (inode_t*)(img_content + BLK_ADDR(I_NUM) + ioff);

    assert(parent_inode->type != FILE_TYPE);

    doff = parent_inode->addr[0];
    while(*(img_content + doff))
        doff += sizeof(dirent_t);
    memcpy(img_content + doff, dirent, sizeof(dirent_t));
}

void create_root_dir()
{
    inode_t root_inode;
    uint32 *p, ioff;

    root_inode.nlink = 1;
    root_inode.size = 0;
    root_inode.type = TYPE_DIR;

    for(int i = 0; i < NDIRECT; i++) {
        root_inode.addr[i] = balloc();
        assert(root_inode.addr[i] > 0);
    }

    p = (uint32 *)(img_content + BLK_ADDR(IBMP_NUM));
    *p |= (1UL << ROOTINO);

    ioff = BLK_ADDR(ROOTINO) + BLK_ADDR(I_NUM);
    memcpy(img_content + ioff, &root_inode, sizeof(inode_t));
}


int main(int argc, char const *argv[])
{
    int img_fd;
    
    img_fd = open("vsfs.img", O_CREAT | O_WRONLY, 777);
    assert(img_fd > 0);

    struct superblock sb = create_sb();
    memcpy(img_content + BLK_ADDR(SB_NUM), &sb, sizeof(sb));

    for(int filecnt = 0; filecnt < argc; filecnt++) {
        if(filecnt > MAXFILES)
            break;


        const char *fname = argv[filecnt];
        int fd = open(fname, O_RDONLY);
        assert(fd > 0);


        uint32 fsize, n = 0;
        char data_buf_tmp[MAXFSIZE];
        memset(data_buf_tmp, 0, MAXFSIZE*sizeof(char));
        while(read(fd, data_buf_tmp + n, BSIZE) > 0)
            n += strlen(data_buf_tmp);
        fsize = n;
        assert(fsize < MAXFSIZE);

        
        int blocks_required = fsize / BSIZE;
        if (blocks_required == 0 && fsize > 0)
            blocks_required = 1;

        uint32 ioff = ialloc();
        assert(ioff > 0);

        inode_t inode;
        iinit(&inode, TYPE_FILE, fsize, blocks_required);
        
        // Скопировать inode
        memcpy(img_content + ioff, &inode, sizeof(inode_t));
    
        // Скопировать данные
        uint32 doff;
        for(int i = 0; i < NDIRECT; i++){
            if(inode.addr[i] == 0)
                continue;
            doff = inode.addr[i];
            memcpy(img_content + doff, 
                    data_buf_tmp + i*BSIZE, BSIZE);
        }

        // Создать запись в директории для 
        // файла и связать ее с корневой папкой
        dirent_t fdirent;
        fdirent.inum = inum(ioff);
        fdirent.strlen = strlen(fname);
        memcpy(fdirent.name, fname, fdirent.strlen);
        fdirent.reclen = fdirent.strlen + fsize;

        add_dirent(ROOTINO, &fdirent);
    }

    write(img_fd, img_content, BSIZE*BNUM);
  
  return 0;
}
