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

heap_block::heap_block() {
   start_address = 0;
   value = NULL;
   size = 0;
   event_id = -1;
}

heap_map::heap_map() : map() {}

heap_block *heap_map::find_block(Generic address) {

   heap_block *block = NULL;

   for (map<Generic,heap_block*>::iterator it = begin(); it != end(); it++) {

      heap_block *hb = it->second;
      if (hb->contains_address(address)) {
         block = hb;
	 break;
      } else if (hb->address_earlier(address))
         break;
   }

   return block;
}

heap_block *heap_map::delete_block(Generic address) {

   heap_block *block = find_block(address);

   for (map<Generic,heap_block*>::iterator it = begin(); it != end(); it++) {

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

