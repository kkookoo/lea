#include "lea_keySche.h"

const unsigned int delta[8] = {
    0xc3efe9db, 0x44626b02, 0x79e27c8a, 0x78df30ec, 
    0x715ea49e, 0xc785da0a, 0xe04ef22a, 0xe5c40957
};

void lea_enc_keySche128(unsigned int *K, unsigned int RK_enc[24][6]) 
{
    unsigned int T[4];

    for(int i = 0; i < 4; i++){
        T[i] = K[i];
    }

    for(int i = 0; i < 24; i++)
    {
        T[0] = bit_rotate_left(T[0] + bit_rotate_left(delta[i % 4], i), 1);
        T[1] = bit_rotate_left(T[1] + bit_rotate_left(delta[i % 4], i + 1), 3);
        T[2] = bit_rotate_left(T[2] + bit_rotate_left(delta[i % 4], i + 2), 6);
        T[3] = bit_rotate_left(T[3] + bit_rotate_left(delta[i % 4], i + 3), 11);
        
        RK_enc[i][0] = T[0];
        RK_enc[i][1] = T[1];
        RK_enc[i][2] = T[2];
        RK_enc[i][3] = T[1];
        RK_enc[i][4] = T[3];
        RK_enc[i][5] = T[1];
    };
}

void lea_enc_keySche192(unsigned int *K, unsigned int RK_enc[28][6]) 
{
    unsigned int T[6];
    
    for(int i = 0; i < 6; i++){
        T[i] = K[i];
    }

    for(int i = 0; i < 28; i++)
    {
        T[0] = bit_rotate_left(T[0] + bit_rotate_left(delta[i % 6], i), 1);
        T[1] = bit_rotate_left(T[1] + bit_rotate_left(delta[i % 6], i + 1), 3);
        T[2] = bit_rotate_left(T[2] + bit_rotate_left(delta[i % 6], i + 2), 6);
        T[3] = bit_rotate_left(T[3] + bit_rotate_left(delta[i % 6], i + 3), 11);
        T[4] = bit_rotate_left(T[4] + bit_rotate_left(delta[i % 6], i + 4), 13);
        T[5] = bit_rotate_left(T[5] + bit_rotate_left(delta[i % 6], i + 5), 17);
        
        
        RK_enc[i][0] = T[0];
        RK_enc[i][1] = T[1];
        RK_enc[i][2] = T[2];
        RK_enc[i][3] = T[3];
        RK_enc[i][4] = T[4];
        RK_enc[i][5] = T[5];
    };
}

void lea_enc_keySche256(unsigned int *K, unsigned int RK_enc[32][6]) 
{
    unsigned int T[8];
    
    for(int i = 0; i < 8; i++){
        T[i] = K[i];
    }

    for(int i = 0; i < 32; i++)
    {
        T[(6 * i) % 8] = bit_rotate_left(T[(6 * i) % 8] + bit_rotate_left(delta[i % 8], i), 1);
        T[(6 * i + 1) % 8] = bit_rotate_left(T[(6 * i + 1) % 8] + bit_rotate_left(delta[i % 8], i + 1), 3);
        T[(6 * i + 2) % 8] = bit_rotate_left(T[(6 * i + 2) % 8] + bit_rotate_left(delta[i % 8], i + 2), 6);
        T[(6 * i + 3) % 8] = bit_rotate_left(T[(6 * i + 3) % 8] + bit_rotate_left(delta[i % 8], i + 3), 11);
        T[(6 * i + 4) % 8] = bit_rotate_left(T[(6 * i + 4) % 8] + bit_rotate_left(delta[i % 8], i + 4), 13);
        T[(6 * i + 5) % 8] = bit_rotate_left(T[(6 * i + 5) % 8] + bit_rotate_left(delta[i % 8], i + 5), 17);
        
        
        RK_enc[i][0] = T[(6 * i) % 8];
        RK_enc[i][1] = T[(6 * i + 1) % 8];
        RK_enc[i][2] = T[(6 * i + 2) % 8];
        RK_enc[i][3] = T[(6 * i + 3) % 8];
        RK_enc[i][4] = T[(6 * i + 4) % 8];
        RK_enc[i][5] = T[(6 * i + 5) % 8];
    };
}

void lea_dec_keySche128(unsigned int RK_enc[24][6], unsigned int RK_dec[24][6])
{
    for(int i = 0; i < 24; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            RK_dec[i][j] = RK_enc[23 - i][j];
        }
    }
}

void lea_dec_keySche192(unsigned int RK_enc[28][6], unsigned int RK_dec[28][6])
{
    for(int i = 0; i < 28; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            RK_dec[i][j] = RK_enc[27 - i][j];
        }
    }
}

void lea_dec_keySche256(unsigned int RK_enc[32][6], unsigned int RK_dec[32][6])
{
    for(int i = 0; i < 32; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            RK_dec[i][j] = RK_enc[31 - i][j];
        }
    }
}

unsigned int bit_rotate_left(unsigned int number, int n)
{
    n &= 31;
    if (n == 0) return number;
	return (number << n) | (number >> (32 - n));
}

unsigned int bit_rotate_right(unsigned int number, int n)
{
	n &= 31;
    if (n == 0) return number;
	return (number >> n) | (number << (32 - n));
}