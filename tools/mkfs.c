/* tools/mkfs.c */
// 用于格式化磁盘

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#include "stat.h"
#include "../include/Tuix/fs.h"

#define INODES_NUM 200
#define min(a, b) ((a) < (b) ? (a) : (b))

int bitmap_num = FS_SIZE / (BPB) + 1;
int inode_blocks_num = INODES_NUM / IPB + 1;
int meta_num;    // Number of meta blocks (boot, sb, nlog, inode, bitmap)
int blocks_num;  // Number of data blocks

int fsfd;
struct superblock sb;
char zeroes[B_SIZE]; // 用单块大小的数组来清空文件
uint32_t freeinode = 1;
uint32_t freeblock;

// 转为小端序
uint32_t xshort(uint16_t x)
{
    uint32_t y;
    uint8_t *a = (uint8_t*)&y;
    a[0] = x;
    a[1] = x >> 8;
    return y;
}

uint32_t xint(uint32_t x)
{
    uint32_t y;
    uint8_t *a = (uint8_t*)&y;
    a[0] = x;
    a[1] = x >> 8;
    a[2] = x >> 16;
    a[3] = x >> 24;
    return y;
}

// 写数据进对应的块位置
void wsect(uint32_t sec, void *buf)
{
    if(lseek(fsfd, sec * B_SIZE, SEEK_SET) != sec * B_SIZE)
    {
        perror("lseek");
        return;
    }
    if(write(fsfd, buf, B_SIZE) != B_SIZE)
    {
        perror("write");
        return;
    }
}
// 从某一个块中读取数据到buf中
void rsect(uint32_t sec, void *buf)
{
    if(lseek(fsfd, sec * B_SIZE, SEEK_SET) != sec * B_SIZE)
    {
        perror("lseek");
        return;
    }
    if(read(fsfd, buf, B_SIZE) != B_SIZE)
    {
        perror("read");
        return;
    }
}
// 读某inode
void rinode(uint32_t inum, struct dinode *ip)
{
    char buf[B_SIZE];
    uint32_t bn;
    struct dinode *dip;

    bn = IBLOCK(inum, sb);
    rsect(bn, buf);
    dip = ((struct dinode*)buf) + (inum % IPB);
    *ip = *dip;
}

// 写入inode
void winode(uint32_t inum, struct dinode *ip)
{
    char buf[B_SIZE];
    uint32_t bn;
    struct dinode *dip;

    bn = IBLOCK(inum, sb);
    rsect(bn, buf);
    // 从buf中找到对应的inode指针的位置
    dip = ((struct dinode*)buf) + (inum % IPB);
    *dip = *ip;
    wsect(bn, buf);
}
// 分配inode,这个类型是目录或文件或设备类型
uint32_t ialloc(uint16_t type)
{
    uint32_t inum = freeinode++;
    struct dinode din;

    memset(&din, 0, sizeof(din));
    din.type = xshort(type);
    din.nlink = xshort(1);
    din.size = xint(0);
    winode(inum, &din);
    return inum;
}
// 初始化块位图 used标记前used个已使用
void balloc(int used)
{
    uint8_t buf[B_SIZE];
    int i;

    printf("balloc: first %d blocks have been allocated\n", used);
    assert(used < B_SIZE * 8);
    memset(buf, 0, B_SIZE);
    for(i = 0; i < used; i++)
        buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));

    printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
    wsect(sb.bmapstart, buf);
}

// 既可以接受目录文件也可以接受文件数据
void iappend(uint32_t inum, void *xp, int n)
{
    char *p = (char*)xp;
    uint32_t fbn, off, n1;
    struct dinode din;
    char buf[B_SIZE];
    uint32_t indirect[INDIRECT_NUM]; // 这个就是指的是一级索引表
    uint32_t x;

    rinode(inum, &din);
    off = xint(din.size);
    printf("在偏移 %d 处追加 inode %d，大小 %d\n", off, inum, n);
    while(n > 0)
    {
        fbn = off / B_SIZE;
        assert(fbn < MAX_FILE); // 12 + 128个文件块
        if(fbn < DIRECT_NUM) // 0 -> 11 为直接
        {
            if(xint(din.addrs[fbn]) == 0) // 如果之前没有分配,则开始分配一个块
                din.addrs[fbn] = xint(freeblock++);

            x = xint(din.addrs[fbn]); // 取出块号
        }
        else
        {
            // 先读出来再修改,最后写回数据块
            if(xint(din.addrs[DIRECT_NUM]) == 0)
                din.addrs[DIRECT_NUM] = xint(freeblock++);

            rsect(xint(din.addrs[DIRECT_NUM]), (char*)indirect);
            if(indirect[fbn - DIRECT_NUM] == 0) // 跳过前12个块
            {
                indirect[fbn - DIRECT_NUM] = xint(freeblock++);
                wsect(xint(din.addrs[DIRECT_NUM]), (char*)indirect);
            }
            x = xint(indirect[fbn - DIRECT_NUM]);
        }
        // (fbn + 1) * B_SIZE - off : 剩余可写量
        n1 = min(n, (fbn + 1) * B_SIZE - off); // (fbn+1)*B_SIZE 是下一个块的偏移 - 当前偏移off
        rsect(x, buf);
        memmove(buf + off % B_SIZE, p, n1);
        wsect(x, buf);
        n -= n1;
        off += n1;
        p += n1;
    }
    din.size = xint(off);
    winode(inum, &din);
}

