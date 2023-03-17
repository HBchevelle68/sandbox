NASM_CC=nasm
NASM_CCFLAGS=-f elf64
LD=ld

CC=gcc

TGTS= hello nodata entry_point

.PHONY: clean

all: $(TGTS)

hello: hello.asm
	$(NASM_CC) $(NASM_CCFLAGS) $< -o $@.o
	$(LD) $@.o -o $@

nodata: nodata.asm
	$(NASM_CC) $(NASM_CCFLAGS) $< -o $@.o
	$(LD) $@.o -o $@

entry_point: entry_point.c
	$(CC) $< -o $@

clean:
	rm -f *.o $(TGTS)