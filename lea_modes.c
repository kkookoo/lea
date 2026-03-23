#include "lea_modes.h"

/*big endian to little endian*/
unsigned int bswap(unsigned int x) 
{
    return ((x >> 24) & 0x000000FF) |
           ((x >> 8)  & 0x0000FF00) |
           ((x << 8)  & 0x00FF0000) |
           ((x << 24) & 0xFF000000);
}

/*ascii to hex*/
void ascii_to_hex(const char* str, int size, unsigned int* hex)
{
    unsigned int i, h, high, low;
    for (h = 0, i = 0; i < size; i += 2, ++h) {
        high = (str[i] > '9') ? str[i] - 'A' + 10 : str[i] - '0';
        low = (str[i + 1] > '9') ? str[i + 1] - 'A' + 10 : str[i + 1] - '0';
        hex[h] = (high << 4) | low;
    }
}

/*CBC KAT, MMT test*/
void lea_cbc_MMT()
{
    FILE *fp = fopen("LEA 운용모드 정확성검증 테스트벡터/LEA128(CBC)MMT.txt","r");

    if(fp == NULL){
        printf("file open error\n");
    }

    unsigned int Key[4] = {0};      // 128 : 4, 192 : 6, 256 : 8
    unsigned int IV[4] = {0};

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

        else if(sscanf(line, "IV = %8x%8x%8x%8x", &IV[0], &IV[1], &IV[2], &IV[3]) == 4)
        {
            for(int i = 0; i < 4; i++)
                IV[i] = bswap(IV[i]);
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
            printf("IV = %08x %08x %08x %08x\n", IV[0], IV[1], IV[2], IV[3]);
            printf("PT     = ");
            for (int i = 0; i < size_PT / 8; i++) 
            printf("%08x ", PT[i]);
            printf("\n");
            printf("CT     = ");
            for (int i = 0; i < size_CT / 8; i++)
            printf("%08x ", CT[i]);
            printf("\n");

            unsigned int RK[24][6]; // 128 : [24][6], 192 : [28][6], 256 : [32][6]
            unsigned int result[256] = {0};
            unsigned int prev[4];
            int blocks = ct_word_len / 4;

            lea_enc_keySche128(Key, RK);
            // lea_enc_keySche192(Key, RK);
            // lea_enc_keySche256(Key, RK);

            for(int i = 0; i < 4; i++)
                prev[i] = IV[i];

            for(int b = 0; b < blocks; b++)
            {
                unsigned int in_block[4];
                unsigned int out_block[4];

                for(int i = 0; i < 4; i++)
                    in_block[i] = PT[b * 4 + i] ^ prev[i];

                lea_encrypt128(in_block, RK, out_block);
                // lea_encrypt192(in_block, RK, out_block);
                // lea_encrypt256(in_block, RK, out_block);

                for(int i = 0; i < 4; i++) {
                    result[b * 4 + i] = out_block[i];
                    prev[i] = out_block[i];
                }
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

/*CBC MCT test*/
void lea_cbc_MCT()
{
    FILE *fp = fopen("LEA 운용모드 정확성검증 테스트벡터/LEA256(CBC)MCT.txt","r");

    if(fp == NULL){
        printf("file open error\n");
    }

    unsigned int Key[8] = {0};      // 128 : 4, 192 : 6, 256 : 8
    unsigned int IV[4] = {0};
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
            for(int i = 0; i < 8; i++){     // 128 : 4, 192 : 6, 256 : 8
                Key[i] = bswap(Key[i]);
            }
        }

        else if(sscanf(line, "IV = %8x%8x%8x%8x", &IV[0], &IV[1], &IV[2], &IV[3]) == 4)
        {
            for(int i = 0; i < 4; i++){
                IV[i] = bswap(IV[i]);
            }
        }

        else if(sscanf(line, "PT = %8x%8x%8x%8x", &PT[0], &PT[1], &PT[2], &PT[3]) == 4)
        {
            for(int i = 0; i < 4; i++){
                PT[i] = bswap(PT[i]);
            }
        }

        else if(sscanf(line, "CT = %8x%8x%8x%8x", &CT[0], &CT[1], &CT[2], &CT[3]) == 4)
        {
            for(int i = 0; i < 4; i++){
                CT[i] = bswap(CT[i]);
            }

            // 128 : 4, 192 : 6, 256 : 8
            printf("KEY = %08x %08x %08x %08x %08x %08x %08x %08x\n", Key[0], Key[1], Key[2], Key[3], Key[4], Key[5], Key[6], Key[7]);
            printf("IV = %08x %08x %08x %08x\n", IV[0], IV[1], IV[2], IV[3]);
            printf("PT     = %08x %08x %08x %08x\n", PT[0], PT[1], PT[2], PT[3]);
            printf("CT     = %08x %08x %08x %08x\n", CT[0], CT[1], CT[2], CT[3]);

            unsigned int RK[32][6] = {0}; // 128 : [24][6], 192 : [28][6], 256 : [32][6]
            unsigned int result[256] = {0};

            unsigned int curIV[4];
            unsigned int curPT[4]; 

            unsigned int lastCT[4] = {0};
            unsigned int prevCT[4] = {0};

            for(int i = 0; i < 4; i++) {
                curIV[i] = IV[i];
                curPT[i] = PT[i];
            }

            for(int j = 0; j < 1000; j++) 
            {
                unsigned int in_block[4];
                unsigned int out_block[4];

                // lea_enc_keySche128(Key, RK);
                // lea_enc_keySche192(Key, RK);
                lea_enc_keySche256(Key, RK);

                for(int i = 0; i < 4; i++)
                    in_block[i] = curPT[i] ^ curIV[i];

                // lea_encrypt128(in_block, RK, out_block);
                // lea_encrypt192(in_block, RK, out_block);
                lea_encrypt256(in_block, RK, out_block);

                for(int i = 0; i < 4; i++) {
                    result[i] = out_block[i];
                    prevCT[i] = lastCT[i];
                    lastCT[i] = out_block[i];
                }

                if(j == 0)
                {
                    for (int i = 0; i < 4; i++) {
                        curIV[i] = IV[i];
                        curPT[i] = out_block[i];
                    }
                }
                else
                {
                    for (int i = 0; i < 4; i++) {
                        curIV[i] = prevCT[i];
                        curPT[i] = out_block[i];
                    }
                }
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

void CTR_increment(unsigned int* CTR)
{
    for(int i = 3; i >= 0; i--) 
    {
        if(CTR[i] == 0xFFFFFFFF)
            CTR[i] = 0;
        else
        {
            CTR[i]++;
            break;
        }
    }
}

/*CTR KAT, MMT test*/
void lea_ctr_MMT()
{
    FILE *fp = fopen("LEA 운용모드 정확성검증 테스트벡터/LEA128(CTR)MMT.txt","r");

    if(fp == NULL){
        printf("file open error\n");
    }

    unsigned int Key[4] = {0};      // 128 : 4, 192 : 6, 256 : 8
    unsigned int CTR[4] = {0};

    char str_PT[1024] = {0};
    char str_CT[1024] = {0};
    
    int size_PT = 0;
    int size_CT = 0;
    
    unsigned int *PT = NULL;
    unsigned int *CT = NULL;
    

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

        else if(sscanf(line, "CTR = %8x%8x%8x%8x", &CTR[0], &CTR[1], &CTR[2], &CTR[3]) == 4){
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

            for(int i = 0; i < ct_word_len; i++) 
                CT[i] = CT_word[i];

            for(int i = 0; i < ct_word_len; i++)
                CT[i] = bswap(CT[i]);

            // 128 : 4, 192 : 6, 256 : 8
            printf("KEY = %08x %08x %08x %08x\n", Key[0], Key[1], Key[2], Key[3]);
            printf("CTR = %08x %08x %08x %08x\n", CTR[0], CTR[1], CTR[2], CTR[3]);
            printf("PT     = ");
            for (int i = 0; i < size_PT / 8; i++) 
            printf("%08x ", PT[i]);
            printf("\n");
            printf("CT     = ");
            for (int i = 0; i < size_CT / 8; i++)
            printf("%08x ", CT[i]);
            printf("\n");

            unsigned int RK[24][6]; // 128 : [24][6], 192 : [28][6], 256 : [32][6]
            unsigned int result[256] = {0};
            int blocks = ct_word_len / 4;
            

            lea_enc_keySche128(Key, RK);
            // lea_enc_keySche192(Key, RK);
            // lea_enc_keySche256(Key, RK);

            for(int b = 0; b < blocks; b++)
            {
                unsigned int out_block[4];
                unsigned int CTR_enc[4];

                for(int i = 0; i < 4; i++)
                    CTR_enc[i] = bswap(CTR[i]);

                lea_encrypt128(CTR_enc, RK, out_block);
                // lea_encrypt192(CTR_enc, RK, out_block);
                // lea_encrypt256(CTR_enc, RK, out_block);

                for(int i = 0; i < 4; i++) 
                    result[4 * b + i] = out_block[i] ^ PT[4 * b + i];

                CTR_increment(CTR);
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

void lea_ctr_MCT()
{
    FILE *fp = fopen("LEA 운용모드 정확성검증 테스트벡터/LEA128(CTR)MCT.txt","r");

    if(fp == NULL){
        printf("file open error\n");
    }

    unsigned int Key[4] = {0};      // 128 : 4, 192 : 6, 256 : 8
    unsigned int CTR[4] = {0};
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
        else if(sscanf(line, "KEY = %8x%8x%8x%8x", &Key[0], &Key[1], &Key[2], &Key[3]) == 4) 
        {    
            for(int i = 0; i < 4; i++){     // 128 : 4, 192 : 6, 256 : 8
                Key[i] = bswap(Key[i]);
            }
        }

        else if(sscanf(line, "CTR = %8x%8x%8x%8x", &CTR[0], &CTR[1], &CTR[2], &CTR[3]) == 4){
        }

        else if(sscanf(line, "PT = %8x%8x%8x%8x", &PT[0], &PT[1], &PT[2], &PT[3]) == 4)
        {
            for(int i = 0; i < 4; i++){
                PT[i] = bswap(PT[i]);
            }
        }

        else if(sscanf(line, "CT = %8x%8x%8x%8x", &CT[0], &CT[1], &CT[2], &CT[3]) == 4)
        {
            for(int i = 0; i < 4; i++){
                CT[i] = bswap(CT[i]);
            }

            // 128 : 4, 192 : 6, 256 : 8
            printf("KEY = %08x %08x %08x %08x\n", Key[0], Key[1], Key[2], Key[3]);
            printf("CTR = %08x %08x %08x %08x\n", CTR[0], CTR[1], CTR[2], CTR[3]);
            printf("PT     = %08x %08x %08x %08x\n", PT[0], PT[1], PT[2], PT[3]);
            printf("CT     = %08x %08x %08x %08x\n", CT[0], CT[1], CT[2], CT[3]);

            unsigned int RK[24][6] = {0}; // 128 : [24][6], 192 : [28][6], 256 : [32][6]
            unsigned int result[4] = {0};

            lea_enc_keySche128(Key, RK);
            // lea_enc_keySche192(Key, RK);
            // lea_enc_keySche256(Key, RK);

            for(int j = 0; j < 1000; j++) 
            {
                unsigned int CTR_enc[4];

                for(int i = 0; i < 4; i++)
                    CTR_enc[i] = bswap(CTR[i]);

                lea_encrypt128(CTR_enc, RK, result);
                // lea_encrypt192(CTR_enc, RK, result);
                // lea_encrypt256(CTR_enc, RK, result);

                for(int i = 0; i < 4; i++) 
                    result[i] ^= PT[i];

                for(int i = 0; i < 4; i++)
                    PT[i] = result[i];

                CTR_increment(CTR);
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

void lea_dec_CBC_MMT()
{
    FILE *fp = fopen("LEA 운용모드 정확성검증 테스트벡터/LEA128(CBC)MCT.txt","r");

    if(fp == NULL){
        printf("file open error\n");
        return;
    }

    unsigned int Key[4] = {0};
    unsigned int IV[4] = {0};
    unsigned int PT[4] = {0};
    unsigned int CT[4] = {0};

    int size_PT = 0;
    int size_CT = 0;

    char line[500];

    int total = 0;
    int pass = 0;
    int count = 0;

    while(fgets(line, sizeof(line), fp))
    {
        if(sscanf(line, "COUNT = %d", &count) == 1){}

        else if(sscanf(line, "KEY = %8x%8x%8x%8x", &Key[0], &Key[1], &Key[2], &Key[3]) == 4)
        {
            for(int i = 0; i < 4; i++)
                Key[i] = bswap(Key[i]);
        }

        else if(sscanf(line, "IV = %8x%8x%8x%8x", &IV[0], &IV[1], &IV[2], &IV[3]) == 4)
        {
            for(int i = 0; i < 4; i++)
                IV[i] = bswap(IV[i]);
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

            int ct_word_len = size_CT / 8;

            unsigned int RK[24][6] = {0};

            unsigned int curIV[4];
            unsigned int curCT[4];

            unsigned int prevCT[4] = {0};

            for(int i = 0; i < 4; i++) {
                curIV[i] = IV[i];
                curCT[i] = CT[i];
            }

            lea_enc_keySche128(Key, RK);

            // 128 : 4, 192 : 6, 256 : 8
            printf("KEY = %08x %08x %08x %08x\n", Key[0], Key[1], Key[2], Key[3]);
            printf("IV = %08x %08x %08x %08x\n", IV[0], IV[1], IV[2], IV[3]);
            printf("PT     = ");
            for (int i = 0; i < size_PT / 8; i++) 
            printf("%08x ", PT[i]);
            printf("\n");
            printf("CT     = ");
            for (int i = 0; i < size_CT / 8; i++)
            printf("%08x ", CT[i]);
            printf("\n");

            unsigned int RK_enc[24][6]; // 128 : [24][6], 192 : [28][6], 256 : [32][6]
            unsigned int RK_dec[24][6]; // 128 : [24][6], 192 : [28][6], 256 : [32][6]
            unsigned int result[256] = {0};
            unsigned int prev[4];
            int blocks = ct_word_len / 4;

            lea_enc_keySche128(Key, RK_enc);
            lea_dec_keySche128(RK_enc, RK_dec);
            // lea_dec_keySche192(RK_enc, RK_dec);
            // lea_dec_keySche256(RK_enc, RK_dec);

            for(int i = 0; i < 4; i++)
                prev[i] = IV[i];

            for(int b = 0; b < blocks; b++)
            {
                unsigned int in_block[4];
                unsigned int out_block[4];

                for(int i = 0; i < 4; i++)
                    in_block[i] = CT[b * 4 + i];

                lea_decrypt128(in_block, RK, out_block);
                // lea_decrypt192(in_block, RK, out_block);
                // lea_decrypt256(in_block, RK, out_block);

                for(int i = 0; i < 4; i++)
                    out_block[i] ^= prev[i];

                for(int i = 0; i < 4; i++) {
                    result[b * 4 + i] = out_block[i];
                    prev[i] = in_block[i];   
                }
            } 

            printf("Result = %08x %08x %08x %08x\n",
                result[0], result[1], result[2], result[3]);

            int ok = 1;
            for(int i = 0; i < 4; i++)
                if(result[i] != PT[i]) ok = 0;

            if(ok){ printf("PASS\n\n"); pass++; }
            else printf("FAIL\n\n");

            total++;
        }
    }

    fclose(fp);

    printf("TOTAL : %d\n", total);
    printf("PASS  : %d\n", pass);
    printf("FAIL  : %d\n", total - pass);
}

void lea_dec_CBC_MCT(unsigned int *ciphertxt, unsigned int RK_dec[24][6])
{
    
}

void lea_dec_CTR_MMT(unsigned int *ciphertxt, unsigned int RK_dec[24][6])
{
    
}

void lea_dec_CTR_MCT(unsigned int *ciphertxt, unsigned int RK_dec[24][6])
{
    
}
    