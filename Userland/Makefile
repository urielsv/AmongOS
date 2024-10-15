include Makefile.inc

SAMPLE_DATA=0001-sampleDataModule.bin

all: utilsModule sampleDataModule

utilsModule:
	cd UserCodeModule; make

sampleDataModule:
	printf "This is sample data." >> $(SAMPLE_DATA) && dd if=/dev/zero bs=1 count=1 >> $(SAMPLE_DATA)

clean:
	cd UserCodeModule; make clean
	rm -rf *.bin
	rm -rf *.elf


.PHONY: utilsModule all clean

