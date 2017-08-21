int Fibonacci(int);
 
int main()
{
   int sum = 0; 
 
   for ( int c = 1 ; c <= 10 ; c++ )
   {
      sum += Fibonacci(c);
       
   }
 
   return 0;
}
 
int Fibonacci(int n)
{
   if ( n == 0 )
      return 0;
   else if ( n == 1 )
      return 1;
   else
      return ( Fibonacci(n-1) + Fibonacci(n-2) );
} 
