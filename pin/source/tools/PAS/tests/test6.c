#include <stdio.h>
#include <string.h>
int main(int argc, char **argv) {

   char a[8];

   printf("&argv = %p\n",argv);
   strcpy(a,argv[1]);   
   printf("a = %s\n",a);

}
