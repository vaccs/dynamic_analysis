/*
 * vaccs_record.h
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#ifndef vaccs_record_h
#define vaccs_record_h

#include <cstdio>
#include <climits>

#include <linux/limits.h>

#include <util/general.h>

#include <pin.H>

/***************************************************************
 * Constants used by the analysis routines
 ***************************************************************/

#define VACCS_INVALID_REC					/* an invalid record type */
#define VACCS_ARCH 0               		 	/* Information regarding target architecture */
#define VACCS_ASM 1                  	 	/* A line of assembly code */
#define VACCS_BINARY 2                   	/* Path to the executable */
#define VACCS_CCODE 3                    	/* Path to a C file */
#define VACCS_CMD_LINE 4              	 	/* The command line for an execution */
#define VACCS_DATA_REC 5              	 	/* The data records section of the file */
#define VACCS_EVENT_REC 6             	 	/* The event records section of the file */
#define VACCS_FUNCTION_INV 7       		 	/* A function invocation event */
#define VACCS_OUTPUT 8                    	/* A line of C ouptut */
#define VACCS_REGISTER 9                  	/* A register update event */
#define VACCS_RETURN 10                   	/* A function return event */
#define VACCS_RETURN_ADDR 11          	 	/* The return address of a function invocation */
#define VACCS_SD_CLEAR 12                 	/* A sensitive data clear event */
#define VACCS_SD_CZERO 13              	 	/* A sensitive data set core size to 0 event */
#define VACCS_SD_LOCK 14                  	/* A sensitive data mlock event */
#define VACCS_SD_SET 15                		/* A sensitive data mset event */
#define VACCS_SD_UNLOCK 16                	/* A sensitive data munlock event */
#define VACCS_SECTION 17                  	/* A data section in the binary */
#define VACCS_VAR_ACCESS 18         		/* A variable access event */
#define VACCS_MALLOC 19									/* a call to malloc() */
#define VACCS_FREE 20									/* a call to free() */

#define VACCS_MAX_RECORD_TYPE_ID USHRT_MAX  /* The maximum number of record type ids */

#define VACCS_ARCH_I386 0                 	/* 32-bit x86 architecture  */
#define VACCS_ARCH_X86_64 1               	/* 64-bit x86 architecture */

#define VACCS_MAX_ARCH_TYPE UCHAR_MAX     		/* The maximum number of architectures */
#define VACCS_MAX_VARIABLE_LENGTH CHAR_MAX		/* The maximum length of a variable name */
#define VACCS_MAX_SRC_LINE_LENGTH UCHAR_MAX		/* The maximum length of a line of C source */
#define VACCS_MAX_OUTPUT_LENGTH UCHAR_MAX		/* The maximum length of a line of output */
#define VACCS_MAX_REGISTER_NAME_LENGTH 16		/* The maximum length of a variable name */
#define VACCS_MAX_VALUE_LENGTH USHRT_MAX		/* The maximum length of a value */

/**********************************************************
 * Structures and types to hold the analysis data
 * Do not use unsigned types because Java does not support
 * all of the unsigned types and this information will be
 * read by a visualization system written in Java
 **********************************************************/

typedef short vaccs_id_t; /* the type of a record id */
typedef char vaccs_arch_t; /* the type of an architecture id */
typedef short vaccs_line_num_t; /* the line number type */
typedef short vaccs_line_pos_t; /* the position within a line for a C statement */
typedef Generic vaccs_address_t; /* the type of a memory address */
typedef Generic vaccs_event_num_t; /* the id of an event for visualization */
typedef Generic vaccs_value_t; /* the type of a value */

/**
 * Class: vaccs_record
 *
 * The base class for vaccs information for the visualization
 *
 */
class vaccs_record {

protected:
	vaccs_id_t id;

public:
	vaccs_record(vaccs_id_t id);
	virtual ~vaccs_record() {
	}

	/**
	 * Write an analysis record to a file
	 *
	 * @param fp the file pointer for the output file
	 * @param rec the analysis record to be written to the file
	 */
	virtual void write(NATIVE_FD fp) = 0;

	/**
	 * Read an analysis record from a file
	 *
	 * @param fp the file pointer for the input file
	 * @return a pointer to an analysis record read from the file.
	 */
	virtual vaccs_record *read(NATIVE_FD fp) = 0;

	/**
	 * Set the id of this record
	 *
	 * @param id the type of this record
	 */
	void set_id(vaccs_id_t id) { this->id = id; }

	/**
	 * Get the id of this record
	 *
	 * @return the id for this record indicating its type
	 */
	vaccs_id_t get_id() { return id; }


};
#endif /* vaccs_record_h */
