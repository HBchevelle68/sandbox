DIR := ${CURDIR}

NASM= nasm
NASMFLAGS= -f elf64
ASMLINK= ld -m elf_x86_64
ASMDBG= $(NASMFLAGS) -g -F dwarf

CC= gcc -std=c11
CFLAGS= -Werror -Wall
LFLAGS = -ldl

SOCFLAGS= -fpic 
SOLFLAGS= -shared

DBG= -g3
DBGCFLAGS= -Werror -Wall

TGTS= standalone-shellcode target-rel \
	  target-debug inject-rel inject-debug \
	  shellcode-test shellcode-test-debug libpayload



.PHONY: clean standalone-shellcode-dump-shellcode

all: clean $(TGTS) 

############ SHELLCODE  ############

standalone-shellcode: standalone-shellcode.o
	$(call PT,Linking $^ -> $@)
	@$(ASMLINK) $^ -o $@
	$(call PG,$@ Done)

standalone-shellcode-dump-shellcode: standalone-shellcode
	@printf '\\x' && objdump -d standalone-shellcode | grep "^ " | cut -f2 | tr -d ' ' | tr -d '\n' | sed 's/.\{2\}/&\\x /g'| head -c-3 | tr -d ' ' && echo ' '

shellcode-test: shelltest.o
	$(call PT,Linking $^ -> $@)
	@$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@
	$(call PG,$@ Done)

shellcode-test-debug: shelltest-debug.o
	$(call PT,Linking $^ -> $@)
	@$(CC) $(DBGCFLAGS) $(DBG) $(LFLAGS) $^ -o $@
	$(call PG,$@ Done)

############ SHELLCODE  ############

############ PAYLOAD (.so) ############

libpayload: libpayload.o
	$(call PT,Linking $^ -> $@)
	@$(CC) $(LFLAGS) $(SOLFLAGS) $^ -o $@.so


############ PAYLOAD (.so) ############

############ CORE TOOLS ############

inject-rel: ptrace_utils.o inject.o main.o
	$(call PT,Linking $^ -> $@)
	@$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@
	$(call PG,$@ Done)

inject-debug: ptrace_utils-debug.o inject-debug.o main-debug.o
	$(call PT,Linking $^ -> $@)
	@$(CC) $(DBGCFLAGS) $(DBG) $(LFLAGS) $^ -o $@
	$(call PG,$@ Done)

############ CORE TOOLS ############

############ TARGET BIN ############

target-rel: target.o 
	$(call PT,Linking $^ -> $@)
	@$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@
	$(call PG,$@ Done)

target-debug: target-debug.o
	$(call PT,Linking $^ -> $@)
	@$(CC) $(DBGCFLAGS) $(DBG) $(LFLAGS) $^ -o $@
	$(call PG,$@ Done)

############ TARGET BIN ############

############# GENERIC ##############
%.o: %.asm
	$(call PT,Building $< -> $@)
	@$(NASM) $(NASMFLAGS) $< -o $@

%.o: %.c
	$(call PT,Building $< -> $@)
	@$(CC) -c $(CFLAGS) $< -o $@

%-debug.o: %.c
	$(call PT,Building $< -> $@)
	@$(CC) -c $(DBG) $(DBGCFLAGS) $< -o $@

lib%.o: %.c
	$(call PT,Building $< -> $@)
	@$(CC) -c $(CFLAGS) $(SOCFLAGS) $< -o $@

clean:
	$(call PY,Full clean...)
	@rm -f *.o *.so .gdbhistory core  $(TGTS)

############# GENERIC ##############

############ Functions ############
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
############ Functions ############



