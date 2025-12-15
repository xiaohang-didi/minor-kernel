#include "gdt.h"
#include "idt.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"
#include "task.h"
#include "my_sched.h"
#include "string.h"
#include "debug.h"

//全局pid值
pid_t now_pid = 0;

//内核线程的创建
int32_t kernel_thread(int (*fn)(void *),void *arg){
    //这个地方分配了一个栈，同时将这块地址解释为任务结构块，意味着这个栈的低地址存放这个任务块内容本身
    task_struct *new_task = (task_struct *)kmalloc(STACK_SIZE);
    assert(new_task != NULL, "kern_thread:kmalloc error");

    //将栈低端结构信息初始化为 0，实际上的内存分配只是“不再需要”，而不是不存在，通常我们会使用覆盖来处理垃圾信息，但是这里没有进行初始化，为了避免不必要的干扰初始化为0
    bzero(new_task, sizeof(task_struct));

    new_task->state = TASK_RUNNABLE;
    
    //内核创建线程共享内核资源，这是简单内核的权宜之计
    new_task->stack = current;
    new_task->pid = now_pid++;
    new_task->mm = NULL;

    uint32_t *stack_top = (uint32_t *)((uint32_t)new_task + STACK_SIZE);

    *(--stack_top) = (uint32_t)arg;
    *(--stack_top) = (uint32_t)kthread_exit;
    *(--stack_top) = (uint32_t)fn;

    new_task->context.esp = (uint32_t)new_task + STACK_SIZE - sizeof(uint32_t)*3;

    //设置新任务的标志寄存器未屏蔽中断
    new_task->context.eflags = 0x200;

    //接下来插入新节点到队列末尾，同时让尾节点指向头节点构成单向循环链表
    new_task->next = running_proc_head;
    
    //找到当前任务队列，插入末尾
    task_struct *tail = running_proc_head;
    assert(tail != NULL,"Must init sched!");

    while(tail->next != running_proc_head){
        tail = tail->next;
    }
    //原本的尾节点指向新节点
    tail->next = new_task;

    return new_task->pid;
}

//线程退出
void kthread_exit(){
    //关中断，防止在修改链表时被打断
    asm volatile("cli");

    current->state = TASK_ZOMBIE;
    
    //如果当前运行进程是队列头，更新队列头
    if(running_proc_head == current){
        //当前进程是队列头同时没有下一个进程块
        if(current->next == current){
            running_proc_head = NULL;
        }else{
            //当前进程是队列头同时还有下一个进程块
            running_proc_head = current->next;
        }
    }
    
    //找到当前线程的前一个结点，修改链表指针
    task_struct *prev = running_proc_head;
    if(prev != NULL){
        while(prev->next != current){
            prev = prev->next;
        }
        prev->next = current->next;
    }

    //释放栈内存
    current->stack = NULL;

    asm volatile("sti");

    schedule();
    panic("kthread_exit have error");
}

//获取当前栈的栈底位置
task_struct *running_task()
{

    task_struct *tsk;
    asm volatile(
        "movl %%esp, %%eax\n"
        "andl $0xfffff000, %%eax\n"
        :"=a"(tsk):
    );
    return tsk;
}

void task_to_user_mode(uint32_t target){
    task_struct *task = running_task();

    //创建用户进程虚拟内存位图
    // task->vmap = kmalloc(sizeof(bitmap_t));
    // void *buf = (void *)pmm_alloc_page();
    // bitmap_init(task->vmap, buf, USER_MMAP_SIZE / PAGE_SIZE / 8, USER_MMAP_ADDR / PAGE_SIZE);

    // //创建用户进程页表
    // task->pde = (uint32_t)copy_pde();
    // asm volatile("movl %%eax %%cr3\n" :: "a"(task->pde));

    //找到用户栈栈顶
    uint32_t addr = (uint32_t)task + PAGE_SIZE;

    //分配中断帧大小
    addr -= sizeof(intr_frame_t);
    intr_frame_t *iframe = (intr_frame_t *)addr;

    //对中断帧进行初始化
    iframe->vector = 0x20;

    iframe->edi = 1;
    iframe->esi = 2;
    iframe->ebp = 3;
    iframe->esp_dummy = 4;
    
    iframe->ebx = 5;
    iframe->edx = 6;
    iframe->ecx = 7;
    iframe->eax = 8;

    iframe->gs = 0;
    iframe->fs = USER_DATA_SELECTOR;
    iframe->es = USER_DATA_SELECTOR;
    iframe->ds = USER_DATA_SELECTOR;

    iframe->vector0 = 0;

    iframe->error = 0;

    iframe->eip = (uint32_t)target;
    iframe->cs = USER_CODE_SELECTOR;
    iframe->eflags = (0 << 12 | 0b10 | 1 << 9);
    uint32_t stack = pmm_alloc_page();
    iframe->esp = PAGE_SIZE + stack;
    iframe->ss = USER_DATA_SELECTOR;

    //m限制符表示将iframe的地址作为参数传入
    asm volatile(
        "movl %0, %%esp\n"
        "jmp interrupt_exit\n" :: "m"(iframe));
}


void task_yield()
{
    schedule();
}

void task_sleep(uint32_t ms)
{
    panic("syscall not implement !");
    // assert(!get_interrupt_state(),"Don't interrupt"); // 不可中断

    // task_struct *task = running_task();

    // task_block(task, &sleep_list, TASK_SLEEPING, ms);
}