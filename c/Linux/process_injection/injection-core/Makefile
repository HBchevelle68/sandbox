DIR := ${CURDIR}

CC= gcc -std=c11
CFLAGS= -Werror -Wall
LFLAGS = -ldl

DBG= -g3
DBGCFLAGS= -Werror -Wall

TGTS= inject-rel inject-debug

.PHONY: clean

all: clean $(TGTS)

inject-rel: ptrace_utils.o process_utils.o inject.o main.o
	$(call PT,Linking $^ -> $@)
	@$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@
	$(call PG,$@ Done)

inject-debug: ptrace_utils-debug.o process_utils-debug.o inject-debug.o main-debug.o
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
	@rm -f *.o *.so .gdbhistory core  $(TGTS)

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