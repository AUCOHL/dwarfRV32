
void IRQ0 (){ //bad interrupt handlers
	*((volatile int*)0x80000000) = '0';
	*((volatile int*)0x80000000) = '!';
	*((volatile int*)0x80000000) = '\n';
	asm("uret"); //necess
}

void IRQ1 (){
	*((volatile int*)0x80000000) = '1';
	*((volatile int*)0x80000000) = '!';
	*((volatile int*)0x80000000) = '\n';
	asm("uret"); //necess
}


int main (){
	for (int i = 0; i < 10000; i++); //delay

	return 0;
}


