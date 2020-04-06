ENV = -g
CC = gcc
TARGET	= dwread
LIBDIR = ../../libs/
TESTDIR = ../../test/

SRCS = test_dwread.cpp
OBJS = $(addsuffix .o,$(basename $(SRCS)))

LIBS = tables/symbol_table.o \
	   tables/symbol_table_factory.o \
	   tables/cu_table.o \
	   tables/type_table.o \
	   tables/var_table.o \
	   read/dw_reader.o \
	   libelfin/elf/libelf++.a \
	   libelfin/dwarf/libdwarf++.a

INCLUDES = -I$(LIBDIR) \
			-I$(TESTDIR)

LIBOBJS = $(addprefix $(LIBDIR), $(LIBS))
DEPENDS = $(addprefix .d_, $(basename $(SRCS)))

RM = /bin/rm -f

CFLAGS	= $(ENV) $(INCLUDES)
CXXFLAGS = $(ENV) $(INCLUDES) -std=c++11

.SILENT:

$(TARGET): $(OBJS) $(LIBOBJS)
		echo "Creating $@" 
		$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBOBJS)

.PHONY: clean

clean:
		$(RM) $(OBJS) $(TARGET) $(DEPENDS)

.c.o:
		echo "Compiling" $<
		$(CC) -c $(CFLAGS) $<

.cpp.o:
		echo "Compiling" $<
		$(CXX) -c $(CXXFLAGS) $<


.d_%: %.c 
		echo  "Updating dependences for" $< "..."
		$(CPP) -MM -MT '$(basename $<).o' $(INCLUDES) -MF $@ $<
		
.d_%: %.cpp
		echo  "Updating dependences for" $< "..."
		$(CPP) -std=c++11 -MM -MT '$(basename $<).o' $(INCLUDES) -MF $@ $<

#
# This includes all of the dependence files. If the file does not exist,
# GNU Make will use one of the above rules to create it.
#

include $(DEPENDS)
 	 
# 	 # DO NOT DELETE THIS LINE -- make depend depends on it.
