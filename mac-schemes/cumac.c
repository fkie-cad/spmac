#include "contiki-net.h"
#include "tinydtls.h"
#include "dtls-hmac.h"

#include <string.h>

#include "cumac.h"

#define MAX_HISTORY 16/TAGLEN

// input to hash function
typedef struct __attribute__((__packed__)) {
    uint32_t counter;
    unsigned char msg[MSGLEN];
} input_t;

// ring buffer of next tags
typedef struct __attribute__((__packed__)) {
    unsigned char tag[MAX_HISTORY][TAGLEN];
} state_t;

struct cumac_ctx
{
    input_t tx_input;
    input_t rx_input;
    state_t tx_state;
    state_t rx_state;
    int cumac_tx_tag;
    int cumac_rx_tag;
    dtls_hmac_context_t* ctx;
    unsigned char cumac_key[32];
    unsigned char temp[32];
    int len;
} cumac_ctx;


void cumac_init(unsigned char* key, int taglen){
    
    cumac_ctx.tx_input.counter = rand();
    cumac_ctx.rx_input.counter = cumac_ctx.tx_input.counter;
    
    cumac_ctx.cumac_tx_tag=0;
    cumac_ctx.cumac_rx_tag=0;
    
   
    memcpy( cumac_ctx.cumac_key, key, 32 );

    cumac_ctx.ctx = dtls_hmac_new( key, 32 );
    dtls_hmac_init(cumac_ctx.ctx, cumac_ctx.cumac_key, 32);

    cumac_ctx.len = taglen;

    int i,j;
    for(i=0; i<MAX_HISTORY; i++){
	    for(j=0; j<TAGLEN; j++){
	        unsigned char temp = rand();	    
		    cumac_ctx.tx_state.tag[i][j] = temp;
		    cumac_ctx.rx_state.tag[i][j] = temp;
	    }
	}

}

void cumac_sign(unsigned char* msg, unsigned char* output, rtimer_clock_t* end_live){

    memcpy(cumac_ctx.tx_input.msg, msg, MSGLEN);

    dtls_hmac_update(cumac_ctx.ctx, (unsigned char*) &cumac_ctx.tx_input, 4+MSGLEN);
    dtls_hmac_finalize(cumac_ctx.ctx, cumac_ctx.temp);

    int i,j;
    for(i=0; i<TAGLEN; i++){
        output[i] = cumac_ctx.temp[i] ^ cumac_ctx.tx_state.tag[cumac_ctx.cumac_tx_tag][i];
        cumac_ctx.tx_state.tag[cumac_ctx.cumac_tx_tag][i] = 0; // reset the bits to remove dependencies on older msgs
    }

    *end_live = RTIMER_NOW();

    for(i=1; i<MAX_HISTORY; i++){
        for(j=0; j<TAGLEN; j++){
           cumac_ctx.tx_state.tag[(cumac_ctx.cumac_tx_tag + i) % MAX_HISTORY][j] ^= cumac_ctx.temp[i*TAGLEN+j];
        }
    }

    cumac_ctx.tx_input.counter += 1;
    cumac_ctx.cumac_tx_tag = (cumac_ctx.cumac_tx_tag + 1)%MAX_HISTORY;
    dtls_hmac_init(cumac_ctx.ctx, cumac_ctx.cumac_key, 32);
}

int cumac_vrfy(unsigned char* msg, unsigned char* sig){

    unsigned char tag[TAGLEN];

    memcpy(cumac_ctx.rx_input.msg, msg, MSGLEN);

    dtls_hmac_update(cumac_ctx.ctx, (unsigned char*) &cumac_ctx.rx_input, 4+MSGLEN);
    dtls_hmac_finalize(cumac_ctx.ctx, cumac_ctx.temp);

    int i,j;
    for(i=0; i<TAGLEN; i++){
        tag[i] = cumac_ctx.temp[i] ^ cumac_ctx.rx_state.tag[cumac_ctx.cumac_rx_tag][i];
        cumac_ctx.rx_state.tag[cumac_ctx.cumac_rx_tag][i] = 0; // reset the bits to remove dependencies on older msgs
    }

    for(i=1; i<MAX_HISTORY; i++){
        for(j=0; j<TAGLEN; j++){
            cumac_ctx.rx_state.tag[(cumac_ctx.cumac_rx_tag + i) % MAX_HISTORY][j] ^= cumac_ctx.temp[i*TAGLEN+j];
        }
    }

    cumac_ctx.rx_input.counter += 1;
    cumac_ctx.cumac_rx_tag = (cumac_ctx.cumac_rx_tag + 1)%MAX_HISTORY;
    dtls_hmac_init(cumac_ctx.ctx, cumac_ctx.cumac_key, 32);

    return memcmp( sig, tag, TAGLEN );
}

void cumac_deinit(){

    dtls_hmac_free(cumac_ctx.ctx);
}
