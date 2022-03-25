#pragma once

#include "contiki.h"

void minimac_init(unsigned char* key, int taglen);
void minimac_sign(unsigned char* msg, unsigned char* output, rtimer_clock_t* end_live);
int minimac_vrfy(unsigned char* msg, unsigned char* sig);
void minimac_deinit();
