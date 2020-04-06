ENV = -g
CC = gcc
TARGET	= dwwrite
LIBDIR = ../../libs/

SRCS = dwwrite.cpp
OBJS = $(addsuffix .o,$(basename $(SRCS)))

LIBS = tables/symbol_table.o \
	   tables/symbol_table_factory.o \
	   tables/vdw_cu_table.o \
	   tables/vdw_type_table.o \
	   tables/vdw_var_table.o \
	   read/dw_reader.o \
	   util/c_string_utils.o \
	   libelfin/elf/libelf++.a \
	   libelfin/dwarf/libdwarf++.a

INCLUDES = -I$(LIBDIR)

LIBOBJS = $(addprefix $(LIBDIR), $(LIBS))
DEPENDS = $(addprefix .d_, $(basename $(SRCS)))

RM = /bin/rm -f

CFLAGS	= $(ENV) $(INCLUDES)
CXXFLAGS = $(ENV) $(INCLUDES) -std=c++11

#.SILENT:

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
