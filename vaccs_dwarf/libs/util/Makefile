# $Id: Makefile,v 1.7 1997/03/27 20:27:20 carr Exp $
SRCS		= general.cpp c_string_utils.cpp 

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
		$(CXX) -c $(CFLAGS) $<

.d_%: %.c 
	echo  "Updating dependences for" $< "..."
	$(CPP) -MM -MT '$(basename $<).o' $(INCLUDES) -MF $@ $<

.d_%: %.cpp 
	echo  "Updating dependences for" $< "..."
	$(CPP) -MM -MT '$(basename $<).o' $(INCLUDES) -MF $@ $<

include $(DEPENDS)

# DO NOT DELETE THIS LINE -- make depend depends on it.
