#pragma once

#include "contiki.h"

void cumac_init(unsigned char* key, int taglen);
void cumac_sign(unsigned char* msg, unsigned char* output, rtimer_clock_t* end_live);
int cumac_vrfy(unsigned char* msg, unsigned char* sig);
void cumac_deinit();
