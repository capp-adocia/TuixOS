/* include/Tuix/vm.h */

#ifndef I_T_VM_H
#define I_T_VM_H

#include <stddef.h>
#include <Tuix/process.h>

struct kmap {
    void *virt;
    uint32_t phys_start;
    uint32_t phys_end;
    int perm;
};

extern pde_t* kpgdir;

/**
 * 切换页目录基址cr3
 * @parma p_addr 页目录实地址
 */
static inline void lcr3(const uint32_t p_addr)
{
    asm volatile("movl %0,%%cr3" : : "r" (p_addr));
}

/**
 * 遍历页目录,根据虚拟地址找到页目录项地址
 * @parma pgdir 页目录地址
 * @parma va 虚拟地址
 * @param alloc=1 写操作,alloc=0 查操作
 * @return 虚拟地址对应的页表项地址
 */
pte_t* walk_pgdir(pde_t* pgdir, const void* va, int alloc);

/**
 * 初始化内核页表,为后续完整映射整个内存做准备
 */
void init_kvm(void);

/**
 * 初始化内存映射
 */
void init_kmappings(void);

/**
 * 创建内核页表，设置页目录项+页表项
 * @parma 返回页目录地址
 */
pde_t* setup_kvm(void);

/**
 * 分配用户进程页目录，将初始化代码拷贝到用户物理内存空间
 * @parma pgdir 用户进程的页目录基址
 * @parma init 初始化代码的地址
 * @parma size 代码的字节数大小
 */
void init_uvm(pde_t *pgdir, char *init, uint32_t size);

/**
 * 切换TSS和进程p的页目录寄存器
 * @param p 切换到的目标进程
 */
void switch_uvm(struct proc* p);

/**
 * 切换到内核页目录基址
 */
void switch_kvm(void);

#endif
