#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lea_keySche.h"
#include "lea_enc_dec.h"
#include "lea_modes.h"
#include "lea_ecb.h"


int main()
{
    // lea_ECB_MMT();
    lea_ECB_MCT();
    
    // lea_cbc_MMT();
    // lea_cbc_MCT();
    // lea_dec_CBC_MMT();
    // lea_dec_CBC_MCT();

    // lea_ctr_MMT();
    // lea_ctr_MCT();
    // lea_dec_CTR_MMT();
    // lea_dec_CTR_MCT();

    return 0;
}