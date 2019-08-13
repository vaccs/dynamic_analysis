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

#include    <map>

#include <util/general.h>

using namespace std;
/*
 * =====================================================================================
 *        Class:  HeapBlock
 *  Description:  This class represents a block of memory allocated on the heap. We just
 *                record the start address and size for checking if a memory access is to
 *                the heap.
 * =====================================================================================
 */
class HeapBlock
{
   public:
      /* ====================  LIFECYCLE     ======================================= */
      HeapBlock ();                             /* constructor */

      /* ====================  BUILDERS      ======================================= */

      HeapBlock *add_start_address(Generic sa) {
         start_address = sa;
         return this;
      }

      HeapBlock *add_size(Generic s) {
         size = s;
         return this;
      }

      HeapBlock *add_event_id(unsigned int i) {
         event_id = i;
         return this;
      }

      HeapBlock *add_value() {
         value = new char[size];
         memcpy(value,(const void *)start_address,size);
         return this;
      }

      void update_value() {
         memcpy(value,(const void *)start_address,size);
      }

      /* ====================  ACCESSORS     ======================================= */

      Generic get_start_address() {
         return start_address;
      }

      Generic get_size() {
         return size;
      }

      unsigned int get_event_id() {
         return event_id;
      }

      const char *get_value() {
         char *buff = new char[size];
         memcpy(buff,value,size);
         return buff;
      }

      /* ====================  OPERATORS     ======================================= */

      bool contains_address(Generic address) {
         return address >= start_address && address < start_address+size;
      }

      bool address_earlier(Generic address) {
	 return address < start_address;
      }

      bool mem_has_new_value() {
         return memcmp(value,(const void *)start_address,size() != 0;
      }

      /* ====================  DATA MEMBERS  ======================================= */
   protected:

   private:
      Generic start_address;
      Generic size;
      char *value;
      unsigned int event_id;

}; /* -----  end of class HeapBlock  ----- */


/*
 * =====================================================================================
 *        Class:  HeapMap
 *  Description:  This class represents all of the blocks allocated on the heap.
 * =====================================================================================
 */
class HeapMap : public map<Generic, HeapBlock *>
{
   public:
      /* ====================  LIFECYCLE     ======================================= */
      HeapMap ();                             /* constructor */

      /* ====================  BUILDERS     ======================================= */

      HeapMap *add_block(HeapBlock *block) {
	 insert(pair<Generic,HeapBlock*>(block->get_start_address(),block));
         return this;
      }

      /* ====================  ACCESSORS     ======================================= */

      /* ====================  MUTATORS      ======================================= */

      /* ====================  OPERATORS     ======================================= */

      HeapBlock *find_block(Generic address);
      HeapBlock *delete_block(Generic address);


      /* ====================  DATA MEMBERS  ======================================= */
   protected:

   private:

}; /* -----  end of class HeapMap  ----- */

#endif
