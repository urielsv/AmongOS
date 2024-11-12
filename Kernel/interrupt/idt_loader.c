// this is a personal academic project. dear pvs-studio, please check it.
// pvs-studio static code analyzer for c, c++ and c#: http://www.viva64.com
#include <idt_loader.h>
#include "defs.h"

#pragma pack(push)
#pragma pack(1)
#pragma pack(pop)

struct interrupt_descriptor_64
{
    uint16_t offset_l, selector;
    uint8_t zero, access;
    uint16_t offset_m;
    uint32_t offset_h, other_zero;
};



typedef struct interrupt_descriptor_64* idt_entry;

idt_entry idt = (idt_entry)0x00;

static void setup_idt_entry(int index, uint64_t offset);

void idt_loader() {
    asm_cli();
    // hardware1
    setup_idt_entry(0x20,(uint64_t) &asm_irq00_handler);
    setup_idt_entry(0x21,(uint64_t) &asm_irq01_handler);

    // exceptions
    setup_idt_entry(0x00, (uint64_t)&asm_exception00_handler);
    setup_idt_entry(0x06, (uint64_t)&asm_exception06_handler);

    // software
    setup_idt_entry(0x80,(uint64_t) &asm_syscall80_handler);
    
    pic_master_mask(0xfc);
    pic_slave_mask(0xff);

    asm_sti();
}

static void setup_idt_entry(int index, uint64_t offset)
{
    idt[index].selector = 0x08;
    idt[index].offset_l = offset & 0xffff;
    idt[index].offset_m = (offset >> 16) & 0xffff;
    idt[index].offset_h = (offset >> 32) & 0xffffffff;
    idt[index].access = ACS_INT;
    idt[index].zero = 0;
    idt[index].other_zero = (uint64_t)0;
}