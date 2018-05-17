SRCS = abbrv_table.c
CC = gcc

OBJS = $(addsuffix .o,$(basename $(SRCS)))

ENV = -g

LIB = libdebug$(ENV).so

VACCS_LIBS = ../lib/libutil-g.so

TEST_OBJS = check_abbrv_table.o ../unity/src/unity.o
DWARF_OBJS = reader.o

INCLUDES= -I. \
	  -I.. \
	  -I../libdwarf/libdwarf

LIBDIRS = -L.
          
CFLAGS	= $(INCLUDES) -fPIC $(ENV) 

LFLAGS = -shared

RM = /bin/rm -f


DEPENDS = $(addprefix .d_, $(basename $(SRCS)))

.SUFFIXES: .c

#.SILENT:

$(LIB): $(OBJS) $(VACCS_LIBS)
	echo "Linking" $@
	$(CC) $(LFLAGS) -o $@ $(OBJS) $(VACCS_LIBS)
	

.PHONY: clean

clean:
	$(RM) $(OBJS) $(TEST_OBJS)

check_abbrv: $(TEST_OBJS) $(LIB) $(VACCS_LIBS)
	echo "Creating executable" $@
	$(CC) $(LIBDIRS) -o $@ $(TEST_OBJS) $(LIB) $(VACCS_LIBS) -lm

dwarfread: $(DWARF_OBJS) $(LIB) $(VACCS_LIBS)
	echo "Creating executable" $@
	$(CC) $(LIBDIRS) -o $@ $(DWARF_OBJS) $(LIB) $(VACCS_LIBS) -lm -ldwarf -lelf

.c.o:
	echo "Compiling" $<
	$(CC) -c $(CFLAGS) $<

#
# The following two rules make the dependence file for the C source
# files. The C files depend upon the corresponding dependence file. The
# dependence file depends upon the source file's actual dependences. This way
# both the dependence file and the source file are updated on any change.
# The depend.sed sed command file sets up the dependence file appropriately.
#

.d_%: %.c 
	echo  "Updating dependences for" $< "..."
	$(CPP) -MM -MT '($(basename $<).o)' $(INCLUDES) -MF $@ $<
	 

#
# This includes all of the dependence files. If the file does not exist,
# GNU Make will use one of the above rules to create it.
#

include $(DEPENDS)
	 
# DO NOT DELETE THIS LINE -- make depend depends on it.
