ENV = -g
CC = gcc
TARGET	= vio
LIBDIR = ../../libs/
TESTDIR = ../../test/

SRCS = test_io.cpp
OBJS = $(addsuffix .o,$(basename $(SRCS)))

LIBS = io/vaccs_record.o \
	   io/arch_record.o \
	   io/asm_record.o \
	   io/binary_record.o \
	   io/ccode_record.o \
	   io/cmd_line_record.o \
	   io/func_inv_record.o \
	   io/output_record.o \
	   io/register_record.o \
	   io/return_addr_record.o \
	   io/return_record.o \
	   io/sd_clear_record.o \
	   io/sd_czero_record.o \
	   io/sd_lock_record.o \
	   io/sd_set_record.o \
	   io/sd_unlock_record.o \
	   io/section_record.o \
	   io/vaccs_record_factory.o \
	   io/vaccs_record_reader.o \
	   io/var_access_record.o \
	   util/c_string_utils.o

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
