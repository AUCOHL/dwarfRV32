/*
extern int printf(const char* format, ...);
extern char* malloc(int size);
extern char* strcpy(char* dest, const char* src);
extern long time();
*/

int main(){
	//display
	printf("Test start:\n\n");
	int x = -3;
	printf("time = %d\n", time());
	char name[] = "Sum = ";
	for (int i = 0; i < 5; i++)
		printf("The %s%d\n", name, i+x);
	printf("\n\n\n");

	//mem
	x = 12;
	char* p = malloc(x+1);
	if (!p)
		return -1;
	for (int i = 0; i < x-1; i++)
		p[i] = 'a' + i +1;
	p[x-1] = '\0';
	printf ("string: %s\n",p);

	printf("time = %d\n", time());
	//strcpy

	char* q = malloc(x+1);
	printf ("string before: %s\n",q);
	strcpy(q, p);
	printf ("string after : %s\n",q);

	printf("strcmp(%s, %s) = %d\n", p, q, strcmp(p,q));
	q[0] = 'a';
	printf("strcmp(%s, %s) = %d\n", p, q, strcmp(p,q));

	printf("time = %d\n", time());

	return 0;

}

