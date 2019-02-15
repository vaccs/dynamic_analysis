#include <stdio.h>
#include <limits.h>
int main(){
unsigned short int x1;
unsigned int ui1;
ui1=UINT_MAX;
x1=ui1;
printf("x1 is %hu unsigned short max is <%hu> \n",x1,USHRT_MAX);

short int x2;
int i2;
i2=INT_MIN;
x2=i2;
printf("x2 is %hd short min is <%hd> \n",x2,SHRT_MIN);


short int x3;
x3=SHRT_MAX;
x3=x3+3;
printf("x3 is %hd short max is <%hd> \n",x3,SHRT_MAX);

int y4;
unsigned int x4;
y4=-1;
x4=y4;
printf("x4 is %u \n",x4);

short int x5;
unsigned int ui5;
ui5=UINT_MAX;
x5=ui5;
printf("x5 is %d\n",x5);

}
