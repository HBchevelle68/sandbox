DIR := ${CURDIR}

CC= gcc -std=c11
CFLAGS= -Werror -Wall
LFLAGS = -ldl

SOCFLAGS= -fpic 
SOLFLAGS= -shared

DBG= -g3
DBGCFLAGS= -Werror -Wall

TGTS= libpayload

.PHONY: clean

all: clean $(TGTS)

libpayload: libpayload.o
	$(call PT,Linking $^ -> $@.so)
	@$(CC) $(LFLAGS) $(SOLFLAGS) $^ -o $@.so

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