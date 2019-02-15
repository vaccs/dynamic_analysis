#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
char *someFunc(int fd){
  char buf[10];
  int i=0;
  while ((read(fd,&buf[i],1)==1)&&(i<10)) i++;
  return(&buf);
}
int main(int argc, char *argv[]){
  int fd;
  char *readValue;
  fd=open(argv[1],O_RDONLY);
  if (fd<0)exit(1);
  readValue=someFunc(fd);
  printf("readValue is <%p> string is <%s>\n",readValue,readValue);
  close(fd);
}
