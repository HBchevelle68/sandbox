ROOTDIR := ${CURDIR}
SHELLDIR= $(ROOTDIR)/shellcode
TARGETDIR= $(ROOTDIR)/test-target
PAYLOADSDIR= $(ROOTDIR)/payloads
INJECTCOREDIR= $(ROOTDIR)/injection-core

CALLMAKE= $(MAKE) --no-print-directory -C 

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

standalone-shellcode:
	@$(CALLMAKE) $(SHELLDIR) $@

standalone-shellcode-dump-shellcode: 
	@$(CALLMAKE) $(SHELLDIR) $@

shellcode-test:
	@$(CALLMAKE) $(SHELLDIR) $@

shellcode-test-debug:
	@$(CALLMAKE) $(SHELLDIR) $@

libpayload:
	@$(CALLMAKE) $(PAYLOADSDIR) $@

inject-rel:
	@$(CALLMAKE) $(INJECTCOREDIR) $@

inject-debug: 
	@$(CALLMAKE) $(INJECTCOREDIR) $@

target-rel:
	@$(CALLMAKE) $(TARGETDIR) $@

target-debug:
	@$(CALLMAKE) $(TARGETDIR) $@

clean:
	$(call PY,Full clean of $(CURDIR))
	@rm -f *.o *.so .gdbhistory core  $(TGTS)
	@$(CALLMAKE) $(SHELLDIR) $@
	@$(CALLMAKE) $(TARGETDIR) $@
	@$(CALLMAKE) $(PAYLOADSDIR) $@
	@$(CALLMAKE) $(INJECTCOREDIR) $@

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

