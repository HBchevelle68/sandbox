#
# Base Path variables
#
TOPLVLDIR := $(abspath ${CURDIR}/..)
BIN = $(TOPLVLDIR)/binaries
SRCBASE = $(TOPLVLDIR)/redirectors

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
BS_ROBJS=$(addprefix $(SRCBASE)/, redirector.o)
BS_DOBJS=$(addprefix $(SRCBASE)/, redirector-debug.o) 


.PHONY: clean

all: clean \
	 release \
	 debug \
	 misc \
	 scrub


#
# Release builds
#
release: redirector-release

redirector-release: $(BS_ROBJS)
	$(CC) $(GLOBAL_CFLAGS) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@


#
# Debug builds
#
debug: redirector-debug

redirector-debug: $(BS_DOBJS)
	$(CC) $(DBGCFLAGS) $(DBG) $^ $(CRYPTSTATIC) $(LFLAGS) -o $(BIN)/$@



%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(RELEASE) $< -o $@

%-debug.o: %.c $(DEPS)
	$(CC) -c $(DBG) $(CFLAGS) $< -o $@

misc:
	md5sum $(BIN)/*redirector* >> $(BIN)/MD5SUMS
	sha1sum $(BIN)/*redirector* >> $(BIN)/SHA1SUMS
	
scrub:
	rm -f $(SRCBASE)/*.o

clean:
	rm -fr $(BIN)/*redirector* $(SRCBASE)/*.o