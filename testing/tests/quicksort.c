#define MAX 7

int intArray[MAX] = {4,6,3,2,1,9,7};

void swap(int* a, int* b)
{
 int temp = *a;
 *a = *b;
 *b = temp;
}

int partition(int left, int right, int pivot) {
   int leftPointer = left -1;
   int rightPointer = right;

   while(1) {
      while(intArray[++leftPointer] < pivot);
		
      while(rightPointer > 0 && intArray[--rightPointer] > pivot);

      if(leftPointer >= rightPointer)
         break;
      else
         swap(&leftPointer,&rightPointer);
   }
	
   return right;
}

void quickSort(int left, int right) {
   if(right-left <= 0)
      return;   
   else {
      int pivot = intArray[right];
      int partitionPoint = partition(left, right, pivot);
      quickSort(left,partitionPoint-1);
      quickSort(partitionPoint+1,right);
   }        
}

int main() {
   quickSort(0,MAX-1);

   return intArray[0];
}
