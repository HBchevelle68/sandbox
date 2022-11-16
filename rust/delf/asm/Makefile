CC=nasm
CCFLAGS=-f elf64
LINKER=ld

.PHONY: clean

all: hello

hello:  hello.asm
	$(CC) $(CCFLAGS) $< -o $@.o
	$(LINKER) $@.o -o $@

clean:
	rm -f *.o hello