#include "debug.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"

//申请内存块
static void alloc_chunk(uint32_t start,uint32_t len);

//释放内存块
static void free_chunk(header_t *chunk);

//切分内存块
static void split_chunk(header_t *chunk,uint32_t len);

//合并内存块
static void glue_chunk(header_t *chunk);

static uint32_t heap_max = HEAP_START;

//内存块管理头指针
static header_t *heap_first;

void init_heap(){
    heap_first = NULL;
}

void *kmalloc(uint32_t len){
    //所有申请的内存长度加上管理头的长度
    //因为在内存申请和释放的时候需要通过该结构去管理
    len += sizeof(header_t);
    header_t *cur_header = heap_first;
    header_t *prev_header = NULL;

    while(cur_header){
        //如果当前内存块没有被申请过而且长度大于申请的块
        if(cur_header->allocated == 0 && cur_header->length >= len){
            //按照当前长度切割内存
            split_chunk(cur_header,len);
            return (void *)((uint32_t)cur_header+sizeof(header_t));
        }
        //否则就要推移指针直至长度合适
        prev_header = cur_header;
        cur_header = cur_header->next;
    }
    uint32_t chunk_start;

    //第一次执行函数则初始化内存块的起始位置，之后根据当前指针加上申请的长度即可
    if(prev_header){
        chunk_start = (uint32_t)prev_header + prev_header->length;
    }else{
        chunk_start = HEAP_START;
        heap_first = (header_t *)chunk_start;
    }

    //检查是否需要申请内存页
    alloc_chunk(chunk_start,len);

}

void alloc_chunk(uint32_t start,uint32_t len){
    while (start + len > heap_max){
        uint32_t page = pmm_alloc_page();
        map(pgd_kern,heap_max,page,PAGE_PRESENT | PAGE_WRITE);
        heap_max += PAGE_SIZE;
    }
    
}

void free_chunk(header_t *chunk) {
    if (chunk->prev)
        chunk->prev->next = chunk->next;
    if (chunk->next)
        chunk->next->prev = chunk->prev;

    if (heap_first == chunk)
        heap_first = chunk->next;

    chunk->allocated = 0;

    // 仅当释放的块在堆顶时，才释放页
    if ((uint32_t)chunk + chunk->length == heap_max) {
        while ((heap_max - PAGE_SIZE) >= (uint32_t)chunk) {
            heap_max -= PAGE_SIZE;
            uint32_t page;
            get_mapping(pgd_kern, heap_max, &page);
            unmap(pgd_kern, heap_max);
            pmm_free_page(page);
        }
    }
}


void split_chunk(header_t *chunk,uint32_t len){
    //保证切剩下的内存块大小够一个 内存块大小
    if(chunk->length - len > sizeof(header_t)){
        header_t *newchunk = (header_t *)((uint32_t)chunk + len);
        newchunk->next = chunk->next;
        newchunk->prev = chunk;
        newchunk->allocated = 0;
        newchunk->length = chunk->length - len;
        chunk->allocated = 1;
        chunk->next = newchunk;
        chunk->length = len;
    }
}

void glue_chunk(header_t *chunk){
    if(chunk->next && chunk->next->allocated == 0){
        chunk->length = chunk->length + chunk->next->length;
        if(chunk->next->next){
            chunk->next->next->prev = chunk;
        }
        chunk->next = chunk->next->next;
    }
    if(chunk->prev && chunk->prev->allocated == 0){
        chunk->prev->length = chunk->length+chunk->prev->length;
        if(chunk->next){
            chunk->next->prev = chunk->prev;
        }
        chunk = chunk->prev;
    }
    if(chunk->next == NULL){
        free_chunk(chunk);
    }
}

void test_heap()
{
	printk_color(rc_black, rc_magenta, "Test kmalloc() && kfree() now ...\n\n");

	void *addr1 = kmalloc(50);
	printk("kmalloc    50 byte in 0x%X\n", addr1);
	void *addr2 = kmalloc(500);
	printk("kmalloc   500 byte in 0x%X\n", addr2);
	void *addr3 = kmalloc(5000);
	printk("kmalloc  5000 byte in 0x%X\n", addr3);
	void *addr4 = kmalloc(50000);
	printk("kmalloc 50000 byte in 0x%X\n\n", addr4);

	printk("free mem in 0x%X\n", addr1);
	kfree(addr1);
	printk("free mem in 0x%X\n", addr2);
	kfree(addr2);
	printk("free mem in 0x%X\n", addr3);
	kfree(addr3);
	printk("free mem in 0x%X\n\n", addr4);
	kfree(addr4);
}