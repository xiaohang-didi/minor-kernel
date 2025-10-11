#ifndef INCLUDE_GDT_H_
#define INCLUDE_GDT_H_
#include "types.h"

typedef
struct gdt_entry_t
{
    uint16_t limit_low;//段界限 15~0
    uint16_t base_low;//段基地址15~0
    uint32_t base_middle;//段基地址23~16
    uint8_t access;//段存在位，描述符特权级，描述符子类别
    uint8_t granularity;//其他标志位，段界限 19~16
    uint8_t base_high;//段基地址31~24
} __attribute__((packed)) gdt_entry_t;

typedef
struct gdt_ptr_t
{
    uint16_t limit;//segment limit
    uint32_t basa;//segment base
} __attribute__((packed)) gdt_ptr_t;

//initialize GDT
void init_gdt();

//load GDT to GDTR
extern void gdt_flush(uint32_t);

#endif