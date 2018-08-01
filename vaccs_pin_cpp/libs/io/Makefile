# $Id: Makefile,v 1.7 1997/03/27 20:27:20 carr Exp $
SRCS		= vaccs_record.cpp arch_record.cpp vaccs_record_factory.cpp \
			  vaccs_record_reader.cpp asm_record.cpp binary_record.cpp \
			  ccode_record.cpp func_inv_record.cpp output_record.cpp \
			  register_record.cpp return_record.cpp return_addr_record.cpp \
			  sd_clear_record.cpp sd_czero_record.cpp sd_lock_record.cpp \
			  sd_set_record.cpp sd_unlock_record.cpp section_record.cpp \
			  var_access_record.cpp cmd_line_record.cpp

CC = gcc
CXX = g++

OBJS = $(addsuffix .o,$(basename $(SRCS)))

ENV = -g

INCLUDES=  \
			 -I..

CFLAGS	= $(INCLUDES) $(ENV) -fPIC
CXXFLAGS	= $(INCLUDES) $(ENV) -fPIC -fabi-version=2

RM = /bin/rm -f

.SILENT:

DEPENDS = $(addprefix .d_, $(basename $(SRCS)))

.SUFFIXES: .c .cpp

target: $(OBJS)
		echo "Done."

.PHONY: clean

clean:
		$(RM) $(OBJS) $(DEPENDS)

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
	$(CPP) -MM -MT '$(basename $<).o' $(INCLUDES) -MF $@ $<

include $(DEPENDS)

# DO NOT DELETE THIS LINE -- make depend depends on it.
