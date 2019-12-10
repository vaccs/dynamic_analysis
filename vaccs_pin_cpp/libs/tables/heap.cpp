/*
 * =====================================================================================
 *
 *       Filename:  heap.cpp
 *
 *    Description:  These routine access the heap data structure for monitoring
 *                  heap accesses
 *
 *        Version:  1.0
 *        Created:  06/07/2019 11:41:16 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Steve Carr
 *        Company:  Western Michigan University
 *
 * =====================================================================================
 */

#include    <tables/heap.h>
#include    <string>
#include  <fstream>
#include  <sstream>
#include  <iostream>
#include  <unistd.h>
#include  <limits.h>

#include <pin.H>

heap_block::heap_block()
{
	start_address = 0;
	value = NULL;
	size = 0;
	event_id = -1;
}

heap_map::heap_map(string map_file_name) : map()
{
	ifstream map_file(map_file_name.c_str());
	string line;

	heap_start = (vaccs_address_t)sbrk(0); // default value 
	heap_end = heap_start + INT_MAX; // default value
	if (!map_file.good()) {
		DEBUGL(LOG("Cannot access " + map_file_name + ", using defaults\n"));
	} else {
		DEBUGL(LOG("Reading Heap Boundaries from file " + map_file_name + "\n"));
		while (getline(map_file, line)) {
			if (line.find("[heap]") != string::npos) {
				istringstream iss(line);
				char dash;
				if (!(iss >> heap_start >> dash >> heap_end) || (dash != '-'))
					cerr << "Error reading memory map file " << map_file_name << "\n";
				DEBUGL(LOG("Heap start = " + hexstr(heap_start) + ", heap end = " + hexstr(heap_end) + "\n"));
				break;
			}
		}
	}
	map_file.close();
}

heap_block *heap_map::find_block(Generic address)
{

	heap_block *block = NULL;

	for (map<Generic, heap_block*>::iterator it = begin(); it != end(); it++) {

		heap_block *hb = it->second;
		if (hb->contains_address(address)) {
			block = hb;
			break;
		} else if (hb->address_earlier(address))
			break;
	}

	return block;
}

heap_block *heap_map::delete_block(Generic address)
{

	heap_block *block = find_block(address);

	for (map<Generic, heap_block*>::iterator it = begin(); it != end(); it++) {

		heap_block *hb = it->second;
		if (hb->contains_address(address)) {
			block = hb;
			erase(it);
			break;
		} else if (hb->address_earlier(address))
			break;
	}

	return block;
}
