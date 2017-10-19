
int A[56];
int main (){
	for (int i = 0; i < 100 ; i++)
		A[i>55?i-56:i] = (i+54321)/35;



	return A[50];
}
