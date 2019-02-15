//
//-- Illustrate overwriting the return address with address of code that
//   will execute
//
#include <stdio.h>

void doBadThings(){
  printf("Do something under attacker control\n");
  _exit(0);
}
void simpleFunc(int parm){   
  int *stackPtr;
  stackPtr=&parm;
  printf("Return address is: <%x>\n",*(stackPtr-1));
  *(stackPtr - 1)=doBadThings;
  printf("Return from simpleFunc now\n");
}
main(){
  simpleFunc(1);
}
