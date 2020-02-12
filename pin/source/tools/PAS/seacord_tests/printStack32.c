#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int distance, i;
int *stackPtr;
void neverCalled(){
  printf("This should never print\n");
  int ncv = 2;
  ncv = ncv + 5;
  exit(0);
}
void func(int parm1, int parm2, int parm3){
  int  firstLocal=23;
  char sbuf[8];
  char *dbuf1;
  char *dbuf2;
  int  lastLocal=43;
  int  *returnPtr;
  
  dbuf1=malloc(8);
  dbuf2=malloc(8);
  stackPtr=&parm3;
  printf("Address of parm3 is <%p>\n",stackPtr);
  printf("Value is <%d>\n",*stackPtr);
  stackPtr++;
  printf("Address of parm2 is <%p>\n",stackPtr);
  printf("Value there is <%d>\n",*stackPtr);
  stackPtr++;
  printf("Address of parm1 is <%p>\n",stackPtr);
  printf("Value there is <%d>\n",*stackPtr);
  stackPtr++;
  printf("Return address at <%p>\n",stackPtr);returnPtr=stackPtr;
  printf("Value there is <%x>\n",(unsigned int)*stackPtr);
  stackPtr++;
  
  /* Change the value of a local through an overflow */
  printf("firstlocal at %p\n",&firstLocal);
  printf("sbuf at %p\n",&sbuf);
  printf("First local value before buffer modification is <%d>\n",firstLocal);
  for (i=0;i<12;i++)sbuf[i]=0;
  printf("First local value after buffer modification is <%d>\n",firstLocal);

  /* Change the return address.  See effect on return */
   *returnPtr=(unsigned int)neverCalled;

   /* Change a value in the heap through an overflow */
  printf("dbuf1 is <%p>\n",dbuf1);
  printf("dbuf2 is <%p>\n",dbuf2);
  for (i=0;i<7;i++)dbuf2[i]='A';				
  dbuf2[7]=0;
  printf("dbuf2 is <%s>\n",dbuf2);
  distance=(unsigned int)dbuf2-(unsigned int)dbuf1;
  distance+=8;
  for (i=0;i<(distance-1);i++)dbuf1[i]='B';
  dbuf1[distance-1]=0;
  printf("dbuf2 is <%s>\n",dbuf2);
	
}
int main(){
  func(7,17,19);
}
  
