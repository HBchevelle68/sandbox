DIR := ${CURDIR}
DUMPSC=	$(objdump -d ./stage0 |grep '[0-9a-f]:'|grep -v 'file'|cut -f2 -d:|cut -f1-6 -d' '|tr -s ' '|tr '\t' ' '|sed 's/ $//g'|sed 's/ /\\x/g'|paste -d '' -s |sed 's/^/"/'|sed 's/$/"/g')


NASM= nasm
NASMFLAGS= -f elf64
ASMLINK= ld
ASMDBG= $(NASMFLAGS) -g -F dwarf

CC= gcc -std=c11
CFLAGS= -Werror -Wall
LFLAGS = -ldl
DBG= -g2 -Og
DBGCFLAGS= -Werror -Wall

BINS= stage0 target-rel target-debug inject-rel inject-debug



.PHONY: clean stage0-shellcode

all: clean $(BINS) 

stage0: stage0.o
	@$(ASMLINK) $^ -o $@

stage0-shellcode:
	@printf '\\x' && objdump -d stage0 | grep "^ " | cut -f2 | tr -d ' ' | tr -d '\n' | sed 's/.\{2\}/&\\x /g'| head -c-3 | tr -d ' ' && echo ' '

inject-rel: inject.o
	$(call PT, Linking $^ -> $@)
	@$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@
	$(call PG, $@ Done)

inject-debug: inject-debug.o
	$(call PG, Linking $^ -> $@)
	@$(CC) $(DBGCFLAGS) $(DBG) $(LFLAGS) $^ -o $@
	$(call PG, $@ Done)

target-rel: target.o
	$(call PT, Linking $^ -> $@)
	@$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@
	$(call PG, $@ Done)

target-debug: target-debug.o
	$(call PT, Linking $^ -> $@)
	@$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@
	$(call PG, $@ Done)

%.o: %.asm
	$(call PT, building $< -> $@)
	@$(NASM) $(NASMFLAGS) $< -o $@

%.o: %.c
	$(call PT, building $< -> $@)
	@$(CC) -c $(CFLAGS) $< -o $@

%-debug.o: %.c
	$(call PT, building $< -> $@)
	@$(CC) -c $(DBG) $(DBGCFLAGS) $< -o $@

clean:
	$(call PY, Full clean...)
	@rm -f *.o $(BINS)

# Functions
# Print Yellow
define PY
	@echo "\033[38;5;227m[*] $(1) \033[0m"
endef
# Print Teal
define PT
	@echo "\033[38;5;081m[>] $(1) \033[0m"
endef
# Print Green
define PG
	@echo "\033[38;5;084m[+] $(1) \033[0m"
endef