int main(int argc, char** argv)
{
    int i, cc, fd;
    uint32_t root_ino, inum, off;
    struct dirent de;
    char buf[B_SIZE];
    struct dinode din;

    static_assert(sizeof(int) == 4, "Integers must be 4 bytes!");

    if(argc < 2)
    {
        fprintf(stderr, "Usage: mkfs fs.img files...\n");
        return 1;
    }

    assert((B_SIZE % sizeof(struct dinode)) == 0);
    assert((B_SIZE % sizeof(struct dirent)) == 0);

    fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);

    if(fsfd < 0)
    {
        perror(argv[1]);
        return 1;
    }
    meta_num = 2 + LOG_SIZE + inode_blocks_num + bitmap_num; // 计算出元数据的块数
    blocks_num = FS_SIZE - meta_num; // 计算出数据块的块数

    sb.size = xint(FS_SIZE);
    sb.nblocks = xint(blocks_num);
    sb.ninodes = xint(INODES_NUM);
    sb.nlog = xint(LOG_SIZE);
    sb.logstart = xint(2);
    sb.inodestart = xint(2 + LOG_SIZE);
    sb.bmapstart = xint(2 + LOG_SIZE + inode_blocks_num);

    printf("(boot:1, super:1, log_blocks:%u, inode_blocks:%u, bitmap_blocks:%u)\nmeta_num:%d \ndata_blocks:%d\ntotal:%d\n",
        LOG_SIZE, inode_blocks_num, bitmap_num, meta_num, blocks_num, FS_SIZE);
 
    // 从freeblock开始就是数据块了
    freeblock = meta_num;

    // 遍历每一个磁盘块,清空所有块内容
    for(i = 0; i < FS_SIZE; i++)
        wsect(i, zeroes);

    // 这里写入超级块,先写入内存再写入磁盘,memmove效率比较低要换成memcpy
    memcpy(buf, &sb, sizeof(sb));
    memset(buf + sizeof(sb), 0, B_SIZE - sizeof(sb));
    wsect(1, buf);
 
    root_ino = ialloc(T_DIR);
    assert(root_ino == ROOT_INO);

    // 把目录'.'加入inode块中
    memset(&de, 0, sizeof(de));
    de.inum = xshort(root_ino); // rootino = 1
    strcpy(de.name, ".");
    iappend(root_ino, &de, sizeof(de));

    // 把目录'..'加入inode块中
    memset(&de, 0, sizeof(de));
    de.inum = xshort(root_ino);
    strcpy(de.name, "..");
    iappend(root_ino, &de, sizeof(de));

    for(i = 2; i < argc; i++)
    {
        char *filename = argv[i];
        assert(strchr(filename, '/') == NULL);

        if((fd = open(filename, 0)) < 0)
        {
            perror(filename);
            return 1;
        }

        inum = ialloc(T_FILE);

        memset(&de, 0, sizeof(de));
        de.inum = xshort(inum);
        strncpy(de.name, filename, DIR_SIZE);
        iappend(root_ino, &de, sizeof(de)); // 目录

        while((cc = read(fd, buf, sizeof(buf))) > 0)
            iappend(inum, buf, cc); // 数据

        close(fd);
    }
    // 更新根目录大小,尽管大小可能不足一个块,但需要上对齐到一个块
    rinode(root_ino, &din);
    off = xint(din.size);
    off = ((off / B_SIZE) + 1) * B_SIZE;
    din.size = xint(off);
    winode(root_ino, &din);
    // 更新位图块
    balloc(freeblock);

    close(fsfd);
    return 0;
}
