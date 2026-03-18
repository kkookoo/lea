#include <stdio.h>
#include <stdlib.h>
#include "lea_keySche.h"
#include "lea_enc_dec.h"

unsigned int bswap(unsigned int x)
{
    return ((x >> 24) & 0x000000FF) |
           ((x >> 8)  & 0x0000FF00) |
           ((x << 8)  & 0x00FF0000) |
           ((x << 24) & 0xFF000000);
}

int main()
{
    FILE *fp = fopen("LEA 운용모드 정확성검증 테스트벡터/LEA128(CBC)MCT.txt","r");

    if(fp == NULL){
        printf("file open error\n");
        return 1;
    }

    unsigned int Key[4] = {0};
    unsigned int IV[4] = {0};
    unsigned int PT[4] = {0};
    unsigned int CT[4] = {0};

    unsigned int IV_init[4];

    char line[200];

    int total = 0;
    int pass = 0;
    int count = 0;

    while(fgets(line, sizeof(line), fp))
    {
        if(sscanf(line, "COUNT = %d", &count) == 1){
        }

        else if(sscanf(line, "KEY = %8x%8x%8x%8x", &Key[0], &Key[1], &Key[2], &Key[3]) == 4)
        {
            for(int i = 0; i < 4; i++)
                Key[i] = bswap(Key[i]);
        }

        else if(sscanf(line, "IV = %8x%8x%8x%8x", &IV[0], &IV[1], &IV[2], &IV[3]) == 4)
        {
            for(int i = 0; i < 4; i++){
                IV[i] = bswap(IV[i]);
                IV_init[i] = IV[i];
            }
        }

        else if(sscanf(line, "PT = %8x%8x%8x%8x", &PT[0], &PT[1], &PT[2], &PT[3]) == 4)
        {
            for(int i = 0; i < 4; i++)
                PT[i] = bswap(PT[i]);
        }

        else if(sscanf(line, "CT = %8x%8x%8x%8x", &CT[0], &CT[1], &CT[2], &CT[3]) == 4)
        {
            for(int i = 0; i < 4; i++)
                CT[i] = bswap(CT[i]);

            printf("KEY = %08x%08x%08x%08x\n", Key[0], Key[1], Key[2], Key[3]);
            printf("IV = %08x%08x%08x%08x\n", IV[0], IV[1], IV[2], IV[3]);
            printf("PT = %08x%08x%08x%08x\n", PT[0], PT[1], PT[2], PT[3]);
            printf("CT     = %08x%08x%08x%08x\n", CT[0], CT[1], CT[2], CT[3]);

            for(int i = 0; i < 4; i++)
                PT[i] ^= IV_init[i];

            unsigned int RK[24][6];
            unsigned int result[4] = {0};

            lea_enc_keySche128(Key, RK);
            lea_encrypt(PT, RK, result);
            printf("Result = %08x%08x%08x%08x\n", result[0], result[1], result[2], result[3]);

            if(result[0]==CT[0] && result[1]==CT[1] &&
            result[2]==CT[2] && result[3]==CT[3])
            {
                printf("PASS\n\n");
                pass++;
            }
            else
            {
                printf("FAIL\n\n");
            }

            total++;
            break;
        }
    }

    fclose(fp);

    printf("TOTAL : %d\n", total);
    printf("PASS  : %d\n", pass);
    printf("FAIL  : %d\n", total-pass);

    return 0;
}