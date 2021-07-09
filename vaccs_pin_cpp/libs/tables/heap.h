/*
 * =====================================================================================
 *
 *       Filename:  heap.h
 *
 *    Description:  These classes keep track of the blocks allocated on the heap so that
 *                  we can search to see if any of them have been accessed
 *
 *        Version:  1.0
 *        Created:  06/07/2019 11:17:44 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Steve Carr
 *        Company:  Western Michigan University
 *
 * =====================================================================================
 */

#ifndef heap_h
#define heap_h

#include <map>
#include <string>
#include <list>

#include <util/general.h>
#include <io/vaccs_record.h>

using namespace std;
/*
 * =====================================================================================
 *        Class:  heap_block
 *  Description:  This class represents a block of memory allocated on the heap. We just
 *                record the start address and size for checking if a memory access is to
 *                the heap.
 * =====================================================================================
 */
class heap_block
{
public:
	/* ====================  LIFECYCLE     ======================================= */
	heap_block();                                /* constructor */

	virtual ~heap_block() {
		if (value != NULL) delete value;
	}
	/* ====================  BUILDERS      ======================================= */

	heap_block *add_start_address(Generic sa)
	{
		start_address = sa;
		return this;
	}

	heap_block *add_size(Generic s)
	{
		size = s;
		return this;
	}

	heap_block *add_event_id(unsigned int i)
	{
		event_id = i;
		return this;
	}

	heap_block *add_value()
	{
		value = new char[size];
		memcpy(value, (const void*)start_address, size);
		return this;
	}

	void update_value()
	{
		memcpy(value, (const void*)start_address, size);
	}

	/* ====================  ACCESSORS     ======================================= */

	Generic get_start_address()
	{
		return start_address;
	}

	Generic get_size()
	{
		return size;
	}

	unsigned int get_event_id()
	{
		return event_id;
	}

	char *get_value()
	{
		char *buff = new char[size];

		memcpy(buff, value, size);
		return buff;
	}

	/* ====================  OPERATORS     ======================================= */

	bool contains_address(Generic address)
	{
		return address >= start_address && address < start_address + size;
	}

	bool address_earlier(Generic address)
	{
		return address < start_address;
	}

	bool mem_has_new_value()
	{
		return memcmp(value, (const void*)start_address, get_size()) != 0;
	}

	/* ====================  DATA MEMBERS  ======================================= */
protected:

private:
	Generic start_address;
	Generic size;
	char *value;
	unsigned int event_id;

}; /* -----  end of class heap_block  ----- */


/*
 * =====================================================================================
 *        Class:  heap_map
 *  Description:  This class represents all of the blocks allocated on the heap.
 * =====================================================================================
 */
class heap_map: public map < Generic, heap_block * >
{
private:
	list<heap_block*> freed_list;

public:
	/* ====================  LIFECYCLE     ======================================= */
	heap_map(string map_file_name);                                /* constructor */

	virtual ~heap_map() {
	}

	/* ====================  BUILDERS     ======================================= */

	heap_map *add_block(heap_block *block)
	{
		insert(pair < Generic, heap_block * > (block->get_start_address(), block));
		return this;
	}

	/* ====================  ACCESSORS     ======================================= */

  vaccs_address_t get_heap_start() {
    return heap_start;
  }

  vaccs_address_t get_heap_end() {
    return heap_end;
  }

	/* ====================  MUTATORS      ======================================= */

	/* ====================  OPERATORS     ======================================= */

	heap_block *find_block(Generic address);
	heap_block *find_freed_block(Generic address);
	heap_block *delete_block(Generic address);
	void find_garbage_blocks();


	/* ====================  DATA MEMBERS  ======================================= */
protected:

private:
  vaccs_address_t heap_start;
  vaccs_address_t heap_end;

}; /* -----  end of class heap_map  ----- */

#endif
