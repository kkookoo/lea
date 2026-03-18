#include <stdio.h>
#include <stdlib.h>

#ifndef LEA_KEYSCHE_H
#define LEA_KEYSCHE_H

void lea_enc_keySche128(unsigned int *K, unsigned int RK_enc[24][6]);
void lea_enc_keySche192(unsigned int *K, unsigned int RK_enc[28][6]);
void lea_enc_keySche256(unsigned int *K, unsigned int RK_enc[32][6]);

void lea_dec_keySche128(unsigned int RK_enc[24][6], unsigned int RK_dec[24][6]);
void lea_dec_keySche192(unsigned int RK_enc[28][6], unsigned int RK_dec[28][6]);
void lea_dec_keySche256(unsigned int RK_enc[32][6], unsigned int RK_dec[32][6]);

unsigned int bit_rotate_left(unsigned int number, int n);
unsigned int bit_rotate_right(unsigned int number, int n);

#endif