/**
 *
 * Class for maintaining information about the memory address information
 * for the heap and stack
 *
 * Author: Steve Carr
 */

 #include <fstream>
 #include <sstream>

 #include <unistd.h>
 #include <util/memory_info.h>

 using namespace std;

memory_info::memory_info (string map_file_name) {
  stack_begin = 0;
  stack_end = 0;
  heap_begin = 0;
  heap_end = 0;
  if (!read_stack_map(map_file_name))
    read_stack_map(MEMORY_MAP_FILE);
  else {
    DEBUGL(LOG("Found stack information\n"));
  }
  
  if (!read_heap_map(map_file_name))
    if (!read_heap_map(MEMORY_MAP_FILE)) {
	    heap_begin = (ADDRINT)sbrk(0); // default value 
	    heap_end = stack_end - sizeof(Generic);
    } else {
      DEBUGL(LOG("Found heap information\n"));
    }
  else {
    DEBUGL(LOG("Found heap information\n"));
  }
}

bool memory_info::read_stack_map(string map_file_name) {

  ifstream mapfile(map_file_name.c_str());
  string line;
  bool found = false;

 DEBUGL(LOG("Reading Stack Boundaries from file " + map_file_name + "\n"));
  while (!found && getline(mapfile,line)) {
    if (line.find("[stack]") != string::npos) {
      unsigned long start,end;
      istringstream iss(line);
      char dash;
      iss >> hex >> end;
      stack_end = (ADDRINT)end; /* stack grows in negative direction */
      iss >> dash;
      iss >> hex >> start;
      stack_begin = (ADDRINT)start;
      found = true;
    }
  }
   mapfile.close();
   return found;
}

bool memory_info::read_heap_map(string map_file_name) {

  ifstream mapfile(map_file_name.c_str());
  string line;
  bool found = false;

 DEBUGL(LOG("Reading Heap Boundaries from file " + map_file_name + "\n"));
  while (!found && getline(mapfile,line)) {
    if (line.find("[heap]") != string::npos) {
      unsigned long start,end;
      istringstream iss(line);
      char dash;
      iss >> hex >> start;
      heap_begin = (ADDRINT)start;
      iss >> dash;
      iss >> hex >> end;
      heap_end = (ADDRINT)end;
      found = true;
    }
  }
   mapfile.close();
   return found;
}
