DIR := ${CURDIR}

NASM= nasm
NASMFLAGS= -f elf64
ASMLINK= ld -m elf_x86_64
ASMDBG= $(NASMFLAGS) -g -F dwarf

TGTS= standalone-shellcode loadso-shellcode shellcode-test shellcode-test-debug

.PHONY: clean

all: clean $(TGTS)

standalone-shellcode: standalone-shellcode.o
	$(call PT,Linking $^ -> $@)
	@$(ASMLINK) $^ -o $@
	$(call PG,$@ Done)

standalone-shellcode-dump-shellcode: standalone-shellcode
	@printf '\\x' && objdump -d standalone-shellcode | grep "^ " | cut -f2 | tr -d ' ' | tr -d '\n' | sed 's/.\{2\}/&\\x /g'| head -c-3 | tr -d ' ' && echo ' '

loadso-shellcode: loadso-shellcode.o
	$(call PT,Linking $^ -> $@)
	@$(ASMLINK) $^ -o $@
	$(call PG,$@ Done)

loadso-shellcode-dump-shellcode: loadso-shellcode
	@printf '\\x' && objdump -d loadso-shellcode | grep "^ " | cut -f2 | tr -d ' ' | tr -d '\n' | sed 's/.\{2\}/&\\x /g'| head -c-3 | tr -d ' ' && echo ' '

shellcode-test: shelltest.o
	$(call PT,Linking $^ -> $@)
	@$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@
	$(call PG,$@ Done)

shellcode-test-debug: shelltest-debug.o
	$(call PT,Linking $^ -> $@)
	@$(CC) $(DBGCFLAGS) $(DBG) $(LFLAGS) $^ -o $@
	$(call PG,$@ Done)

%.o: %.asm
	$(call PT,Building $< -> $@)
	@$(NASM) $(NASMFLAGS) $< -o $@

%.o: %.c
	$(call PT,Building $< -> $@)
	@$(CC) -c $(CFLAGS) $< -o $@

%-debug.o: %.c
	$(call PT,Building $< -> $@)
	@$(CC) -c $(DBG) $(DBGCFLAGS) $< -o $@

clean:
	$(call PY,Full clean of $(CURDIR))
	@rm -f *.o *.so .gdbhistory core $(TGTS)

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