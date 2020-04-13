#include <string.h>
#include <stdio.h>

void func(char *cptr){
  int i;
  char buf[8];
  int j;

  i=7;
  j=11;
  strcpy(buf,cptr);
  i=0;
  j=0;
}
int main(int argc, char *argv[]){
  printf("argv[0] = %s\n",*argv);
  func(argv[1]);
}
