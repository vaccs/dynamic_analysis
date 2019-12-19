# $Id: Makefile,v 1.7 1997/03/27 20:27:20 carr Exp $
SRCS		= sd.c

CC = gcc

OBJS = $(addsuffix .o,$(basename $(SRCS)))

ENV = -g

INCLUDES=  \
	-I.

CFLAGS	= $(INCLUDES) $(ENV) -fPIC

RM = /bin/rm -f

.SILENT:

DEPENDS = $(addprefix .d_, $(basename $(SRCS)))

.SUFFIXES: .c

target: $(OBJS)
		echo "Done."

.PHONY: clean

clean:
		$(RM) $(OBJS) $(DEPENDS)

.c.o:
		echo "Compiling" $<
		$(CC) -c $(CFLAGS) $<

.d_%: %.c
	echo  "Updating dependences for" $< "..."
	$(CPP) -MM -MT '$(basename $<).o' $(INCLUDES) -MF $@ $<

include $(DEPENDS)

# DO NOT DELETE THIS LINE -- make depend depends on it.
