# if you type 'make' without arguments, this is the default
PROG = parking
all: $(PROG)

# other source files and the associated object files (this can be blank) SRC is unused here
SRC     = parking.c readsubs.c hashdb.c misc.c commands.c split_input.c
HEAD    = commands.h hashdb.h misc.h readsubs.h split_input.h
OBJ     = parking.o misc.o readsubs.o commands.o split_input.o insert_ticket.o del_ticket.o vehicle_lookup.o free_tickets.o \
          hashsubs.o

# Testing
TDIR	= ./tests

# special libraries
LIB	= helperLib.a
LIBFLAGS =-L ./ $(LIB)

# select the compiler and flags you can over-ride on command line e.g. make DEBUG= 
CC      = gcc
DEBUG	= -ggdb
CSTD	=
WARN	= -Wall -Wextra -Werror
CDEFS	=
CFLAGS	= -I. $(DEBUG) $(WARN) $(CSTD) $(CDEFS)

command.o:          hashdb.h commands.h
hashdb.o:           hashdb.h
hashsubs.o:         hashdb.h
misc.o:             misc.h
parking.o:          hashdb.h misc.h readsubs.h commands.h
readsubs.o:         hashdb.h readsubs.h misc.h split_input.h
split_input.o:      hashdb.h split_input.h
insert_ticket.o:    hashdb.h readsubs.h
del_ticket.o:       hashdb.h
free_tickets.o:     hashdb.h
vehicle_lookup.o:   hashdb.h

# specify how to compile the target
$(PROG):	$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIBFLAGS) -o $@

# test the program
test:	$(PROG)
	(cd $(TDIR); make $@)

#demo mode
debug:  demo
demo:   $(PROG)
	./parking -f $(TDIR)/in/Fines.csv -d $(TDIR)/in/Tiny.csv -t3

# remove binaries
.PHONY: clean clobber update
clean:
	rm -f $(OBJ) $(PROG)
	(cd $(TDIR); make $@)

# remove binaries and other junk
clobber: clean
	rm -f core

#update the library
update:
	cp -p ~/public/helperLib.a .
