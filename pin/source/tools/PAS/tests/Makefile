# $Id: Makefile,v 1.7 1997/03/27 20:27:20 carr Exp $
VACCSSD_DIR = /home/carr/vaccs/dynamic_analysis/secure_data
VACCSSD_OBJ = $(VACCSSD_DIR)/sd.o

CC = gcc

ENV = -g

INCLUDES=  \
	-I. \
	-I$(VACCSSD_DIR)

CFLAGS	= $(INCLUDES) $(ENV) -fno-stack-protector

RM = /bin/rm -f

.SILENT:

.SUFFIXES: .c

sd1: sdtest1.o $(VACCSSD_OBJ)
		$(CC) $(CFLAGS) -o $@ $^
		echo $@ " Created"

.PHONY: clean

clean:
		$(RM) sdtest1.o

.c.o:
		echo "Compiling" $<
		$(CC) -c $(CFLAGS) $<
