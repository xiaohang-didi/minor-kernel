#include "common.h"
#include "console.h"
#include "string.h"
#include "debug.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"
#include "task.h"
#include "sched.h"


//内核初始化函数
void kern_init();

//开启分页机制之后的 Multiboot 数据结构指针
multiboot_t *glb_mboot_ptr;

//开启分页机制之后的内核栈
char kern_stack[STACK_SIZE];

//pgd_tmp用于存放在cr3记录页目录起始地址
//定义了 pte_low  pte_high 两个页表
__attribute__((section(".init.data"))) pgd_t *pgd_tmp = (pgd_t *)0x1000;
__attribute__((section(".init.data"))) pgd_t *pte_low = (pgd_t *)0x2000;
__attribute__((section(".init.data"))) pgd_t *pte_high = (pgd_t *)0x3000;

//内核入口函数
__attribute__((section(".init.text"))) void kern_entry(){
	//记录两个页表在页目录的存放位置
	pgd_tmp[0] = (uint32_t) pte_low | PAGE_PRESENT | PAGE_WRITE;
	pgd_tmp[PGD_INDEX(PAGE_OFFSET)] = (uint32_t)pte_high | PAGE_PRESENT | PAGE_WRITE;
	
	//映射内核虚拟地址 4MB 到物理地址的前 4MB
	int i;
	//这个地方执行 i<<12 是因为一个页帧我们规定为4KB
	for(i = 0; i < 1024; i++){
		pte_low[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
	}

	//映射  x00000000−0x00400000 的物理地址到虚拟地址 0xC0000000−0xC0400000
	for(i = 0; i < 1024; i++){
		pte_high[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
	}

	//默认AT&T汇编格式
	//设置临时页表
	//"+r"可输入可输出约束，“r” 只读输入约束
	asm volatile ("mov %0, %%cr3" : : "r" (pgd_tmp));

	uint32_t cr0;

	//启用分页，将cr0寄存器的分页位置为 1 就好
	asm volatile("mov %%cr0,%0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0,%%cr0" : : "r"(cr0));

	//切换内核栈
	uint32_t kern_stack_top = ((uint32_t)kern_stack+STACK_SIZE) & 0xFFFFFFF0;
	asm volatile ("mov %0, %%esp\n\t" "xor %%ebp, %%ebp" : : "r" (kern_stack_top));
	//更新全局 multiboot_t 指针
	glb_mboot_ptr = mboot_ptr_tmp + PAGE_OFFSET;
	//调用内核初始化函数
	kern_init();
}

int flag = 0;

int thread(void *arg)
{
	while (1) {
		if (flag == 1) {
			printk_color(rc_black, rc_green, "B");
			flag = 0;
		}
	}

	return 0;
}

void kern_init()
{
	init_debug();
	init_gdt();
	init_idt();

	console_clear();
	printk_color(rc_black, rc_green, "Hello, OS kernel!\n");

	init_timer(200);
	// 开启中断
	// asm volatile ("sti");

	printk("kernel in memory start: 0x%08X\n", kern_start);
	printk("kernel in memory end:   0x%08X\n", kern_end);
	printk("kernel in memory used:   %d KB\n\n", (kern_end - kern_start) / 1024);
	
	show_memory_map();
	init_pmm();

	printk_color(rc_black, rc_red, "\nThe Count of Physical Memory Page is: %u\n\n", phy_page_count);

	uint32_t allc_addr = NULL;
	printk_color(rc_black, rc_light_brown, "Test Physical Memory Alloc :\n");
	allc_addr = pmm_alloc_page();
	printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", allc_addr);
	allc_addr = pmm_alloc_page();
	printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", allc_addr);
	allc_addr = pmm_alloc_page();
	printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", allc_addr);
	allc_addr = pmm_alloc_page();
	printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", allc_addr);

	while ((1))
	{
		asm volatile("hlt");
	}
}

