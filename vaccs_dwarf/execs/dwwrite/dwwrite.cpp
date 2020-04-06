/*
 * dwread.cpp
 *
 *  Created on: Jul 16, 2018
 *      Author: carr
 */
#include <cstdio>
#include <cassert>
#include <read/dw_reader.h>
#include <util/c_string_utils.h>

int main(int argc, char **argv) {

	if (argc < 2) {
		fprintf(stderr, "Usage: dwwrite <filename>\n");
		exit(-1);
	}

	std::string fn(argv[1]);

	dw_reader *dwr = (new dw_reader())->add_file_name(fn);

	dwr->read_dw_info();

	char *vfn = nssave(2,argv[1],".vdw");

	FILE *fp;
	assert((fp = fopen(vfn,"w")) != NULL);

	dwr->get_cutab()->write(fp);

	fclose(fp);

	delete dwr;
}

