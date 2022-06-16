# This is a generic makefile designed for quick
# Deployment and usage in a flat directory
#
# Best used to get off the ground quickly
# and then tailor to your needs

DIR := ${CURDIR}
NAME := $(shell basename $(CURDIR))
BIN  = $(DIR)/build
HASH = $(BIN)/hashes
INFO = $(BIN)/bininfo

BINLIST = $(shell ls -p $(BIN) | grep -v /)
FINDTREE= $(shell which tree)

# tinygo compiler and linker
# Default to optimize for size. See tinygo -h for options
TINYGOCC=tinygo
TINYGOFLAGS= -opt z 

# Standard go compiler and linker
GOCC=go
GOBUILD=build
GORUN=run 
GOLFLAGS=-ldflags="-s -w"


.PHONY: clean

all: clean init-dirs build-stdgo misc success

build-stdgo: $(DIR)
	$(call PT,building $^ -> $(NAME))
	@$(GOCC) $(GOBUILD) $(LFLAGS) -o $(BIN)/$(NAME)-debug $^ 
	$(call PG,$(NAME) Done)
	$(call PT,striping $(NAME)-debug -> $(NAME)-rel-stripped)
	@strip $(BIN)/$(NAME)-debug -o $(BIN)/$(NAME)-rel-stripped
	$(call PG,$(NAME) Done)
	$(call PT,packing $(NAME)-rel-stripped -> $(NAME)-stripped-upx)
	@upx --best --lzma $(BIN)/$(NAME)-rel-stripped -o $(BIN)/$(NAME)-rel-stripped-upx
	$(call PG,$(NAME)-stripped-upx Done)

init-dirs:
	@mkdir -p $(HASH)
	@mkdir -p $(INFO)
	@mkdir -p $(BIN)	

misc: 
	@md5sum $(BIN)/*$(NAME)* >> $(HASH)/MD5SUMS
	@sha1sum $(BIN)/*$(NAME)* >> $(HASH)/SHA1SUMS
	$(call READELF_LOOP, $(BINLIST))

clean:
	$(call PY,Full clean of $(CURDIR))
	@$(GOCC) clean && rm -rf $(BIN)/*
	$(call PG,$@ Done)

success:
	$(call PG, BUILD COMPLETE) 
	@tree $(BIN)

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

define READELF_LOOP
	@for file in $(1) ; do \
	  out=$$file-readelf; \
	  readelf -a $(BIN)/$$file > $(INFO)/$$out 2> /dev/null; \
	done
endef
############ Functions ############