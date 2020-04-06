# $Id: Makefile,v 1.2 1997/03/27 20:19:38 carr Exp $

DIRS = \
	    secure_data \
	    vaccs_dwarf \
	    vaccs_pin_cpp \
	    pin/source/tools/PAS

.SILENT:

all:
		for dir in $(DIRS); do \
			echo "Making directory $$dir"; \
			make -C $$dir; \
		done

clean:
		for dir in $(DIRS); do \
			echo "Cleaning directory $$dir"; \
			$(MAKE) -C $$dir clean; \
		done
