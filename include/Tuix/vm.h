/* include/Tuix/vm.h */

#ifndef I_T_VM_H
#define I_T_VM_H

#include <stddef.h>
struct kmap {
    void *virt;
    uint32_t phys_start;
    uint32_t phys_end;
    int perm;
};

typedef int pde_t;
typedef int pte_t;
extern pde_t* kpgdir;

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
 * 创建页表，设置页目录项+页表项
 * @parma 返回页目录地址
 */
pde_t* setup_kvm(void);

#endif
