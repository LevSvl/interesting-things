#include <stdlib.h>
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

// #define CONFIG_DEBUG_DATA

#define info(fmt, ...) (printf("Info: "fmt"\n", ## __VA_ARGS__))
#define warn(fmt, ...) (printf("Warning: "fmt"\n", ## __VA_ARGS__))
#define err(fmt, ...) (printf("Error: "fmt"\n", ## __VA_ARGS__))

/* ======================================================================== */
/* Filesystem mount                                                         */
/* ======================================================================== */

uintptr_t fs_base;
int fs_fd;

extern void __attribute__((constructor)) mount_fs();
extern void __attribute__((destructor)) umount_fs();


/* ======================================================================== */
/* Work with Filesystem                                                     */
/* ======================================================================== */

#define min(a, b) ((a < b) ? a : b)

#define MAX_FILES_OPENED    4

#define FS_O_RDONLY  0x000
#define FS_O_WRONLY  0x001
#define FS_O_RDWR    0x002
#define FS_O_CREATE  0x200
#define FS_O_TRUNC   0x400

inode_t *current_dir;
superblock_t * sb;

/*
 * This macro is used for low level functions
 * that work with storage device to read and
 * write filesystem's blocks
 */
#define fs_low_level_storage

/*
 * This macro is used for low level functions
 * that work with inmem inodes that is why
 * such functions must care about state 
 * of inmem inodes they use
 */
#define fs_low_level_inode

#define INMEM_BLOCKS_NUM    10
#define INMEM_INODES_NUM    10

enum state {UNUSED, BUSY};

struct inmem_block {
    enum state state;
    char inmem_addr[BSIZE];
    uint32_t dev_block_num;
} inmem_blocks[INMEM_BLOCKS_NUM];

struct inmem_inode {
    enum state state;
    inode_t inode;
} inmem_inodes[INMEM_INODES_NUM];

int opened_files_cnt = 0;
struct file opened_files[MAX_FILES_OPENED];

int fs_init();
int fs_open(char * path, int flags);
int fs_read(int fd, void *buf, uint32_t size);
int fs_write(int fd, void *buf, uint32_t size);
int fs_close(int fd);


