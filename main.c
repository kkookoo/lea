#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lea_keySche.h"
#include "lea_enc_dec.h"
#include "lea_modes.h"


int main()
{
    // lea_cbc_MMT();
    // lea_cbc_MCT();
    // lea_ctr_MMT();
    // lea_ctr_MCT();
    lea_dec_CBC_MMT();

    return 0;
}