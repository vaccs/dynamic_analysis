/**
 *
 * Class for maintaining information about the memory address information
 * for the heap and stack
 *
 * Author: Steve Carr
 */

 #include <fstream>
 #include <sstream>

 #include <util/memory_info.h>

 using namespace std;

memory_info::memory_info () {
  stack_begin = 0;
  stack_end = 0;
  heap_begin = 0;
  heap_end = 0;
}

void memory_info::read_stack_map() {

  ifstream mapfile(MEMORY_MAP_FILE);
  string line;

  while (getline(mapfile,line)) {
    if (line.find("[stack]") != string::npos) {
      unsigned long start,end;
      istringstream iss(line);
      char dash;
      iss >> hex >> start;
      stack_begin = (ADDRINT)start;
      iss >> dash;
      iss >> hex >> end;
      stack_end = (ADDRINT)end;
      break;
    }
  }
   mapfile.close();
}

void memory_info::read_heap_map() {

  ifstream mapfile(MEMORY_MAP_FILE);
  string line;

  while (getline(mapfile,line)) {
    if (line.find("[heap]") != string::npos) {
      unsigned long start,end;
      istringstream iss(line);
      char dash;
      iss >> hex >> start;
      heap_begin = (ADDRINT)start;
      iss >> dash;
      iss >> hex >> end;
      heap_end = (ADDRINT)end;
      break;
    }
  }
   mapfile.close();
}
