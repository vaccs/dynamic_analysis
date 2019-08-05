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

HeapBlock::HeapBlock() {
   start_address = 0;
   size = 0;
}

HeapMap::HeapMap() : list() {}

HeapBlock *HeamMap::find(Generic address) {

   HeapBlock *block = NULL;

   for (list<HeapBlock*>::iterator it = begin(); it != end() && block == NULL; it++) {

      HeapBlock *hb = *it;
      if (hb->contains_address(address))
         block = hb;
   }

   return block;
}

