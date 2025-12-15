#ifndef INCLUDE_TASK_H_
#define INCLUDE_TASK_H_

#include "types.h"
#include "pmm.h"
#include "vmm.h"
#include "bitmap.h"

//全局pid值
extern pid_t now_pid;

//进程名字长度
#define TASK_NAME_LEN 16

//进程文件数量
#define TASK_FILE_NR 16

// 进程状态描述
typedef
enum task_state {
	TASK_UNINIT = 0, 	// 未初始化
	TASK_SLEEPING = 1, 	// 睡眠中
	TASK_RUNNABLE = 2, 	// 可运行(也许正在运行)
	TASK_ZOMBIE = 3, 	// 僵尸状态
} task_state;

// 内核线程的上下文切换保存的信息
typedef
struct context {
	uint32_t esp;
	uint32_t ebp;
	uint32_t ebx;
	uint32_t esi;
	uint32_t edi;
	uint32_t eflags;
};

// 进程内存地址结构
typedef
struct mm_struct {
	pgd_t *pgd_dir; 	// 进程页表
} mm_struct;

// 进程控制块 PCB 
typedef
struct task_struct {
	volatile task_state state; 	        // 进程当前状态
	uint32_t priority;                  // 任务优先级
    int ticks;                          // 剩余时间片
    uint32_t jiffies;                   // 上次执行时全局时间片
    char name[TASK_NAME_LEN];           // 任务名
    uint32_t uid;                       // 用户 id
    uint32_t gid;                       // 用户组 id
	pid_t 	 pid; 			            // 进程标识符
	pid_t ppid;                         // 父任务 id
    pid_t pgid;                         // 进程组
    pid_t sid;                          // 进程会话
    dev_t tty;                          // tty 设备
    uint32_t pde;                       //页目录物理地址
	void *stack; 		                // 进程的内核栈地址
	mm_struct *mm; 		                // 当前进程的内存地址映像
	struct context context; 	        // 进程切换需要的上下文信息
	struct task_struct *next;       	// 链表指针
    struct bitmap_t *vmap;              // 进程虚拟内存位图
    uint32_t text;                      // 代码段地址
    uint32_t data;                      // 数据段地址
    uint32_t end;                       // 程序结束地址
    uint32_t brk;                       // 进程堆内存最高地址
    int status;                         // 进程特殊状态
    pid_t waitpid;                      // 进程等待的 pid
    char *pwd;                          // 进程当前目录
    struct inode_t *ipwd;               // 进程当前目录 inode program work directory
    struct inode_t *iroot;              // 进程根目录 inode
    struct inode_t *iexec;              // 程序文件 inode
    uint16_t umask;                     // 进程用户权限
    struct file_t *files[TASK_FILE_NR]; // 进程文件表
    uint32_t signal;                    // 进程信号位图
    uint32_t blocked;                   // 进程信号屏蔽位图
    struct timer_t *alarm;              // 闹钟定时器
    struct timer_t *timer;              // 超时定时器
    struct fpu_t *fpu;                  // fpu 指针
    uint32_t flags;                     // 特殊标记
    uint32_t magic;                     // 内核魔数，用于检测栈溢出
} task_struct;

// 中断帧
typedef struct intr_frame_t
{
    uint32_t vector;

    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    // 虽然 pushad 把 esp 也压入，但 esp 是不断变化的，所以会被 popad 忽略
    uint32_t esp_dummy;

    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    uint32_t vector0;

    uint32_t error;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} intr_frame_t;

// 内核线程创建
int32_t kernel_thread(int (*fn)(void *), void *arg);

// 线程退出函数
void kthread_exit();

void task_to_user_mode(uint32_t target);

static void real_init_thread();

void init_thread();

#endif