#include"idt.h"
#include"task.h"
#include"debug.h"
#include"debug.h"
#include"console.h"
#include"stdio.h"
#include"syscall.h"


//总共有64个系统调用
#define SYSCALL_SIZE 64

//为我们的系统调用注册系统调用表
interrupt_handler_t syscall_table[SYSCALL_SIZE];

void syscall_check(uint32_t nr){
    if(nr > SYSCALL_SIZE){
        panic("tribute an error syscall nr !");
    }
}

void sys_default(){
    panic("syscall not implement !");
}

sys_test(){
    panic("syscall not implement !");
}

void sys_write(fd_t fd, char *buf, uint32_t len){
    if(fd == stdout || fd == stderr){
        console_write(buf);
    }else{
        panic("write!!");
    }
}

//对未实现的系统调用注册提示未实现调用字样的打印符
void syscall_init(){
    for(uint32_t i = 0;i < SYSCALL_SIZE;i++){
        syscall_table[i] = sys_default;
    }

    syscall_table[SYS_NR_TEST] = sys_test;
    syscall_table[SYS_NR_SLEEP] = task_sleep();
    syscall_table[SYS_NR_YIELD] = task_yield();
    syscall_table[SYS_NR_WRITE] = sys_write;

}

static void real_init_thread(){
    sleep(100);
	while(true){
        printf("user space is successful!\n");
    }
}

void init_thread(){
	char tmp[100];
	task_to_user_mode(real_init_thread);
}