void timer(){
    int *ptr = (int *)(1024 * 20);
    *ptr += 1;
    int time = 100;

    asm volatile("csrrw x0, 65, %0"
                :
                :"r" (100));
}

int main()
{
    int *ptr = (int *)(1024 * 20);
    *ptr = 0;

    timer();

    while(1);
}
