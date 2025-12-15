#ifndef SYSCALL_H_
#define SYSCALL_H_

#include"types.h"

typedef enum syscall_t{
    SYS_NR_TEST,
    SYS_NR_WRITE,
    SYS_NR_SLEEP,
    SYS_NR_YIELD,
} syscall_t;

uint32_t test();
void yield();
void sleep(uint32_t ms);

uint32_t write(fd_t fd,char *buf,uint32_t len);

void sleep(uint32_t ms);

#endif