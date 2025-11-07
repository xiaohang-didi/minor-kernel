#ifndef INCLUDE_MY_SCHED_H_
#define INCLUDE_MY_SCHED_H_

#include"task.h"

//可调度进程链表
extern task_struct *running_proc_head;

// 等待进程链表
extern task_struct *wait_proc_head;

// 当前运行的任务
extern task_struct *current;

//定义内核栈顶
extern uint32_t kern_stack_top;

// 初始化任务调度
void init_sched();

// 任务调度
void schedule();

// 任务切换准备
void change_task_to(task_struct *next);

// 任务切换
void switch_to(struct context *prev, struct context *next);

#endif