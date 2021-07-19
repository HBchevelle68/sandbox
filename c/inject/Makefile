DIR := ${CURDIR}

NASM= nasm
NASMFLAGS= -felf64
ASMLINK= ld
ASMDBG= $(NASMFLAGS) -g -F dwarf

CC= gcc -std=c11
CFLAGS= -Werror -Wall
LFLAGS = -ldl
DBG= -g2 -Og
DBGCFLAGS= -Werror -Wall

BINS= stage0 target-rel target-debug inject-rel inject-debug

.PHONY: clean

all: clean $(BINS)

stage0: stage0.o
	@$(ASMLINK) $^ -o $@

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




