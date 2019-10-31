int a;

void modify(int a, int b, int c, int d , int e) {

   a = a + b + c + d + e;
}

int main() {

   int b;
   int *p = &b;

   
   a = 1;
   b = 2;
   modify(a,b,a,b,a);
}
