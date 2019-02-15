#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[]){
  char **toUpArgv;
  int i;

  toUpArgv=malloc(argc*sizeof(char *));
  for (i=0;argv[i]!=(char *)0;i++){
    toUpArgv[i]=malloc(sizeof(char)*(strlen(argv[i])+1));
    strcpy(toUpArgv[i],argv[i]);
    printf("Copy is <%s>\n",toUpArgv[i]);
  }
}
