#include "./special/libdivide.h"





int A[56];
int main (){
    struct libdivide_s32_t fast_d = libdivide_s32_gen(35);
	for (int i = 0; i < 100 ; i++)
		A[i>55?i-56:i] = libdivide_s32_do(i+54321, &fast_d);

	return A[50];
}
