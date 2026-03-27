#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lea_keySche.h"
#include "lea_enc_dec.h"
#include "lea_modes.h"


/*ECB KAT, MMT test*/
void lea_ECB_MMT()
{
    FILE *fp = fopen("LEA 운용모드 정확성검증 테스트벡터/LEA128(ECB)KAT.txt","r");

    if(fp == NULL){
        printf("file open error\n");
    }

    unsigned int Key[4] = {0};      // 128 : 4, 192 : 6, 256 : 8

    char str_PT[1024] = {0};
    char str_CT[1024] = {0};
    
    int size_PT = 0;
    int size_CT = 0;
    
    unsigned int *PT = NULL;
    unsigned int *CT = NULL;

    unsigned int IV_init[4];

    char line[500];

    int total = 0;
    int pass = 0;
    int count = 0;

    while(fgets(line, sizeof(line), fp))
    {
        if(sscanf(line, "COUNT = %d", &count) == 1){
        }
        
        // 128 : 4, 192 : 6, 256 : 8
        else if(sscanf(line, "KEY = %8x%8x%8x%8x", &Key[0], &Key[1], &Key[2], &Key[3]) == 4) 
        {    
            for(int i = 0; i < 4; i++)     // 128 : 4, 192 : 6, 256 : 8
                Key[i] = bswap(Key[i]);
        }

        else if(sscanf(line, "PT = %1023s", str_PT) == 1)
        {
            size_PT = strlen(str_PT);

            PT = (unsigned int *)malloc(size_PT * sizeof(unsigned int));
            ascii_to_hex(str_PT, size_PT, PT);
            
            int pt_word_len = size_PT / 8;
            unsigned int PT_word[256];

            for(int i = 0; i < pt_word_len; i++) 
            {
                PT_word[i] = ((PT[i * 4] & 0xFF) << 24) |
                             ((PT[i * 4 + 1] & 0xFF) << 16) |
                            ((PT[i * 4 + 2] & 0xFF) << 8)  |
                            ((PT[i * 4 + 3] & 0xFF));
            }

            for(int i = 0; i < pt_word_len; i++) 
                PT[i] = PT_word[i];

            for(int i = 0; i < pt_word_len; i++)
                PT[i] = bswap(PT[i]);
        }

        else if(sscanf(line, "CT = %1023s", str_CT) == 1)
        {
            size_CT = strlen(str_CT);

            CT = (unsigned int *)malloc(size_CT * sizeof(unsigned int));
            ascii_to_hex(str_CT, size_CT, CT);

            int ct_word_len = size_CT / 8;
            unsigned int CT_word[256];

            for(int i = 0; i < ct_word_len; i++) 
            {
                CT_word[i] = ((CT[i * 4] & 0xFF) << 24) |
                        ((CT[i * 4 + 1] & 0xFF) << 16) |
                        ((CT[i * 4 + 2] & 0xFF) << 8)  |
                        ((CT[i * 4 + 3] & 0xFF));
            }

            for(int i = 0; i < ct_word_len; i++) {
                CT[i] = CT_word[i];
            }

            for(int i = 0; i < ct_word_len; i++)
                CT[i] = bswap(CT[i]);

            // 128 : 4, 192 : 6, 256 : 8
            printf("KEY = %08x %08x %08x %08x\n", Key[0], Key[1], Key[2], Key[3]);
            printf("PT     = ");
            for (int i = 0; i < size_PT / 8; i++) 
            printf("%08x ", PT[i]);
            printf("\n");
            printf("CT     = ");
            for (int i = 0; i < size_CT / 8; i++)
            printf("%08x ", CT[i]);
            printf("\n");

            unsigned int RK_enc[24][6]; // 128 : [24][6], 192 : [28][6], 256 : [32][6]
            unsigned int result[256] = {0};
            int blocks = ct_word_len / 4;

            lea_enc_keySche128(Key, RK_enc);
            // lea_dec_keySche128(RK_enc, RK_dec);
            // lea_enc_keySche192(Key, RK_enc);
            // lea_dec_keySche192(RK_enc, RK_dec);
            // lea_enc_keySche256(Key, RK_enc);
            // lea_dec_keySche256(RK_enc, RK_dec);

            for(int b = 0; b < blocks; b++)
            {
                unsigned int in_block[4];
                unsigned int out_block[4];

                for(int i = 0; i < 4; i++)
                    in_block[i] = PT[b * 4 + i];

                // for(int i = 0; i < 4; i++)
                //     in_block[i] = CT[b * 4 + i];

                lea_encrypt128(in_block, RK_enc, out_block);
                // lea_encrypt192(in_block, RK_enc, out_block);
                // lea_encrypt256(in_block, RK_enc, out_block);

                // lea_decrypt128(in_block, RK_dec);
                // lea_decrypt192(in_block, RK_dec);
                // lea_decrypt256(in_block, RK_dec);

                for(int i = 0; i < 4; i++)
                    result[b * 4 + i] = out_block[i];
            
                // for(int i = 0; i < 4; i++)
                //     result[b * 4 + i] = in_block[i];

            }       

            printf("Result = ");
            for (int i = 0; i < size_PT / 8; i++)
                printf("%08x ", result[i]);
            printf("\n");

            /*result check*/
            int ok = 1;
            for(int b = 0; b < ct_word_len / 4; b++) {
                if(result[4 * b] != CT[4 * b] ||
                result[4 * b + 1] != CT[4 * b + 1] ||
                result[4 * b + 2] != CT[4 * b + 2] ||
                result[4 * b + 3] != CT[4 * b + 3]) {
                    ok = 0;
                    break;
                }
            }   

            if(ok) {
                printf("PASS\n\n");
                pass++;
            } else {
                printf("FAIL\n\n");
            }
            total++;
        }
    }

    fclose(fp);

    printf("TOTAL : %d\n", total);
    printf("PASS  : %d\n", pass);
    printf("FAIL  : %d\n", total - pass);
}