int main(int argc, char const *argv[])
{
    int fd, n, r;
    char tmp_buf[BSIZE], *s = "babadeda\n";

    /* Init file system */
    if (fs_init()) {
        err("FS initialization failed\n");
        exit(1);
    }
    info("FS initialization ok\n");

    /* Test open */
    fd = fs_open("/Makefile", FS_O_RDONLY);
    if (fd < 0) {
        err("File \"/Makefile\" open failed\n");
        exit(1);
    }
    info("Open \"/Makefile\" ok, fd == %d\n", fd);

    /* Test read */
    n = fs_read(fd, tmp_buf, 1000);
    tmp_buf[279] = 0;
    if (n != 279) {
#ifdef CONFIG_DEBUG_DATA
        warn("data = %s\n", tmp_buf);
#endif
        err("Read \"/Makefile\" failed, read %d bytes\n", n);
        exit(1);
    }
    info("Read \"/Makefile\" ok\n");
#ifdef CONFIG_DEBUG_DATA
    info("data = %s\n", tmp_buf);
#endif
    /* Test close */
    r = fs_close(fd);
    if (r != 0) {
        err("Close \"/Makefile\" failed\n");
        exit(1);
    }
    n = fs_read(fd, tmp_buf, 1000);
    if (n != -1) {
        err("Can read \"/Makefile\" after close, read %d bytes\n", n);
        exit(1);
    }

    /* Test write */
    fd = fs_open("/Makefile", FS_O_RDWR);
    if (fd < 0) {
        err("File \"/Makefile\" open failed\n");
        exit(1);
    }
    info("Reopen \"/Makefile\" ok, fd == %d\n", fd);

    memset(tmp_buf, 0, sizeof(char)*BSIZE);
    memcpy(tmp_buf, s, strlen(s));

    n = fs_write(fd, tmp_buf, strlen(s));
    if (n != strlen(s)) {
        err("New write \"/Makefile\" failed, wrote %d bytes\n", n);
        exit(1);
    }
    r = fs_close(fd);
    if (r != 0) {
        err("Reclose \"/Makefile\" failed\n");
        exit(1);
    }

    fd = fs_open("/Makefile", FS_O_RDONLY);
    if (fd < 0) {
        err("File \"/Makefile\" reopen after write failed\n");
        exit(1);
    }
    info("Reopen after write \"/Makefile\" ok, fd == %d\n", fd);

    n = fs_read(fd, tmp_buf, 1000);
    if (memcmp(tmp_buf, s, strlen(s))) {
        err("Read after write \"/Makefile\" failed,"
            "read %d bytes\n", n);
#ifdef CONFIG_DEBUG_DATA
        err("data = %s\n", tmp_buf);
#endif
        exit(1);
    }
#ifdef CONFIG_DEBUG_DATA
    info("data = %s\n", tmp_buf);
#endif
    info("Write to \"/Makefile\" ok, fd == %d\n", fd);
    r = fs_close(fd);
    if (r != 0) {
        err("Close after testing write to \"/Makefile\" failed\n");
        exit(1);
    }

    /* Test open with trunc */
    fd = fs_open("/Makefile", FS_O_TRUNC);
    if (fd < 0) {
        err("File \"/Makefile\" reopen after write failed\n");
        exit(1);
    }
    info("Reopen after write \"/Makefile\" ok, fd == %d\n", fd);

    n = fs_read(fd, tmp_buf, 1000);
    if (n > 0) {
        err("Read after write \"/Makefile\" failed,"
            "read %d bytes\n", n);
        exit(1);
    }
    info("Read from truncated \"/Makefile\" ok, fd == %d, n = %d, \n", fd, n);
    r = fs_close(fd);
    if (r != 0) {
        err("Close after testing write to \"/Makefile\" failed\n");
        exit(1);
    }

    /* Test open with creation */
    fd = fs_open("/foo", FS_O_CREATE | FS_O_RDWR);
    if (fd < 0) {
        err("File \"/foo\" reopen after write failed\n");
        exit(1);
    }
    info("Open with creation \"/foo\" ok, fd == %d\n", fd);
    r = fs_close(fd);
    if (r != 0) {
        err("Close \"/foo\" failed\n");
        exit(1);
    }
    info("Close \"/foo\" ok\n");

    return 0;
}


/* ======================================================================== */
/* Filesystem operations                                                    */
/* ======================================================================== */

struct inmem_block * fs_low_level_storage
inmem_read_block(uint32_t block_num)
{
    for (int i = 0; i < INMEM_BLOCKS_NUM; i++) {
        if (inmem_blocks[i].state == UNUSED) {
            inmem_blocks[i].state = BUSY;
            inmem_blocks[i].dev_block_num = block_num;

            memcpy(inmem_blocks[i].inmem_addr, 
                (char *)IMG(fs_base, block_num*BSIZE), BSIZE);

            return &inmem_blocks[i];
        }
    }
    return 0;
}

void fs_low_level_storage
inmem_block_free(struct inmem_block *inmem_block, int is_dirty)
{
    if (is_dirty) {
        memcpy((char *)IMG(fs_base,inmem_block->dev_block_num*BSIZE),
                            inmem_block->inmem_addr, BSIZE);
    }
    inmem_blocks->state = UNUSED;
}

inode_t * fs_low_level_inode inmem_lock_inode()
{
    for (int i = 0; i < INMEM_INODES_NUM; i++)
        if (inmem_inodes[i].state == UNUSED) {
            inmem_inodes[i].state = BUSY;
            return &inmem_inodes[i].inode;
        }
    return 0;
}

void fs_low_level_inode inmem_free_inode(inode_t *ip)
{
    for (int i = 0; i < INMEM_INODES_NUM; i++)
        if (&inmem_inodes[i].inode == ip)
            inmem_inodes[i].state = BUSY;
}

