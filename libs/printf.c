#include"vargs.h"
#include"stdio.h"
#include"syscall.h"
#include"types.h"

int vsprintf(char *buff, const char *format, va_list args);

static char buf[1024];

void printf(const char * fmt, ...){
    va_list args;

    int i;

    va_start(args,fmt);

    i = vsprintf(buf, fmt, args);
    
    va_end(args);

    write(stdout, buf, i);

}