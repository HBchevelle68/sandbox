# This is a generic makefile designed for quick
# deployment and usage in a flat directory
#
# Best used to get off the ground quickly
# and then tailor to your needs

DIR := ${CURDIR}

# tinygo compiler and linker
TINYGOCC=tinygo
TINYGOFLAGS= -opt z # Default to optimize for size. See tinygo -h for options

TINYGODBGFLAGS= -opt 0 

# Standard go compiler and linker
GOCC=go
GOBUILD=build
GORUN=run 
GOLFLAGS=-ldflags="-s -w"


TGTS=release-bin-stdgo debug-bin-stdgo
PCKTGTS=release-bin-stdgo-upx

.PHONY: clean

all: clean release $(TGTS)

release-bin-stdgo: $(DIR)
	$(call PT,building $^ -> $@)
	$(GOCC) $(GOBUILD) $(LFLAGS) -o $@ $^ 
	$(call PG,$@ Done)
	$(call PT,packing $^ -> $@)
	@upx --best --lzma $@ -o $@-upx
	$(call PG,$@-upx Done)

release-bin-stdgo-upx:
	@upx --best --lzma

debug-bin-stdgo: #$(DBGOBJS)
	$(call PT,Linking $^ -> $@)
	@$(CC) $(DBGCFLAGS) $(DBG) $(LFLAGS) -o $@ $^ 
	$(call PG,$@ Done)

clean:
	$(call PY,Full clean of $(CURDIR))
	@$(GOCC) clean && rm -f $(TGTS) $(PCKTGTS)
	$(call PG,$@ Done)

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