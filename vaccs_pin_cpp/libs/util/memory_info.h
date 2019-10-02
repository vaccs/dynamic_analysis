/*
 * Class: memory_info

 * This class reads the file /proc/self/maps to find the beginning and end points of the
 * stack and heap. ASLR must be turned off for this to work.
 *
 */

#include <pin.H>

#define MEMORY_MAP_FILE "/proc/self/maps"

class memory_info {

private:

  ADDRINT stack_begin;
  ADDRINT stack_end;
  ADDRINT heap_begin;
  ADDRINT heap_end;

public:

    memory_info();
    virtual ~memory_info() {}

    /**
     * Get the stack begin address
     *
     * @return the start address of the stack
     */
     ADDRINT get_stack_begin() {
       if (stack_begin == 0)
         read_stack_map();
       return stack_begin;
     }

     /**
     * Get the stack end address
     *
     * @return the end address of the stack
     */
     ADDRINT get_stack_end() {
       if (stack_end == 0)
         read_stack_map();
       return stack_end;
     }

     /**
     * Get the heap begin address
     *
     * @return the start address of the heap
     */
     ADDRINT get_heap_begin() {
       if (heap_begin == 0)
         read_heap_map();
       return heap_begin;
     }

     /**
     * Get the heap end address
     *
     * @return the end address of the heap
     */
     ADDRINT get_heap_end() {
       if (heap_end == 0)
         read_heap_map();
       return heap_end;
     }

     /**
      * Read the stack address information from MEMORY_MAP_FILE
      */
     void read_stack_map();

     /**
      * Read the heap address information from MEMORY_MAP_FILE
      */
     void read_heap_map();
};
