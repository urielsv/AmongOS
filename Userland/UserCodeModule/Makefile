include ../Makefile.inc

# MODULE=0000-sampleCodeModule.bin
MODULE=userlandCodeModule.bin
SOURCES=$(wildcard *.c)
SOURCES_EXEC=$(wildcard exec/*.c)
SOURCES_LIBC=$(wildcard libc/*.c)
SOURCES_ASM=$(wildcard libasm/*.asm)
SOURCES_SHARED= $(wildcard ../../Shared/*.c)
SOURCES_TEST=$(wildcard tests/*.c)


OBJECTS=$(SOURCES:.c=.o)
OBJECTS_EXEC=$(SOURCES_EXEC:.c=.o)
OBJECTS_LIBC=$(SOURCES_LIBC:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_SHARED= $(SOURCES_SHARED:.c=.o)
OBJECTS_TEST=$(SOURCES_TEST:.c=.o)

ALL_OBJECTS= $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_EXEC) $(OBJECTS_LIBC) $(OBJECTS_SHARED) $(OBJECTS_TEST)

all: $(MODULE)

$(MODULE): $(ALL_OBJECTS)
	$(GCC) $(GCCFLAGS) -T userlandCodeModule.ld $(ALL_OBJECTS) -o ../$(MODULE)
	$(GCC) $(GCCFLAGS) -T userlandCodeModule.ld -Wl,--oformat=elf64-x86-64 -o ../userlandCodeModule.elf $^

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -I../../Shared/include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	@rm -rf *.o
	@rm -rf exec/*.o
	@rm -rf libasm/*.o
	@rm -rf libc/*.o
	@rm -rf tests/*.o	
	@rm -rf ../../Shared/*.o

.PHONY: all clean print