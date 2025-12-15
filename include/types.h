#ifndef INCLUDE_TYPES_H_
#define INCLUDE_TYPES_H_

#define EOS '\0' // 字符串结尾
#define EOF -EEOF // END OF FILE

#ifndef NULL
	#define NULL 0
#endif

#ifndef TRUE
	#define TRUE  1
	#define FALSE 0
#endif

typedef unsigned int   uint32_t;
typedef          int   int32_t;
typedef unsigned short uint16_t;
typedef          short int16_t;
typedef unsigned char  uint8_t;
typedef          char  int8_t;

typedef uint32_t pid_t;
typedef uint32_t dev_t;

typedef uint32_t time_t;
typedef uint32_t idx_t;

//文件描述符
typedef uint32_t fd_t;
typedef enum std_fd_t{
	stdin,
	stdout,
	stderr,
} std_fd_t;

#ifndef __cplusplus
#define bool _Bool
#define true 1
#define false 0
#endif

#endif 	// INCLUDE_TYPES_H_
