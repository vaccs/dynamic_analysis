/*
 * dwread.cpp
 *
 *  Created on: Jul 16, 2018
 *      Author: carr
 */
#include <cstdio>
#include <read/dw_reader.h>

int main(int argc, char **argv) {

	if (argc < 2) {
		fprintf(stderr, "Usage: dwread <filename>\n");
		exit(-1);
	}

	std::string fn(argv[1]);

	dw_reader *dwr = (new dw_reader())->add_file_name(fn);

	dwr->read_dw_info();

	printf("Done reading\n");

	delete dwr;
}

