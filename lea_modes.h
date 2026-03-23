#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lea_keySche.h"
#include "lea_enc_dec.h"

#ifndef LEA_MODES_H
#define LEA_MODES_H

unsigned int bswap(unsigned int x);
void ascii_to_hex(const char* str, int size, unsigned int* hex);
void CTR_increment(unsigned int* CTR);
void lea_cbc_MMT();
void lea_cbc_MCT();
void lea_ctr_MMT();
void lea_ctr_MCT();
void lea_dec_CBC_MMT();
void lea_dec_CBC_MCT();
void lea_dec_CTR_MMT();
void lea_dec_CTR_MCT();

#endif