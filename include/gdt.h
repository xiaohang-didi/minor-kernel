#ifndef INCLUDE_GDT_H_
#define INCLUDE_GDT_H_
#include "types.h"

#define KERNEL_CODE_IDX 1
#define KERNEL_DATA_IDX 2
#define KERNEL_TSS_IDX 3

#define USER_CODE_IDX 4
#define USER_DATA_IDX 5

#define KERNEL_CODE_SELECTOR (KERNEL_CODE_IDX << 3)
#define KERNEL_DATA_SELECTOR (KERNEL_DATA_IDX << 3)
#define KERNEL_TSS_SELECTOR (KERNEL_TSS_IDX << 3)

#define USER_CODE_SELECTOR (USER_CODE_IDX << 3 | 0b11)
#define USER_DATA_SELECTOR (USER_DATA_IDX << 3 | 0b11)

//任务状态段
typedef struct tss{
    uint32_t backlink; // 前一个任务的链接，保存了前一个任状态段的段选择子
    uint32_t esp0;     // ring0 的栈顶地址
    uint32_t ss0;      // ring0 的栈段选择子
    uint32_t esp1;     // ring1 的栈顶地址
    uint32_t ss1;      // ring1 的栈段选择子
    uint32_t esp2;     // ring2 的栈顶地址
    uint32_t ss2;      // ring2 的栈段选择子
    uint32_t cr3;
    uint32_t eip;
    uint32_t flags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldtr;          // 局部描述符选择子
    uint16_t trace : 1;     // 如果置位，任务切换时将引发一个调试异常
    uint16_t reversed : 15; // 保留不用
    uint16_t iobase;        // I/O 位图基地址，16 位从 TSS 到 IO 权限位图的偏移
    uint32_t ssp;           // 任务影子栈指针
} __attribute__((packed)) tss_t;

typedef
struct gdt_entry_t
{
    uint16_t limit_low;//段界限 15~0
    uint16_t base_low;//段基地址15~0
    uint8_t base_middle;//段基地址23~16
    uint8_t access;//段存在位，描述符特权级，描述符子类别
    uint8_t granularity;//其他标志位，段界限 19~16
    uint8_t base_high;//段基地址31~24
} __attribute__((packed)) gdt_entry_t;

typedef
struct gdt_ptr_t
{
    uint16_t limit;//segment limit
    uint32_t base;//segment base
} __attribute__((packed)) gdt_ptr_t;

//initialize GDT
void init_gdt();

//load GDT to GDTR
extern void gdt_flush(uint32_t);

#endif