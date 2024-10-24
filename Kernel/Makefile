include Makefile.inc

KERNEL=kernel.bin
KERNEL_ELF=$(KERNEL:.bin=.elf)

SOURCES=$(wildcard *.c)
SOURCES_DRIVERS=$(wildcard drivers/*.c)
SOURCES_IDT=$(wildcard idt/*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_UTILS=$(wildcard utils/*.c)
SOURCES_TEST=$(wildcard tests/*.c)
SOURCES_SHARED= $(wildcard ../Shared/*.c)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_DRIVERS=$(SOURCES_DRIVERS:.c=.o)
OBJECTS_IDT=$(SOURCES_IDT:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_UTILS=$(SOURCES_UTILS:.c=.o)
OBJECTS_SHARED= $(SOURCES_SHARED:.c=.o)

ALL_OBJECTS= $(OBJECTS) $(OBJECTS_DRIVERS) $(OBJECTS_IDT) $(OBJECTS_ASM) $(OBJECTS_UTILS) 

# Shared source files

LOADERSRC=loader.asm
LOADEROBJ=$(LOADERSRC:.asm=.o)
STATICLIBS=$(OBJECTS_SHARED)

all: $(KERNEL)

$(KERNEL): $(LOADEROBJ) $(STATICLIBS) $(ALL_OBJECTS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJ) $(STATICLIBS) $(ALL_OBJECTS)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o ./kernel.elf $^

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -I../Shared/include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJ):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJ)

clean:
	rm -rf *.bin *.elf *.o drivers/*.o idt/*.o libasm/*.o libc/*.o test/*.o utils/*.o asm/*.o ../Shared/*.o 

.PHONY: all clean