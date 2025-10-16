#ifndef INCLUDE_PMM_H
#define INCLUDE_PMM_H

#include "multiboot.h"

extern uint8_t kern_start[];
extern uint8_t kern_end[];

void show_memory_map();

#endif