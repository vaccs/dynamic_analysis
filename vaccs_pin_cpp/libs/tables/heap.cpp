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

HeapBlock::HeapBlock() {
   start_address = 0;
   value = NULL;
   size = 0;
   event_id = -1;
}

HeapMap::HeapMap() : map() {}

HeapBlock *HeapMap::find_block(Generic address) {

   HeapBlock *block = NULL;

   for (map<Generic,HeapBlock*>::iterator it = begin(); it != end(); it++) {

      HeapBlock *hb = it->second;
      if (hb->contains_address(address)) {
         block = hb;
	 break;
      } else if (hb->address_earlier(address))
         break;
   }

   return block;
}

