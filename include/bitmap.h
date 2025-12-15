#ifndef BITMAP_H
#define BITMAP_H

#include "types.h"

typedef struct bitmap_t
{
    uint8_t *bits;   // 位图缓冲区
    uint32_t length; // 位图缓冲区长度
    uint32_t offset; // 位图开始的偏移
} bitmap_t;

// 初始化位图
void bitmap_init(bitmap_t *map, char *bits, uint32_t length, uint32_t offset);

// 构造位图
void bitmap_make(bitmap_t *map, char *bits, uint32_t length, uint32_t offset);

// 测试位图的某一位是否为 1
bool bitmap_test(bitmap_t *map, uint32_t index);

// 设置位图某位的值
void bitmap_set(bitmap_t *map, uint32_t index, bool value);

// 从位图中得到连续的 count 位
int bitmap_scan(bitmap_t *map, uint32_t count);

#endif
