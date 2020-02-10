
int main(){
  short int s;
  int i;
  long int l;
  long long int ll ;
  short unsigned int us;
  unsigned int ui;
  long unsigned int ul;
  long long unsigned int ull;

  float f;

  char buf[7]="abc123";
  
  s=32767;
  i=2147483647;
  l=9223372036854775807L;
  ll=9223372036854775807LL;
  us=65535;
  ui=4294967295U;
  ul=18446744073709551615UL;
  ull=18446744073709551615ULL;

  s++;
  i++;
  l++;
  ll++;
  us++;
  ui++;
  ul++;
  ull++;
  
  return(0);
}