int fs_low_level_storage
fs_inode_alloc()
{
    struct inmem_block *inmem_block;
    uint32_t *ibm0, *ibm;
    uint32_t max_off = dataBitMapAddr - inodeBitMapAddr;
    int inode_num = 0;

    inmem_block = inmem_read_block(sb->ibmap_start);
    if (inmem_block == 0)
        return 0;

    ibm0 = (uint32_t *)inmem_block->inmem_addr;

    for (ibm = ibm0;
        (uint8_t *)ibm - (uint8_t *)ibm0 < max_off;
         ibm++)  
        {
            int i = 0;
            for (i = 0; i < BITS_PER_BITMAP(uint32_t); i++) {
                if (*ibm & (1U << i))
                    continue;
                *ibm |= (1U << i);
                inmem_block_free(inmem_block, 1);
                inode_num += i;
                return inode_num;
        }
        inode_num += i;
    }

    inmem_block_free(inmem_block, 0);
    return -1;
}

void fs_low_level_storage
fs_inode_free(int inum)
{
    struct inmem_block *inmem_block;
    uint32_t *ibm;

    inmem_block = inmem_read_block(sb->ibmap_start);
    if (inmem_block == 0)
        return;

    ibm = (uint32_t *)inmem_block->inmem_addr + \
        (inum / BITS_PER_BITMAP(uint32_t));

    *ibm &= ~(1U << (inum % BITS_PER_BITMAP(uint32_t)));
    inmem_block_free(inmem_block, 1);
}

uint32_t fs_low_level_storage
fs_block_alloc()
{
    struct inmem_block *inmem_block;
    uint32_t *dbm0, *dbm;
    uint32_t max_off = inodeStartAddr - dataBitMapAddr;
    int data_block_num = 0;

    inmem_block = inmem_read_block(sb->dbmap_start);
    if (inmem_block == 0)
        return 0;

    dbm0 = (uint32_t *)inmem_block->inmem_addr;

    for (dbm = dbm0;
        (uint8_t *)dbm - (uint8_t *)dbm0 < max_off;
         dbm++)  
        {
            int i = 0;
            for (i = 0; i < BITS_PER_BITMAP(uint32_t); i++) {
                if (*dbm & (1U << i))
                    continue;
                *dbm |= (1U << i);
                inmem_block_free(inmem_block, 1);
                return dataStartAddr + data_block_num + i*BSIZE;
        }
        data_block_num += i;
    }

    inmem_block_free(inmem_block, 0);
    return 0;
}

void fs_low_level_storage
fs_block_free(uint32_t dev_block_addr)
{
    struct inmem_block *inmem_block;
    uint32_t *dbm;
    int data_block_num;

    inmem_block = inmem_read_block(sb->dbmap_start);
    if (inmem_block == 0)
        return;
    
    data_block_num = BLOCK_NUM(dev_block_addr - dataStartAddr);
    dbm = (uint32_t *)inmem_block->inmem_addr + \
        (data_block_num / BITS_PER_BITMAP(uint32_t));

    *dbm &= ~(1U << (data_block_num % BITS_PER_BITMAP(uint32_t)));
    inmem_block_free(inmem_block, 1);
}

char * fs_strtok_by_sep(char * path, char * name)
{
    char *name_curr = name;

    if (*path == '/')
        path++;

    while (*path && *path != '/') {
        *name_curr = *path;
        name_curr++;
        path++;
    }
    *name_curr = 0;

    if (*path == 0)
        return 0;

    return path;
}

inode_t * fs_low_level_storage fs_low_level_inode
fs_get_inode_by_inum(int inum)
{
    struct inmem_block *inmem_block;
    inode_t *ip0 = inmem_lock_inode(), *ip1;
    char *inode_block;

    if (inum >= MAXFILES)
        return 0;
    
    inmem_block = inmem_read_block((inodeStartAddr / BSIZE + blk(inum)));
    if (inmem_block == 0)
        return 0;
    
    inode_block = inmem_block->inmem_addr;

    ip1 = (inode_t *)(inode_block + ((inum*sizeof(inode_t)) % BSIZE));
    *ip0 = *ip1;

    inmem_block_free(inmem_block, 0);

    return ip0;
}

