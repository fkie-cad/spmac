/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include "contiki.h"
#include "tinydtls.h"


#include <string.h>


#ifndef TAGLEN
    #define TAGLEN 4
    #define MSGLEN 10
#endif


#include "truncated.h"
#include "minimac.h"
#include "whips.h"
#include "spmac.h"
#include "cumac.h"

#include <stdio.h>

PROCESS(udp_client_process, "promac");
AUTOSTART_PROCESSES(&udp_client_process);
PROCESS_THREAD(udp_client_process, ev, data1)
{

    PROCESS_BEGIN();
    int i,n;
    #define nb_of_repetitions 30
    rtimer_clock_t start, end_live, end;
    unsigned char msg[MSGLEN];
    unsigned char output[TAGLEN];
    unsigned char key[32];
    for(i = 0; i < 32; i++)
        key[i] = rand();

    // Truncated MAC
    {
	truncated_init(key, TAGLEN);

        for(n=0; n < nb_of_repetitions; n++){
        
            for(i = 0; i < MSGLEN; i++)
                msg[i] = rand();  

            start = RTIMER_NOW();
            
            truncated_sign(msg, output, &end_live);
            
            end = RTIMER_NOW();

            printf("Trunc %d l: %ld p: %ld t: %ld\n", n, (long unsigned int)(end_live-start), (long unsigned int)(end-end_live), (long unsigned int)RTIMER_ARCH_SECOND);

        }

        truncated_deinit();
    }

    // Mini-MAC
    {
	minimac_init(key, TAGLEN);

        for(n=0; n < nb_of_repetitions; n++){
        
            for(i = 0; i < MSGLEN; i++)
                msg[i] = rand();  

            start = RTIMER_NOW();
            
            minimac_sign(msg, output, &end_live);
            
            end = RTIMER_NOW();

            printf("Minimac %d l: %ld p: %ld t: %ld\n", n, (long unsigned int)(end_live-start), (long unsigned int)(end-end_live), (long unsigned int)RTIMER_ARCH_SECOND);

        }

        minimac_deinit();
    }
 
    // Whips
    {

	unsigned char key2[32];
    	for(i = 0; i < 32; i++)
            key2[i] = rand();

	whips_init(key, key2, TAGLEN);

	for(n=0; n < nb_of_repetitions; n++){
	
	    for(i = 0; i < MSGLEN; i++)
	        msg[i] = rand();  

	    start = RTIMER_NOW();
	    
	    whips_sign(msg, output, &end_live);
	    
	    end = RTIMER_NOW();

	    printf("Whips %d l: %ld p: %ld t: %ld\n", n, (long unsigned int)(end_live-start), (long unsigned int)(end-end_live), (long unsigned int)RTIMER_ARCH_SECOND);

	}

	whips_deinit();
    }
    
    // CuMAC   
    {
	cumac_init(key, TAGLEN);

        for(n=0; n < nb_of_repetitions; n++){
        
            for(i = 0; i < MSGLEN; i++)
                msg[i] = rand();  

            start = RTIMER_NOW();
            
            cumac_sign(msg, output, &end_live);
            
            end = RTIMER_NOW();

            printf("CuMAC %d l: %ld p: %ld t: %ld\n", n, (long unsigned int)(end_live-start), (long unsigned int)(end-end_live), (long unsigned int)RTIMER_ARCH_SECOND);

        }

        spmac_deinit();
    }

    // SP-MAC   
    {
	spmac_init(key, TAGLEN);

        for(n=0; n < nb_of_repetitions; n++){
        
            for(i = 0; i < MSGLEN; i++)
                msg[i] = rand();  

            start = RTIMER_NOW();
            
            spmac_sign(msg, output, &end_live);
            
            end = RTIMER_NOW();

            printf("Spmac %d l: %ld p: %ld t: %ld\n", n, (long unsigned int)(end_live-start), (long unsigned int)(end-end_live), (long unsigned int)RTIMER_ARCH_SECOND);

        }

        spmac_deinit();
    }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
