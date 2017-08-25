#define size 9

unsigned int next = 1;
int rand() { //not so random
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % 32768;
}


int maxSubArraySum(int a[])
{
    int max_so_far = 0xffffffff, max_ending_here = 0, i;
 
    for (i = 0; i < size; i++){
        max_ending_here = max_ending_here + a[i];
        if (max_so_far < max_ending_here)
            max_so_far = max_ending_here;
 
        if (max_ending_here < 0)
            max_ending_here = 0;
    }
    return max_so_far;
}
 

int main()
{
	int x[size];
    for (int i = 0; i < size; i++)
        x[i] = rand()%1001-500;

    return maxSubArraySum(x);
}
