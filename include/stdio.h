#ifndef STDIO_H_
#define STDIO_H_H

#include "stdarg.h"

int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);
void printf(const char *fmt, ...);

#endif