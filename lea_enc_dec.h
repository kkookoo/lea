#include <stdio.h>
#include <stdlib.h>

#ifndef LEA_ENC_DEC_H
#define LEA_ENC_DEC_H

void lea_encrypt128(unsigned int *Plaintxt, unsigned int RK_enc[24][6], unsigned int *Ciphertxt);
void lea_encrypt192(unsigned int *Plaintxt, unsigned int RK_enc[28][6], unsigned int *Ciphertxt);
void lea_encrypt256(unsigned int *Plaintxt, unsigned int RK_enc[32][6], unsigned int *Ciphertxt);
void lea_roundEnc(unsigned int *x, unsigned int RK_enc_i[6]);

void lea_decrypt128(unsigned int *ciphertxt, unsigned int RK_dec[24][6], unsigned int *plaintxt);
void lea_decrypt192(unsigned int *ciphertxt, unsigned int RK_dec[28][6], unsigned int *plaintxt);
void lea_decrypt256(unsigned int *ciphertxt, unsigned int RK_dec[32][6], unsigned int *plaintxt);
void lea_roundDec(unsigned int *x, unsigned int RK_dec_i[6]);

#endif