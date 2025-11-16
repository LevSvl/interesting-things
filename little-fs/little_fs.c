#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "little_fs.h"

uintptr_t img_base;

/* ======================================================================== */
/* Filesystem creation helpers                                              */
/* ======================================================================== */

superblock_t sb;
char tmp_buf[BSIZE];

int img_open()
{
    int img_fd;
    unsigned long bytes_written = 0;
    uint64_t null = 0;
    
    img_fd = open(TARGET_IMAGE_NAME, O_CREAT | O_RDWR, S_IRWXU);
    if (img_fd < 0)
        return img_fd;

    while (bytes_written < FS_TOTAL_SIZE * FS_BLOCK_SIZE) {
        unsigned long writen_curent;

        writen_curent = write(img_fd, &null, sizeof(null));
        if (writen_curent == 0)
            goto mmap_failed;
        bytes_written += writen_curent;
    }

    if ((img_base = (uintptr_t)mmap(NULL, FS_TOTAL_SIZE * FS_BLOCK_SIZE,
         PROT_READ | PROT_WRITE, MAP_SHARED, img_fd, 0)) == (uintptr_t)MAP_FAILED) {
        goto mmap_failed;
    }

    return img_fd;

mmap_failed:
    close(img_fd);
    return -1;
}

void img_close(int fd)
{
    munmap((void *)img_base, FS_TOTAL_SIZE * FS_BLOCK_SIZE);
    close(fd);
}

void superblock_create()
{
    sb.total_blocks_num = FS_TOTAL_SIZE;
    sb.magic = FS_MAGIC;
    sb.bmap_start = inodeBitMapAddr;
    sb.iblocks_start = inodeStartAddr / BSIZE;
    sb.iblocks_num = (dataStartAddr - inodeStartAddr) / BSIZE;
    sb.dblocks_num = (FS_TOTAL_SIZE*BSIZE - dataStartAddr) / BSIZE;

    memcpy((void *)img_base, &sb, sizeof(sb));
}

uint32_t ialloc()
{
    uint64_t *ibit_map;
    int target_inum = 0;

    ibit_map = (uint64_t *)IMG(img_base, inodeBitMapAddr);

    while (ibit_map < (uint64_t *)IMG(img_base, dataStartAddr)) 
    {
        int i;

        for (i = 0; i < BITS_PER_BITMAP(uint64_t); i++) {
            if (*ibit_map & (1ULL << i))
                continue;
            *ibit_map |= (1ULL << i);
            return inodeStartAddr + ((target_inum + i)*sizeof(inode_t));
        }

        target_inum += i;
        ibit_map++;
    }

  return 0; 
}

uint32_t balloc()
{
    uint64_t *dbit_map;
    int target_block_num = 0;

    dbit_map = (uint64_t *)IMG(img_base, dataBitMapAddr);

    while (dbit_map < (uint64_t *)IMG(img_base, inodeStartAddr)) 
    {
        int i;

        for (i = 0; i < BITS_PER_BITMAP(uint64_t); i++) {
            if (*dbit_map & (1ULL << i))
                continue;
            *dbit_map |= (1ULL << i);
            return dataStartAddr + target_block_num + i*BSIZE;
        }

        target_block_num += i;
        dbit_map++;
    }

    return 0;
}

void rootdir_create()
{
    uint64_t *ibit_map;
    inode_t inode_root;
    uintptr_t inode_addr_root;

    inode_root.nlink = 1;
    inode_root.size = 0;
    inode_root.type = TYPE_DIR;
    for (int i = 0; i < NDIRECT; i++) {
        inode_root.addr[i] = balloc();
        assert(inode_root.addr[i] > 0);
    }

    ibit_map = (uint64_t *)IMG(img_base, inodeBitMapAddr);
    *ibit_map |= (0x1ULL << ROOTINO);

    inode_addr_root = inodeStartAddr + ROOTINO*sizeof(inode_t);

    memcpy((void *)IMG(img_base, inode_addr_root), &inode_root, sizeof(inode_t));
}

