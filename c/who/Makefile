DIR := ${CURDIR}

CC= gcc
CFLAGS= -Werror -Wall
LFLAGS= 

DBG= -g3
DBGCFLAGS= -Werror -Wall

# Find all .c files
$(eval SRC=$(shell find $(PWD) -maxdepth 1 -name "*.c"))

# Convert .c files to just the basefile name 
$(eval BASENAMES=$(shell basename $(SRC) .c))  

# Append the .o suffix for release builds
RELOBJS = $(addsuffix .o,$(BASENAMES))

# Append the -debug.o suffix for debug builds
DBGOBJS = $(addsuffix -debug.o,$(BASENAMES))

TGTS= release-bin debug-bin

.PHONY: clean

all: clean $(TGTS)

release-bin: $(RELOBJS)
	$(call PT,Linking $^ -> $@)
	@$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@
	$(call PG,$@ Done)

debug-bin: $(DBGOBJS)
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