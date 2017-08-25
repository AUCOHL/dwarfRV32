//#include "stdio.h"
void encrypt (unsigned int* v, unsigned int* k) {
    unsigned int v0=v[0], v1=v[1], sum=0, i;           /* set up */
    unsigned int delta=0x9e3779b9;                     /* a key schedule constant */
    unsigned int k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    for (i=0; i < 32; i++) {                       /* basic cycle start */
        sum += delta;
        v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
    }                                              /* end cycle */
    v[0]=v0; v[1]=v1;
}

void decrypt (unsigned int* v, unsigned int* k) {
    unsigned int v0=v[0], v1=v[1], sum=0xC6EF3720, i;  /* set up */
    unsigned int delta=0x9e3779b9;                     /* a key schedule constant */
    unsigned int k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    for (i=0; i<32; i++) {                         /* basic cycle start */
        v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
        v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        sum -= delta;
    }                                              /* end cycle */
    v[0]=v0; v[1]=v1;
}

int main(){
    unsigned int data[2], d[2];
    unsigned int key[4];

    int r = -1;

    key[0]=0x11111111;
    key[1]=0x22222222;
    key[2]=0x33333333;
    key[3]=0x44444444;

    data[0] = 0xDEAD0BAD;
    data[1] = 0xFEED0BED;

    d[0] = data[0];
    d[1] = data[1];

    encrypt(d,key);
    decrypt(d,key);

    if(data[0]==d[0] && data[1]==d[1]) r = 1;
    else r = 0;

    //printf("%d\n", r);
    return r;
}
