#
# Base Path variables
#
TOPLVLDIR := $(abspath ${CURDIR}/..)
BIN = $(TOPLVLDIR)/binaries
REDIRSRC = $(TOPLVLDIR)/redirectors
SRCBASE = $(TOPLVLDIR)/shells

#
# Includes
#
INCLUDE= $(TOPLVLDIR)/include

#
# Wolfssl directories 
#
CRYPTBASE= $(TOPLVLDIR)/crypt
CRYPTINC= $(CRYPTBASE)/include
CRYPTSTATIC= $(CRYPTBASE)/lib/libwolfssl.a

#
# Build variables
#
CC= gcc
GLOBAL_CFLAGS= -Werror -Wall -s -O2 -I$(INCLUDE) -I$(CRYPTINC)
GLOBAL_DBG_CFLAGS= -Werror -Wall -g3 -I$(INCLUDE) -I$(CRYPTINC)
GLOBAL_DBG= -g3
LFLAGS= -L$(CRYPTBASE)/lib -lm -lpthread
STATIC= -static

#
# Bind Shell
#
BS_ROBJS=$(addprefix $(SRCBASE)/, bind_shell.o)
BS_DOBJS=$(addprefix $(SRCBASE)/, bind_shell-debug.o) 

#
# Reverse Shell
#
RS_ROBJS=$(addprefix $(SRCBASE)/, reverse_shell.o)
RS_DOBJS=$(addprefix $(SRCBASE)/, reverse_shell-debug.o) 

#TOBJS=$(addprefix $(SRC)/, main-test.o server-test.o crypto-test.o sighandler-test.o ns-test.o)


.PHONY: clean

all: clean \
	 release \
	 debug \
	 misc \
	 scrub


#test: clean netsand-release-test runtest scrub


#
# Release builds
#
release: bind_shell-release reverse_shell-release

bind_shell-release: $(BS_ROBJS)
	$(CC) $(GLOBAL_CFLAGS) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

reverse_shell-release: $(RS_ROBJS)
	$(CC) $(GLOBAL_CFLAGS) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

#
# Debug builds
#
debug: bind_shell-debug reverse_shell-debug

bind_shell-debug: $(BS_DOBJS)
	$(CC) $(DBGCFLAGS) $(DBG) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@

reverse_shell-debug: $(RS_DOBJS)
	$(CC) $(DBGCFLAGS) $(DBG) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@


%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(RELEASE) $< -o $@

%-test.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(TEST) $(DBG) $(RELEASE) $< -o $@

%-debug.o: %.c $(DEPS)
	$(CC) -c $(DBG) $(CFLAGS) $< -o $@

misc:
	md5sum $(BIN)/*shell* >> $(BIN)/MD5SUMS
	sha1sum $(BIN)/*shell* >> $(BIN)/SHA1SUMS
	
scrub:
	rm -f $(SRCBASE)/*.o

clean:
	rm -fr $(BIN)/*shell* $(SRCBASE)/*.o