#include "gdt.h"
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
    
    //内核创建进程的时候会为新进程重新申请一个进程控制块（PCB），并使用父进程的PCB进行初始化，因此这里指向父进程的栈
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