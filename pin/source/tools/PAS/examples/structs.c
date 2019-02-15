//
//--- Illustrate layout of struct in memory
//
#include <stdlib.h>
#include <string.h>
struct simple {
  char buf1; 
  int  i;
  char buf2[3];
  char *cptr;  
};
int main(int argc, char *argv[]){
  struct simple s;
  s.buf1=argv[0][1];
  s.i=argc;
  s.buf2[0]='a';
  s.buf2[1]='b';
  s.buf2[2]='c';
  s.cptr=malloc(strlen(argv[0])+1);
  strcpy(s.cptr,argv[0]);
}
