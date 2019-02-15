//
//-- Illustrate use of character pointers as parameters
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char *changeFirst(char *inBuf){
  char *mod;

  mod=malloc(strlen(inBuf)+1);
  inBuf=mod;
  inBuf[0]='A';
  return(mod);
}
int main(int argc, char *argv[]){
  char buf[]="012345";
  char *mbuf;
  mbuf=changeFirst(buf);
  printf("buf is <%s>\n",buf);
  printf("mbuf is <%s>\n",mbuf);
}
