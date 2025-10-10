#ifndef INCLUDE_GDT_H_
#define INCLUDE_GDT_H_
#include "types.h"

typedef
struct gdt_ptr_r
{
    uint16_t limit;//segment limit
    uint32_t basa;//segment base
} __attribute__((packed)) gdt_entry_t;

//initialize GDT
void init_gdt();

//load GDT to GDTR
extern void gdt_flush(uint32_t);

#endif