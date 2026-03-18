#include "lea_enc_dec.h"
#include "lea_keySche.h"

void lea_encrypt(unsigned int *plaintxt, unsigned int RK_enc[24][6], unsigned int *ciphertxt)
{
    for(int i = 0; i < 24; i++)
        lea_roundEnc(plaintxt, RK_enc[i]);

    for(int i = 0; i < 4; i++)
        ciphertxt[i] = plaintxt[i];
}

void lea_roundEnc(unsigned int *x, unsigned int RK[6])
{
    unsigned int x0 = x[0];
    unsigned int x1 = x[1];
    unsigned int x2 = x[2];
    unsigned int x3 = x[3];

    unsigned int t0, t1, t2;

    t0 = bit_rotate_left((x0 ^ RK[0]) + (x1 ^ RK[1]), 9);
    t1 = bit_rotate_right((x1 ^ RK[2]) + (x2 ^ RK[3]), 5);
    t2 = bit_rotate_right((x2 ^ RK[4]) + (x3 ^ RK[5]), 3);

    x[0] = t0;
    x[1] = t1;
    x[2] = t2;
    x[3] = x0;
}

void lea_decrypt(unsigned int *ciphertxt, unsigned int RK_dec[24][6])
{
    for(int i = 0; i < 24; i++)
    {
        lea_roundDec(ciphertxt, RK_dec[i]);
    }
}

void lea_roundDec(unsigned int *x, unsigned int RK_dec_i[6])
{
    unsigned int tmp0 = x[0];
    unsigned int tmp1 = x[1];
    unsigned int tmp2 = x[2];

    x[0] = x[3];
    x[1] = (bit_rotate_right(tmp0, 9) - (x[0] ^ RK_dec_i[0])) ^ RK_dec_i[1];
    x[2] = (bit_rotate_left(tmp1, 5) - (x[1] ^ RK_dec_i[2])) ^ RK_dec_i[3];
    x[3] = (bit_rotate_left(tmp2, 3) - (x[2] ^ RK_dec_i[4])) ^ RK_dec_i[5];
}