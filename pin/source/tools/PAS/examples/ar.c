#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//
//-- Illustrate layout of activation record, overflows (using argv[1])
// 
//
int simpleFunction(int iparm, char *cpparm){
   char  lbuf1[3];
   int   localInt;
   char  lbuf2[8];
   char  *lbuf3;

   strcpy(lbuf1,cpparm);
   localInt=iparm;
   strcpy(lbuf2,cpparm);
   lbuf3=malloc(8);
   strcpy(lbuf3,cpparm);

   return(0);
}
int main(int argc, char **argv){
  int x;
  simpleFunction(argc,argv[1]);
  x=1;
  printf("Done\n");
}
