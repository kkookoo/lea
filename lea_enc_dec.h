#include <stdio.h>
#include <stdlib.h>

#ifndef LEA_ENC_DEC_H
#define LEA_ENC_DEC_H

void lea_encrypt(unsigned int *Plaintxt, unsigned int RK_enc[24][6], unsigned int *Ciphertxt);
void lea_roundEnc(unsigned int *x, unsigned int RK_enc_i[6]);
void lea_decrypt(unsigned int *ciphertxt, unsigned int RK_dec[24][6]);
void lea_roundDec(unsigned int *x, unsigned int RK_dec_i[6]);

#endif