#pragma once

#include "contiki.h"

void truncated_init(unsigned char* key, int taglen);
void truncated_sign(unsigned char* msg, unsigned char* output, rtimer_clock_t* end_live);
int truncated_vrfy(unsigned char* msg, unsigned char* sig);
void truncated_deinit();
