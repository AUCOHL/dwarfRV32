#define N 8
int row[N], count = 0;

int abs (int a){
  return a > 0? a : -a;
}

int canPlace(int r, int c){
  for (int i = 0; i < c; i++)
    if (row[i] == r || abs(row[i]-r) == abs(i-c))
      return 0;
  return 1;
}

void backtrack (int c){
  if (c == N){
    count++;
    return;
  }

  for (int r = 0; r < N; r++)
    if (canPlace(r,c)){
      row[c] = r;
      backtrack(c+1);
    }
}

int main (){
  backtrack(0);
  int A[1] = {count};
  return A[0];
}
