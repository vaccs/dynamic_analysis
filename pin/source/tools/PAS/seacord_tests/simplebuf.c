#include <stdlib.h>

int main(){
  char *bufPtr=0;
  //  char  cArr[10];

  int i;
  /*
  for (i=0;i<9;i++){
    cArr[i]=97+i;
    }
  cArr[9]=0;
  */
   bufPtr=malloc(10);
    for (i=0;i<10;i++){
    bufPtr[i]=65+i;
    }
  
    return(0);
}
