/* kernel/driver/ide.c */

#include <Tuix/ide.h>
#include <Tuix/panic.h>
#include <Tuix/serial.h>
#include <Tuix/spinlock.h>
#include <arch/x86.h>
#include <Tuix/pic.h>
#include <Tuix/fs.h>
#include <Tuix/buf.h>
#include <Tuix/process.h>

#define SECTOR_SIZE   512
#define IDE_BSY       0x80
#define IDE_DRDY      0x40
#define IDE_DF        0x20
#define IDE_ERR       0x01

#define IDE_CMD_READ  0x20
#define IDE_CMD_WRITE 0x30
#define IDE_CMD_RDMUL 0xc4
#define IDE_CMD_WRMUL 0xc5

static struct spinlock ide_lock;
static int have_disk1;
static struct buf* ide_queue;
static int ide_wait(int errno);

void init_ide(void)
{
    int i;
    init_lock(&ide_lock, "ide");
    // 启用14号中断
    enable_irq(IRQ_ATA1); // 启用键盘
    ide_wait(0);
    // 检查第二磁盘是否存在
    outb(0x1f6, 0xe0 | (1 << 4));
    for(i = 0; i < 1000; i++)
    {
        if(inb(0x1f7) != 0)
        {
            have_disk1 = 1;
            break;
        }
    }

    if(have_disk1) serial_printf("IDE init done!\n");
    else PANIC("IDE failed! no fs.img in qemu");

    // 切换回原磁盘
    outb(0x1f6, 0xe0 | (0 << 4));
}

static int ide_wait(int errno)
{
    int r;

    while(((r = inb(0x1f7)) & (IDE_BSY|IDE_DRDY)) != IDE_DRDY);

    if(errno && (r & (IDE_DF | IDE_ERR)) != 0)
        return -1;

    return 0;
}

// 开始 b 的请求。调用者必须持有 idelock。
static void ide_start(struct buf* b)
{
    if(b == 0)
        PANIC("ide_start");
    if(b->blockno >= FS_SIZE)
        PANIC("incorrect blockno");
    int sector_per_block =  B_SIZE / SECTOR_SIZE;
    int sector = b->blockno * sector_per_block;
    int read_cmd = (sector_per_block == 1) ? IDE_CMD_READ :  IDE_CMD_RDMUL;
    int write_cmd = (sector_per_block == 1) ? IDE_CMD_WRITE : IDE_CMD_WRMUL;

    if (sector_per_block > 7)
        PANIC("idestart");
    ide_wait(0);
    outb(0x3f6, 0); // 启用ide中断
    outb(0x1f2, sector_per_block);  // 扇区号
    outb(0x1f3, sector & 0xff);
    outb(0x1f4, (sector >> 8) & 0xff);
    outb(0x1f5, (sector >> 16) & 0xff);
    outb(0x1f6, 0xe0 | ((b->dev & 1) << 4) | ((sector >> 24) & 0x0f));
    // 非读即写
    if(b->flags & B_DIRTY)
    {
        outb(0x1f7, write_cmd);
        outsl(0x1f0, b->data, B_SIZE / 4);
    }
    else
        outb(0x1f7, read_cmd);
}

void ide_handler(void)
{
    struct buf *b;

    // 第一个排队的缓冲区是当前的请求。
    acquire(&ide_lock);

    if((b = ide_queue) == 0)
    {
        release(&ide_lock);
        return;
    }
    ide_queue = b->qnext;

    // 如果要读...
    if(!(b->flags & B_DIRTY) && ide_wait(1) >= 0)
        insl(0x1f0, b->data, B_SIZE / 4);

    // 叫醒等待这个buf的进程
    b->flags |= B_VALID;
    b->flags &= ~B_DIRTY;
    wakeup(b);

    // 在队列下一个buffer中启动磁盘
    if(ide_queue != 0)
        ide_start(ide_queue);

    release(&ide_lock);
}
