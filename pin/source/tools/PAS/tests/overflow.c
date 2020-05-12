#include <stdio.h>

extern void func(char* cptr);

int main(int argc, char *argv[]){
  printf("argv[0] = %s\n",*argv);
  func(argv[1]);
}
