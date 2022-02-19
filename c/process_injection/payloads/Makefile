DIR := ${CURDIR}

CC= gcc -std=c11
CFLAGS= -Werror -Wall
LFLAGS = -ldl -pthread

SOCFLAGS= -fpic 
SOLFLAGS= -shared

DBG= -g3
DBGCFLAGS= $(CFLAGS)

TGTS= libpayload test-payload

.PHONY: clean

all: clean $(TGTS)

libpayload: payload.o
	$(call PT,Linking $^ -> $@.so)
	@$(CC) $(SOLFLAGS) $(LFLAGS) $^ -o $@.so

test-payload: test-payload-norm.o
	$(call PT,Linking $^ -> $@)
	@$(CC) -ldl $^ -o $@

%-norm.o: %.c
	$(call PT,Building $< -> $@)
	@$(CC) -c $(DBG) $(CFLAGS)  $< -o $@


%.o: %.c
	$(call PT,Building $< -> $@)
	@$(CC) -c $(CFLAGS) $(SOCFLAGS) $< -o $@

%-debug.o: %.c
	$(call PT,Building $< -> $@)
	@$(CC) -c $(DBG) $(SOCFLAGS) $(DBGCFLAGS) $< -o $@

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