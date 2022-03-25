#pragma once

#include "contiki.h"

void spmac_init(unsigned char* key, int taglen);
void spmac_sign(unsigned char* msg, unsigned char* output, rtimer_clock_t* end_live);
int spmac_vrfy(unsigned char* msg, unsigned char* sig);
void spmac_deinit();