/*ECB MCT test*/
void lea_ECB_MCT()
{
    FILE *fp = fopen("LEA 운용모드 정확성검증 테스트벡터/LEA256(ECB)MCT.txt","r");

    if(fp == NULL){
        printf("file open error\n");
    }

    unsigned int Key[8] = {0};      // 128 : 4, 192 : 6, 256 : 8
    unsigned int PT[4] = {0};
    unsigned int CT[4] = {0};

    char line[500];

    int total = 0;
    int pass = 0;
    int count = 0;

    while(fgets(line, sizeof(line), fp))
    {
        if(sscanf(line, "COUNT = %d", &count) == 1){
        }
        
        // 128 : 4, 192 : 6, 256 : 8
        else if(sscanf(line, "KEY = %8x%8x%8x%8x%8x%8x%8x%8x", &Key[0], &Key[1], &Key[2], &Key[3], &Key[4], &Key[5], &Key[6], &Key[7]) == 8) 
        {    
            for(int i = 0; i < 8; i++)     // 128 : 4, 192 : 6, 256 : 8
                Key[i] = bswap(Key[i]);
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

            // 128 : 4, 192 : 6, 256 : 8
            printf("KEY = %08x %08x %08x %08x\n", Key[0], Key[1], Key[2], Key[3]);
            printf("PT     = %08x %08x %08x %08x\n", PT[0], PT[1], PT[2], PT[3]);
            printf("CT     = %08x %08x %08x %08x\n", CT[0], CT[1], CT[2], CT[3]);

            unsigned int RK[32][6] = {0}; // 128 : [24][6], 192 : [28][6], 256 : [32][6]
            unsigned int result[4] = {0};            

            // lea_enc_keySche128(Key, RK_enc);
            // lea_dec_keySche128(RK_enc, RK_dec);
            // lea_enc_keySche192(Key, RK_enc);
            // lea_dec_keySche192(RK_enc, RK_dec);
            lea_enc_keySche256(Key, RK_enc);
            // lea_dec_keySche256(RK_enc, RK_dec);

            for(int j = 0; j < 1000; j++) 
            {
                // unsigned int in_block[i];

                // for(int i = 0; i < 4; i++)
                //     in_block[i] = CT[i];

                // lea_encrypt128(PT, RK, result);
                // lea_encrypt192(PT, RK, result);
                lea_encrypt256(PT, RK, result);

                // lea_decrypt128(in_block, RK_dec);
                // lea_decrypt192(in_block, RK_dec);
                // lea_decrypt256(in_block, RK_dec);
                
                for(int i = 0; i < 4; i++)
                    PT[i] = result[i];

                // for(int i = 0; i < 4; i++)
                //     result[i] = in_block[i];
            }

            printf("Result = %08x %08x %08x %08x\n", result[0], result[1], result[2], result[3]);

            /*result check*/
            int ok = 1;
            if(result[0] != CT[0] || result[1] != CT[1] || result[2] != CT[2] || result[3] != CT[3]) {
                    ok = 0;
                    break;
                }

            if(ok) {
                printf("PASS\n\n");
                pass++;
            } else {
                printf("FAIL\n\n");
            }
            total++;
        }
    }
    fclose(fp);

    printf("TOTAL : %d\n", total);
    printf("PASS  : %d\n", pass);
    printf("FAIL  : %d\n", total - pass);
}