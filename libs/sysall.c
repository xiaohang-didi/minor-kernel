#include"syscall.h"
#include"types.h"

//传递系统调用号
static inline uint32_t _syscall0(uint32_t nr)
{
    uint32_t ret;
    asm volatile(
        "int $0x80\n"
        : "=a"(ret)
        : "a"(nr));
    return ret;
}

//需要一个参数的系统调用
static inline uint32_t _syscall1(uint32_t nr, uint32_t arg)
{
    uint32_t ret;
    asm volatile(
        "int $0x80\n"
        : "=a"(ret)
        : "a"(nr), "b"(arg));
    return ret;
}

//需要两个参数的系统调用
static inline uint32_t _syscall2(uint32_t nr, uint32_t arg1, uint32_t arg2)
{
    uint32_t ret;
    asm volatile(
        "int $0x80\n"
        : "=a"(ret)
        : "a"(nr), "b"(arg1), "c"(arg2));
    return ret;
}

//需要三个参数的系统调用
static inline uint32_t _syscall3(uint32_t nr, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
    uint32_t ret;
    asm volatile(
        "int $0x80\n"
        : "=a"(ret)
        : "a"(nr), "b"(arg1), "c"(arg2), "d"(arg3));
    return ret;
}

//需要四个参数的系统调用
static inline uint32_t _syscall4(uint32_t nr, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
    uint32_t ret;
    asm volatile(
        "int $0x80\n"
        : "=a"(ret)
        : "a"(nr), "b"(arg1), "c"(arg2), "d"(arg3), "S"(arg4));
    return ret;
}

//需要五个参数的系统调用
static inline uint32_t _syscall5(uint32_t nr, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{
    uint32_t ret;
    asm volatile(
        "int $0x80\n"
        : "=a"(ret)
        : "a"(nr), "b"(arg1), "c"(arg2), "d"(arg3), "S"(arg4), "D"(arg5));
    return ret;
}

//需要六个参数的系统调用
static inline uint32_t _syscall6(uint32_t nr, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, uint32_t arg6)
{
    uint32_t ret;
    asm volatile(
        "pushl %%ebp\n"
        "movl %7, %%ebp\n"
        "int $0x80\n"
        "popl %%ebp\n"
        : "=a"(ret)
        : "a"(nr), "b"(arg1), "c"(arg2), "d"(arg3), "S"(arg4), "D"(arg5), "m"(arg6));
    return ret;
}

//写指令
uint32_t write(fd_t fd, char *buf,uint32_t len){
    return _syscall3(SYS_NR_WRITE, fd, (uint32_t)buf, len);
}

void sleep(uint32_t ms){
    _syscall1(SYS_NR_SLEEP, ms);
}