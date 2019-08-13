# $Id: Makefile,v 1.7 1997/03/27 20:27:20 carr Exp $
SRCS		= symbol_table.cpp cu_table.cpp type_table.cpp var_table.cpp \
			symbol_table_factory.cpp frame.cpp deref.cpp heap.cpp

CC = gcc
CXX = g++

OBJS = $(addsuffix .o,$(basename $(SRCS)))

PINDIR = /home/carr/vaccs/dynamic_analysis/pin
ENV = -g

INCLUDES=  \
	-I.. \
    -I$(PINDIR)/source/include/pin \
    -I$(PINDIR)/source/include/pin/gen \
    -I$(PINDIR)/extras/components/include \
    -I$(PINDIR)/extras/xed-intel64/include/xed \
    -I$(PINDIR)/source/tools/InstLib

CFLAGS	= $(INCLUDES) $(ENV) -fPIC
CXXFLAGS	= $(INCLUDES) $(ENV) -Werror -Wall -Wno-unknown-pragmas \
	 -D__PIN__=1 -DPIN_CRT=1 -fno-stack-protector -fno-exceptions -funwind-tables \
	 -fasynchronous-unwind-tables -fno-rtti -DTARGET_IA32E -DHOST_IA32E -fPIC -DTARGET_LINUX \
	 -fabi-version=2 -isystem /home/carr/vaccs/dynamic_analysis/pin/extras/stlport/include \
	 -isystem /home/carr/vaccs/dynamic_analysis/pin/extras/libstdc++/include \
	 -isystem /home/carr/vaccs/dynamic_analysis/pin/extras/crt/include \
	 -isystem /home/carr/vaccs/dynamic_analysis/pin/extras/crt/include/arch-x86_64 \
	 -isystem /home/carr/vaccs/dynamic_analysis/pin/extras/crt/include/kernel/uapi \
	 -isystem /home/carr/vaccs/dynamic_analysis/pin/extras/crt/include/kernel/uapi/asm-x86

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
	$(CPP) $(CXXFLAGS) -MM -MT '$(basename $<).o' -MF $@ $<

include $(DEPENDS)

# DO NOT DELETE THIS LINE -- make depend depends on it.
