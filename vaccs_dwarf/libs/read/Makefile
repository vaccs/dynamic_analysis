# $Id: Makefile,v 1.7 1997/03/27 20:27:20 carr Exp $
SRCS		= dw_reader.cpp base_expr_context.cpp

CC = gcc
CXX = g++

OBJS = $(addsuffix .o,$(basename $(SRCS)))

ENV = -g

INCLUDES=  \
			 -I..

CFLAGS	= $(INCLUDES) $(ENV) -fPIC
CXXFLAGS	= $(INCLUDES) $(ENV) -fPIC -std=c++0x -Werror -Wall -fabi-version=2 -DNDEBUG

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
	$(CPP) -std=c++0x -Werror -Wall -MM -MT '$(basename $<).o' $(INCLUDES) -MF $@ $<

include $(DEPENDS)

# DO NOT DELETE THIS LINE -- make depend depends on it.
