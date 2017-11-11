int sum(int x[], int c){
  int i;
  int s=0;
  for(i=0; i<c; i++)
    s+=x[i];
  return s;
}
int main()
{
  int y[]={10, 20, 30, 100, -50, 200, -10};
  int x = sum(y, 7);
  return x;
}
