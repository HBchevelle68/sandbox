NASM_CC=nasm
NASM_CCFLAGS=-f elf64
LINKER=ld

CC=gcc


.PHONY: clean

all: hello entry_point

hello:  hello.asm
	$(NASM_CC) $(NASM_CCFLAGS) $< -o $@.o
	$(LINKER) $@.o -o $@

entry_point: entry_point.c
	$(CC) $< -o $@

clean:
	rm -f *.o hello entry_point