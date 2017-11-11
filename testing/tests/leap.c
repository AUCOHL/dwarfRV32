int leap(int year)
{
if (((year % 4 == 0) && (year % 100!= 0)) || (year%400 == 0))
   return 1;
else

   return 0;
}
int main() {
   int year;
   year = 2016;
   leap(year);
   return 0;
}