int fs_low_level_storage
fs_store_inode_by_inum(inode_t *ip, int inum)
{
    struct inmem_block *inmem_block;
    inode_t *on_dev_ip;
    char *inode_block;

    if (inum >= MAXFILES)
        return -1;
    
    inmem_block = inmem_read_block((inodeStartAddr / BSIZE + blk(inum)));
    if (inmem_block == 0)
        return -1;
    
    inode_block = inmem_block->inmem_addr;

    on_dev_ip = (inode_t *)(inode_block + inode_off(inum));
    *on_dev_ip = *ip;

    inmem_block_free(inmem_block, 1);

    return 0;
}

inode_t * fs_low_level_storage fs_find_inode_in_dir(inode_t *ip_parent, char *fname)
{
    struct inmem_block *inmem_block;
    inode_t *ip_child;

    if (ip_parent->type != TYPE_DIR)
        return 0;
    
    for (int i = 0; i < NDIRECT; i++) {
        char *current_block;
        dirent_t * curr_dirent;
        uint32_t curr_blk_num;

        if (ip_parent->addr[i] == 0)
            continue;

        curr_blk_num = BLOCK_NUM(ip_parent->addr[i]);

        inmem_block = inmem_read_block(curr_blk_num);
        if (inmem_block == 0)
            return 0;

        current_block = inmem_block->inmem_addr;

        curr_dirent = (dirent_t *)current_block;

        while ((uintptr_t)current_block - (uintptr_t)curr_dirent < BSIZE) {
            if (strcmp(curr_dirent->name, fname) == 0) {
                ip_child = fs_get_inode_by_inum(curr_dirent->inum);
                inmem_block_free(inmem_block, 0);
                return ip_child;
            }
            curr_dirent++;
        }

        inmem_block_free(inmem_block, 0);
    }

    return 0;
}

int fs_low_level_storage
fs_add_inode_in_dir(inode_t *ip_parent, char *fname, int inum_child)
{
    struct inmem_block *inmem_block;

    if (ip_parent->type != TYPE_DIR)
        return 1;
    
    for (int i = 0; i < NDIRECT; i++) {
        char *current_block;
        dirent_t * curr_dirent;
        uint32_t curr_blk_num;

        if (ip_parent->addr[i] == 0)
            continue;

        curr_blk_num = BLOCK_NUM(ip_parent->addr[i]);

        inmem_block = inmem_read_block(curr_blk_num);
        if (inmem_block == 0)
            return 1;

        current_block = inmem_block->inmem_addr;

        curr_dirent = (dirent_t *)current_block;

        while ((uintptr_t)curr_dirent - (uintptr_t)current_block < BSIZE) {
            if (curr_dirent->strlen == 0) {
                /* Found free dirent, place here */
                curr_dirent->inum = inum_child;
                curr_dirent->strlen = strlen(fname);
                curr_dirent->reclen = sizeof(dirent_t);

                memcpy(curr_dirent->name, fname, strlen(fname));
                curr_dirent->name[strlen(fname)] = 0;

                inmem_block_free(inmem_block, 1);
                return 0;
            }
            curr_dirent++;
        }

        inmem_block_free(inmem_block, 0);
    }

    return 1;
}

inode_t * fs_low_level_inode fs_get_inode(char *path)
{
    char current_name[MAXFNAME];
    inode_t *ip_parent, *ip_child;

    if (*path == '/') {
        ip_parent = fs_get_inode_by_inum(ROOTINO);
        path++;
    } else if (*path == 0) {
        return (inode_t *)0;
    } else {
        ip_parent = current_dir;
    }

    while ((path = fs_strtok_by_sep(path, current_name)) != 0) {
        printf("%s\n", path);
        // ip = fs_find_inode_in_dir(ip, current_name);
    }

    ip_child = fs_find_inode_in_dir(ip_parent, current_name);

    /* 
     * Since ip_parent won't be used anymore, 
     * we need to free inmem inode that was 
     * locked by fs_get_inode_by_inum
     */
    inmem_free_inode(ip_parent);

    return ip_child;
}

