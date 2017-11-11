#include <stdarg.h>

#define OUT_LOC 0x80000000
#define MAXHEAP 1024
#define NULL    0
#define PREC 10

extern int printf (const char* format, ...);
extern int puts(char* s);
extern int putchar(int c);
extern char* malloc(int size);
extern void *memcpy(void *dest, const void *src, long n);
extern char* strcpy (char* dest, const char* src);
extern int strcmp(const char* str1, const char* str2);
extern long time(); 
extern long insn(); 

char alloc[MAXHEAP];
int mused = 0;

long time (){
	int cycs;
	asm("rdcycle %0" : "=r"(cycs));
	return cycs;
}

long insn (){
	int insns;
	asm("rdinstret %0" : "=r"(insns));
	return insns;
}

///print
int putchar (int c){ //promoted
	*((volatile int*)OUT_LOC) = c;
	return 1;
}

static void printd (int d){
	if (d < 0){
		putchar('-');
		d = -d;
	}
	char buffer[32], *p = buffer;
	do {
		*(p++) = (char)('0' + d%10);
		d /= 10;
	} while (d);

	while (p-- != buffer)
		putchar(*p);
}

extern int puts (char* s){
	while (*s)
		putchar (*(s++));
    putchar('\n');
	return 1;
}

static void printdf(double f){
	int count = 0;
	do {
		f /= 10.0;
		count++;
	} while ((int)f);
	for (int i = 0; i < count+PREC; i++){
		f *= 10.0;
		if (i == count)
			putchar('.');
		putchar('0' + ((int)f)%10);

	}
}

int printf (const char* format, ...){
	va_list ap;
	va_start(ap,format);

	char* p = format;
	while (*p){
		if (*p == '%'){
			switch (*(++p)){
				case 'c':
					putchar(va_arg(ap,int));
					break;
				case 's':
					puts(va_arg(ap,char*));
					break;
				case 'd':
					printd(va_arg(ap,int));
					break;
				default: //f
					while(*p && *p != 'f') //skip precision
					       p++;	
					printdf(va_arg(ap,double));
			}
		}
		else putchar(*p);
		++p;
	}
	va_end(ap);
	return 1;
}

//memory
char* malloc (int size){
	char* p = alloc + mused;
	mused += size;
	if (mused > MAXHEAP)
		p = NULL;
	return p;
}


void *memcpy(void *aa, const void *bb, long n)
{
	char *a = aa;
	const char *b = bb;
	while (n--) *(a++) = *(b++);
	return aa;
}

//cstring
/*
char* strcpy (char* dest, const char* src){
	char* p = src;
	while (*p){
		*dest = *p;
		dest++, p++;
	}
	*(dest) = '\0';
	return dest;
}
*/
char* strcpy(char* dest, const char* source) {
    int i = 0;
    while ((dest[i] = source[i]) != '\0')
	    i++;
    return dest;
}

int strcmp(const char* str1, const char* str2) {
    while(*str1 && (*str1 == *str2))
            str1++, str2++;
    return *str1 - *str2;
}

