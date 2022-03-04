DIR := ${CURDIR}

CC= gcc
CFLAGS= -v -Werror -Wall
LFLAGS= 

DBG= -g3
DBGCFLAGS= -Werror -Wall

TGTS= dirwalk-rel dirwalk-debug

.PHONY: clean

dirwalk-rel: dirwalk.o
	$(call PT,Linking $^ -> $@)
	@$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@
	$(call PG,$@ Done)

dirwalk-debug: dirwalk-debug.o
	$(call PT,Linking $^ -> $@)
	@$(CC) $(DBGCFLAGS) $(DBG) $(LFLAGS) $^ -o $@
	$(call PG,$@ Done)

%.o: %.c
	$(call PT,Building $< -> $@)
	@$(CC) -c $(CFLAGS) $< -o $@

%-debug.o: %.c
	$(call PT,Building $< -> $@)
	@$(CC) -c $(DBG) $(DBGCFLAGS) $< -o $@

clean:
	$(call PY,Full clean of $(CURDIR))
	@rm -f *.o .gdbhistory $(TGTS)

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