inode_t *fs_create_inode(char *path, int type)
{
    char current_name[MAXFNAME];
    int inum_child;
    inode_t *ip_parent, *ip_child;

    if (*path == '/') {
        ip_parent = fs_get_inode_by_inum(ROOTINO);
        path++;
    } else if (*path == 0) {
        return (inode_t *)0;
    } else {
        ip_parent = current_dir;
    }

    /* Allocate inode on storage device first */
    inum_child = fs_inode_alloc();
    if (inum_child < 0)
        return (inode_t *)0;

    /* Create inmemory inode */
    ip_child = inmem_lock_inode();
    if (ip_child == 0) {
        fs_inode_free(inum_child);
        inmem_free_inode(ip_child);
        inmem_free_inode(ip_parent);
        return (inode_t *)0;
    }
    ip_child->size = 0;
    ip_child->type = type;
    ip_child->nlink = 1;

    while ((path = fs_strtok_by_sep(path, current_name)) != 0) {
        printf("%s\n", path);
        // ip = fs_find_inode_in_dir(ip, current_name);
    }

    /* Place inode in parent's dirent */
    if (fs_add_inode_in_dir(ip_parent, current_name, inum_child)) {
        fs_inode_free(inum_child);
        inmem_free_inode(ip_child);
        inmem_free_inode(ip_parent);
        return (inode_t *)0;
    }

    /* Finally place inode store new inode on storage device */
    fs_store_inode_by_inum(ip_child, inum_child);

    /* 
     * Since ip_parent won't be used anymore, 
     * we need to free inmem inode that was 
     * locked by fs_get_inode_by_inum
     */
    inmem_free_inode(ip_parent);

    return ip_child;
}

int fs_trunc_inode(inode_t *ip)
{
    for (int i = 0; i < NDIRECT; i++) {
        if (ip->addr[i] == 0)
            continue;
        fs_block_free(ip->addr[i]);
        ip->addr[i] = 0;
        ip->size = 0;
    }
    return 0;
}

int fs_open(char *path, int flags)
{
    int fd;
    struct file *f;
    inode_t *ip;

    if (opened_files_cnt == MAX_FILES_OPENED) {
        return -1;
    }


    if (flags & FS_O_CREATE) {
        ip = fs_create_inode(path, TYPE_FILE);
        if (ip == 0)
            return -1;
    } else {
        ip = fs_get_inode(path);
        if (ip == 0)
            return -1;

        if (ip->type == TYPE_DIR && flags != FS_O_RDONLY)
            return -1;

        if (flags & FS_O_TRUNC) {
            if (fs_trunc_inode(ip))
                return -1;
        }
    }

    fd = opened_files_cnt++;
    f = &opened_files[fd];
    f->type = FD_INODE;
    f->off = 0;
    f->ip = ip;
    f->ref = 1;
    f->readable = !(flags & FS_O_WRONLY);
    f->writable = (flags & FS_O_WRONLY) || (flags & FS_O_RDWR);

    return fd;
}

int fs_low_level_inode fs_close(int fd)
{
    struct file *f;

    if (fd > MAX_FILES_OPENED)
        return -1;

    f = &opened_files[fd];
    inmem_free_inode(f->ip);
    memset(f, 0, sizeof (struct file));
    opened_files_cnt -= 1;

    return 0;
}

int fs_low_level_storage fs_read_from_inode(inode_t *ip, uint32_t off, 
                            uint32_t size, char * buf)
{
    struct inmem_block *inmem_block;
    int bytes_read = 0;

    if (off > ip->size || off + size < off)
        return 0;
    
    if (off + size > ip->size) {
        size = ip->size - off;
    }

    while (bytes_read < size) {
        char *current_block;
        uint32_t current_block_num = BLOCK_NUM(off);
        uint32_t in_block_off = off % BSIZE;
        uint32_t req_to_read_here = min(size - bytes_read, BSIZE - in_block_off);

        if ((BLOCK_NUM(ip->addr[current_block_num])) == 0) {
            break;
        }

        inmem_block = inmem_read_block(BLOCK_NUM(ip->addr[current_block_num]));
        if (inmem_block == 0)
            return 0;

        current_block = inmem_block->inmem_addr;

        memcpy(buf,(char *)(current_block + in_block_off), req_to_read_here);
        inmem_block_free(inmem_block, 0);

        bytes_read += req_to_read_here;
        off += req_to_read_here;
    }

    return bytes_read;
}

