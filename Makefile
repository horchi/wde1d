#
# Makefile
#
# See the README file for copyright information and how to reach the author.
#
#

CC = g++

TARGET   = wde1d
CLTARGET = dbchart

LIBS = -lmysqlclient_r -lrt
DEFINES += -D_GNU_SOURCE
CFLAGS   = -ggdb -Wreturn-type -Wformat -pedantic -Wunused-variable -Wunused-label \
           -Wunused-value -Wunused-function \
           -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

VERSION = $(shell grep 'define VERSION ' $(TARGET).h | awk '{ print $$3 }' | sed -e 's/[";]//g')
TMPDIR = /tmp
ARCHIVE = $(TARGET)-$(VERSION)

# object files 

LOBJS  =  lib/db.o lib/tabledef.o lib/common.o
OBJS   = $(LOBJS) main.o update.o csv.o serial.o
CLOBJS = $(LOBJS) chart.o

# rules:

%.o: %.c
	$(CC) $(CFLAGS) -c $(DEFINES) -o $@ $<

all: $(TARGET) $(CLTARGET)

$(CLTARGET): $(CLOBJS)
	$(CC) $(CFLAGS) $(CLOBJS) $(LIBS) -lmgl -o $@

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

clean:
	@-rm -f $(OBJS) $(CLOBJS) core* *~ lib/*~ lib/t *.jpg
	rm -f $(TARGET) $(CLTARGET) $(ARCHIVE).tgz

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(ARCHIVE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(ARCHIVE).tgz


#***************************************************************************
# dependencies
#***************************************************************************

HEADER = lib/db.h lib/common.h wde1d.h

lib/common.o    :  lib/common.c   lib/common.h $(HEADER)
lib/config.o    :  lib/config.c   lib/config.h $(HEADER)
lib/db.o        :  lib/db.c       lib/db.h $(HEADER)
lib/tabledef.o  :  lib/tabledef.c lib/tabledef.h $(HEADER)

main.o			 :  main.c        $(HEADER)
csv.o  			 :  csv.c         $(HEADER)
serial.o        :  serial.c      $(HEADER)
update.o        :  update.c      $(HEADER)
chart.o         :  chart.c

