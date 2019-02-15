#include <stdio.h>
#include <string.h>

#define MAX_LEN 8

typedef struct _vulnerable_struct {
  char buff[MAX_LEN]; 
  int (*cmp)(char*,char*);   /* Function pointer, takes 2 char* args*/
} vulnerable;

/* Let users specify their own comparison func.  Like sort */
int chkStr(vulnerable* s, char* one, char* two, char *three ) {
   strcpy( s->buff, one );                /* Copy one to s->buff until NULL*/
   strcat( s->buff, two );
   return s->cmp( s->buff, three );       /* Run function via pointer*/
}
int main(int argc, char *argv[]) {
  vulnerable v;
  int notMatch;

  v.cmp=&strcmp;                /* Will use strcmp as comparison function */
  notMatch=chkStr(&v,argv[1],argv[2],argv[3]);
  if (notMatch) printf("Not match\n");else printf("Match\n");
}


