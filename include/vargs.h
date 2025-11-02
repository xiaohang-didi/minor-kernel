#ifndef INCLUDE_VARGS_H_
#define INCLUDE_VARGS_H_

//__builtin（）使用的是编译器的内建函数，帮助我们进行一些操作
typedef __builtin_va_list va_list;

//va_list是一种特殊的数据类型，用来存储可变参数列表
//va_start中，ap表示指向可变参数列表的第一个参数，last指向已知可变参数的最后一个参数
//va_arg表示从可变参数列表中检索参数
//va_end 函数用于清理可变参数列表
#define va_start(ap, last)         (__builtin_va_start(ap, last))
#define va_arg(ap, type)           (__builtin_va_arg(ap, type))
#define va_end(ap) 

#endif 	// INCLUDE_VARGS_H_