void add_dirent(int parent_inum, dirent_t *dirent)
{
    uint32_t dirent_off;
    inode_t *inode_parent;

    inode_parent = (inode_t*)IMG(img_base, inodeStartAddr + parent_inum*sizeof(inode_t));
    assert(inode_parent->type != TYPE_FILE);

    dirent_off = inode_parent->addr[blk(parent_inum)];
    assert(dirent_off > 0);

    while(((dirent_t *)IMG(img_base, dirent_off))->strlen)
        dirent_off += sizeof(dirent_t);
  
    memcpy((void *)IMG(img_base, dirent_off), dirent, sizeof(dirent_t));
}

void print_litle_fs_config()
{
    printf("little-fs               \n");
    printf("Block size: %d bytes    \n", FS_BLOCK_SIZE);
    printf("Total size: %d blocks   \n", sb.total_blocks_num);
    printf("Data: %d blocks         \n", sb.dblocks_num);
    printf("Data start block: %d    \n", dataStartAddr);
    printf("Inodes: %d blocks       \n", sb.iblocks_num);
    printf("Inodes start block: %d  \n", sb.iblocks_start);
    printf("Bitmap start block: %d  \n", sb.bmap_start);
    printf("Magic: 0x%8x            \n", sb.magic);
}

int main(int argc, char const *argv[])
{
    int little_fs_img_fd;

    if (argc < 2) {
        printf("No files provided\n");
        return 1;
    }
    
    little_fs_img_fd = img_open();
    assert(little_fs_img_fd > 0);

    superblock_create();
    rootdir_create();

    for (int file_cnt = 1; file_cnt < argc; file_cnt++) {
        int file_fd;
        int file_size, blocks_req;
        inode_t inode;
        uint32_t inode_off;
        dirent_t dirent;
        const char *file_name = argv[file_cnt];
        
        if (file_cnt > MAXFILES)
            break;
        
        file_fd = open(file_name, O_RDONLY);
        assert(file_fd > 0);


        /* Only count file size */
        for (uint32_t bytes_read = 0xFFFFFFFF; bytes_read > 0;) {
            bytes_read = read(file_fd, tmp_buf, BSIZE);
            file_size += bytes_read;
        }
        assert(file_size < BSIZE*NDIRECT);
        /* Count blocks required */
        blocks_req = file_size / BSIZE;
        if (blocks_req == 0 && file_size > 0)
            blocks_req = 1;


        /* Create inode for file */
        inode_off = ialloc();
        assert(inode_off > 0);

        inode.size = file_size;
        inode.type = TYPE_FILE;
        inode.nlink = 1;

        for (int i = 0; i < NDIRECT; i++) {
            if (i < blocks_req) {
                inode.addr[i] = balloc();
                assert (inode.addr[i] != 0);
            }
            else {
                inode.addr[i] = 0;
            }
        }

        /* Place inode for file */
        memcpy((void *)IMG(img_base, inode_off), &inode, sizeof(inode_t));

        /* Place data for file */
        for (uint32_t i = 0, bytes_left = file_size; i < NDIRECT; i++) {
            if (inode.addr[i] == 0)
                continue;

            lseek(file_fd, i*BSIZE, SEEK_SET);

            memset(tmp_buf, 0, BSIZE);
            bytes_left -= read(file_fd, tmp_buf, BSIZE);
            assert(!(((i + 1) == NDIRECT) && bytes_left != 0));

            memcpy((void *)IMG(img_base, inode.addr[i]), tmp_buf, BSIZE);
        }

        /* Add file into the root directory */
        dirent.inum = inum(inode_off);
        dirent.strlen = strlen(file_name);
        dirent.reclen = sizeof(dirent_t);
        memcpy(&dirent.name, file_name, strlen(file_name));
        dirent.name[strlen(file_name)] = 0;
        add_dirent(ROOTINO, &dirent);
    }

    img_close(little_fs_img_fd);
    print_litle_fs_config();
    
    return 0;
}
