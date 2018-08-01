/*
 * cmd_line_record.h
 *
 *  Created on: Jul 2, 2018
 *      Author: carr
 */

#ifndef LIBS_IO_CMD_LINE_RECORD_H_
#define LIBS_IO_CMD_LINE_RECORD_H_

#include <io/vaccs_record.h>

#include <util/c_string_utils.h>

class cmd_line_record: public vaccs_record {

private:
	char *cmd_line; /* a line of C output */

public:
	cmd_line_record();
	virtual ~cmd_line_record();

	cmd_line_record *add_cmd_line(const char *cmd_line) {
		this->cmd_line = ssave(cmd_line);
		return this;
	}

	const char *get_cmd_line() {
		return cmd_line;
	}
	/**
	 * Write an analysis record to a file
	 *
	 * @param fp the file pointer for the output file
	 * @param rec the analysis record to be written to the file
	 */
	virtual void write(FILE *fp);

	/**
	 * Read an analysis record to a file
	 *
	 * @param fp the file pointer for the output file
	 */
	virtual vaccs_record *read(FILE *fp);
};

#endif /* LIBS_IO_CMD_LINE_RECORD_H_ */