int fs_low_level_storage fs_write_to_inode(inode_t *ip, uint32_t off, 
                            uint32_t size, char * buf)
{
    struct inmem_block *inmem_block;
    int bytes_written = 0;

    if (off > ip->size || off + size < off)
        return -1;
    
    if (off + size > MAX_BLOCKS_PER_FILE * BSIZE) {
        return -1;
    }

    while (bytes_written < size) {
        char *current_block;
        uint32_t current_block_num = BLOCK_NUM(off);
        uint32_t in_block_off = off % BSIZE;
        uint32_t req_to_write_here = min(size - bytes_written, BSIZE - in_block_off);

        if (ip->addr[current_block_num] == 0) {
            break;
        }

        inmem_block = inmem_read_block(BLOCK_NUM(ip->addr[current_block_num]));
        if (inmem_block == 0)
            return 0;

        current_block = inmem_block->inmem_addr;

        memcpy((char *)(current_block + in_block_off), buf, req_to_write_here);
        inmem_block_free(inmem_block, 1);

        bytes_written += req_to_write_here;
        off += req_to_write_here;
    }

    return bytes_written;
}

int fs_read(int fd, void *buf, uint32_t size)
{
    struct file *f;
    int bytes_read = 0;

    if (fd < 0) {
        return -1;
    }

    f = &opened_files[fd];

    if (!f->readable) {
        return -1;
    }
    
    if (f->type == FD_INODE) {
        bytes_read = fs_read_from_inode(f->ip, f->off, 
                                    size, (char *)buf);
        f->off += bytes_read;
    }

    return bytes_read;
}

int fs_write(int fd, void *buf, uint32_t size)
{
    struct file *f;
    int bytes_written = 0;

    if (fd < 0) {
        return -1;
    }

    f = &opened_files[fd];

    if (!f->writable)
        return -1;

    if (f->type == FD_INODE) {
        bytes_written = fs_write_to_inode(f->ip, f->off, 
                                    size, (char *)buf);
        f->off += bytes_written;
    }

    return bytes_written;
}


/* ======================================================================== */
/* Filesystem initialization                                                */
/* ======================================================================== */

int fs_init()
{
    sb = (superblock_t *)fs_base;
    if (sb->magic != FS_MAGIC)
        return 1;
    return 0;

    /* init inmemory inodes and blocks */
    for (int i = 0; i < INMEM_BLOCKS_NUM; i++) {
        inmem_blocks[i].state = UNUSED;
    }
    for (int i = 0; i < INMEM_INODES_NUM; i++) {
        inmem_inodes[i].state = UNUSED;
    }
}


/* ======================================================================== */
/* Filesystem mount                                                         */
/* ======================================================================== */

void __attribute__((constructor)) mount_fs()
{    
    fs_fd = open(TARGET_IMAGE_NAME, O_RDWR, S_IRWXU);
    if (fs_fd < 0)
        goto open_failed;

    if ((fs_base = (uintptr_t)mmap(NULL, FS_TOTAL_SIZE * FS_BLOCK_SIZE,
         PROT_READ | PROT_WRITE, MAP_SHARED, fs_fd, 0)) == (uintptr_t)MAP_FAILED) {
        goto mmap_failed;
    }

    return;

open_failed:
    err("open failed\n");
    exit(1);

mmap_failed:
    close(fs_fd);
    err("mmap failed\n");
}

void __attribute__((destructor)) umount_fs()
{
    munmap((void *)fs_base, FS_TOTAL_SIZE * FS_BLOCK_SIZE);
    close(fs_fd);
    exit(1);
}
