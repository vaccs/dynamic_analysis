// 
//-- Illustrate canary design.
//
#include <stdio.h>
#include <stdlib.h>
int aGlobal; /* A global variable */
int simple(){
  printf("Simple returning 13\n");
  return(13);
}
int aFunction(int intParm, int (*ptrParm)()){
  int localInt;
  char numBuf[10];
  aGlobal=intParm;
  puts("Enter a number with 8 digits or less: ");
  gets(numBuf);
  localInt=atoi(numBuf)+ptrParm()+intParm;
  if (aGlobal!=intParm) exit(1);
  return(localInt+intParm);
}
int main(int argc, char *argv[]){
  int ret;
  ret=aFunction(5,simple);
  printf("ret is <%d>\n",ret);
}
