[GLOBAL gdt_flush]

gdt_flush:
    mov eax, [esp+4] ;参数存入eax寄存器
    lgdt [eax]       ;加载到GDTR修改原先[设置GRUB]
    //这里加载数据段的选择子
    mov ax, 0x10     ; 加载我们的数据段描述符
    mov ds, ax       ; 更新所有可以更新的段寄存器mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush  ; 远跳转，0x08是我们的代码段描述符
                     ; 远眺目的是为了清空流水席那并串行化处理器

.flush:
    ret