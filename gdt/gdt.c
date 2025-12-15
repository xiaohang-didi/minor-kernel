#include "gdt.h"
#include "string.h"

//定义全局描述符长度
#define GDT_LENGTH 128

//定义全局任务状态段
tss_t tss;

gdt_entry_t gdt_entries[GDT_LENGTH];

gdt_ptr_t gdt_ptr;

static void gdt_set_gate(int32_t num,uint32_t base,uint32_t limit,uint8_t access,uint8_t gran);

extern uint32_t stack;

//初始去全局描述符表
void init_gdt(){
    gdt_ptr.limit = sizeof(gdt_entry_t)*GDT_LENGTH-1;
    gdt_ptr.base = (uint32_t)gdt_entries;

    //采用 Intel 平坦模型 我们知道平坦模型的基地址都是0，段线长为整个地址空间的长度，因此只需要偏移地址即可得到对应的物理地址
    gdt_set_gate(0, 0, 0, 0, 0); // 按照 Intel 文档要求，第一个描述符必须全 0
    gdt_set_gate(KERNEL_CODE_IDX, 0, 0xFFFFFFFF, 0x9A, 0xCF); // 指令段
    gdt_set_gate(KERNEL_DATA_IDX, 0, 0xFFFFFFFF, 0x92, 0xCF); // 数据段
    gdt_set_gate(USER_CODE_IDX, 0, 0xFFFFFFFF, 0xFA, 0xCF); // 用户模式代码段
    gdt_set_gate(USER_DATA_IDX, 0, 0xFFFFFFFF, 0xF2, 0xCF); // 用户模式数据段

    //加载GDT到GDTR
    gdt_flush((uint32_t)&gdt_ptr);
}

//设置tss全局描述符同时加载段选择子到 任务状态寄存器
void tss_init(){
    memset(&tss, 0 ,sizeof(tss));

    tss.ss0 = KERNEL_DATA_SELECTOR;
    tss.iobase = sizeof(tss);//不限制内核任务的io操作

    gdt_set_gate(3, (uint32_t)&(tss), sizeof(tss)-1, 0x89, 0x00);

    uint16_t sel = KERNEL_TSS_SELECTOR;
    asm volatile("ltr %0\n" ::"r"(sel));
}

//全局描述符表构造函数，根据下标构造
//参数分别是数组下标、基地址，限长，访问标志，其他访问标志
static void gdt_set_gate(int32_t num,uint32_t base,uint32_t limit,uint8_t access,uint8_t gran){
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    
    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access = access;
}