/*
 * Class: memory_info

 * This class reads the file /proc/self/maps to find the beginning and end points of the
 * stack and heap. ASLR must be turned off for this to work.
 *
 */

#include <pin.H>
#include <util/general.h>

#define MEMORY_MAP_FILE "/proc/self/maps"

class memory_info {

private:

  ADDRINT stack_begin;
  ADDRINT stack_end;
  ADDRINT heap_begin;
  ADDRINT heap_end;


  /**
   * Read the stack address information
   * 
   * @param map_file_name the name of the proc file containing information
   * @return true if information is found in the file; otherwise, false
   */
  bool read_stack_map(string map_file_name);

  /**
   * Read the heap address information
   * 
   * @param map_file_name the name of the proc file containing information
   * @return true if information is found in the file; otherwise, false
   */
  bool read_heap_map(string map_file_name);

public:

    memory_info(string map_file_name);
    virtual ~memory_info() {}

    /**
     * Get the stack begin address
     *
     * @return the start address of the stack
     */
     ADDRINT get_stack_begin() {
       return stack_begin;
     }

     /**
     * Get the stack end address
     *
     * @return the end address of the stack
     */
     ADDRINT get_stack_end() {
       return stack_end;
     }

     /**
     * Get the heap begin address
     *
     * @return the start address of the heap
     */
     ADDRINT get_heap_begin() {
       return heap_begin;
     }

     /**
     * Get the heap end address
     *
     * @return the end address of the heap
     */
     ADDRINT get_heap_end() {
       return heap_end;
     }
};
