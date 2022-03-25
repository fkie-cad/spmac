#pragma once

#include "contiki.h"

void whips_init(unsigned char* key, unsigned char* key2, int taglen);
void whips_sign(unsigned char* msg, unsigned char* output, rtimer_clock_t* end_live);
int whips_vrfy(unsigned char* msg, unsigned char* sig);
void whips_deinit();
