#include "contiki-net.h"
#include "tinydtls.h"
#include "dtls-hmac.h"

#include <string.h>

#include "truncated.h"

typedef struct __attribute__((__packed__)) {
    uint32_t counter;
    unsigned char msg[MSGLEN];
} input_t;

typedef struct __attribute__((__packed__)) {
    uint32_t counter;
    unsigned char key[32];
} state_t;

struct truncated_ctx
{
    input_t input;
    state_t tx_state;
    state_t rx_state;
    dtls_hmac_context_t* ctx;
    unsigned char truncated_key[32];
    unsigned char temp[32];
    int len;
} truncated_ctx;

void truncated_init(unsigned char* key, int taglen){
    
    int temp = rand();
    truncated_ctx.tx_state.counter = temp;
    truncated_ctx.rx_state.counter = temp;
   
    memcpy( truncated_ctx.truncated_key, key, 32 );

    truncated_ctx.ctx = dtls_hmac_new( key, 32 );
    dtls_hmac_init(truncated_ctx.ctx, key, 32);

    truncated_ctx.len = taglen;
}

void truncated_sign(unsigned char* msg, unsigned char* output, rtimer_clock_t* end_live){

    truncated_ctx.input.counter = truncated_ctx.tx_state.counter;
    memcpy(truncated_ctx.input.msg, msg, MSGLEN );
            
    
    dtls_hmac_update(truncated_ctx.ctx,(unsigned char*) &truncated_ctx.input, 4+MSGLEN);
    dtls_hmac_finalize(truncated_ctx.ctx, truncated_ctx.temp);
    
    memcpy( output, truncated_ctx.temp, truncated_ctx.len );

    *end_live = RTIMER_NOW();

    dtls_hmac_init(truncated_ctx.ctx, truncated_ctx.truncated_key, 32);
    truncated_ctx.tx_state.counter++;
}

int truncated_vrfy(unsigned char* msg, unsigned char* sig){
    
    truncated_ctx.input.counter = truncated_ctx.rx_state.counter;
    memcpy(truncated_ctx.input.msg, msg, MSGLEN );
            
    dtls_hmac_update(truncated_ctx.ctx,(unsigned char*) &truncated_ctx.input, 4+MSGLEN);
    dtls_hmac_finalize(truncated_ctx.ctx, truncated_ctx.temp);
    
    dtls_hmac_init(truncated_ctx.ctx, truncated_ctx.truncated_key, 32);
    truncated_ctx.rx_state.counter++;

    return memcmp( sig, truncated_ctx.temp, truncated_ctx.len );
}

void truncated_deinit(){

    dtls_hmac_free(truncated_ctx.ctx);
}
