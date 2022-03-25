#include "contiki-net.h"
#include "tinydtls.h"
#include "dtls-hmac.h"

#include <string.h>

#include "whips.h"

#define SUBSTATELEN 32

typedef struct __attribute__((__packed__)) {
    uint32_t counter;
    unsigned char substates[16/TAGLEN][SUBSTATELEN];
} state_t;

struct whips_ctx
{
    state_t tx_state;
    state_t rx_state;
    dtls_hmac_context_t* ctx;
    unsigned char whips_key[32];
    unsigned char whips_key2[32];
    unsigned char temp[32];
    int len;
    int nb_ele;
} whips_ctx;

void whips_init(unsigned char* key, unsigned char* key2, int taglen){
    
    whips_ctx.tx_state.counter = rand();
    whips_ctx.rx_state.counter = whips_ctx.tx_state.counter;
   
    memcpy( whips_ctx.whips_key, key, 32 );
    memcpy( whips_ctx.whips_key2, key2, 32 );

    whips_ctx.ctx = dtls_hmac_new( key, 32 );
    dtls_hmac_init(whips_ctx.ctx, whips_ctx.whips_key, 32);

    whips_ctx.len = taglen;
    whips_ctx.nb_ele = 16/TAGLEN;    

    int i,j;
    for(i=0; i<whips_ctx.nb_ele; i++){
	for(j=0; j<SUBSTATELEN; j++){
            unsigned char temp = rand();	    
            whips_ctx.tx_state.substates[i][j] = temp;
            whips_ctx.rx_state.substates[i][j] = temp;
	}
    }
}

void whips_update_state(state_t* state){
    int j;
    for(j=whips_ctx.nb_ele-1; j>0; j--){
        memcpy( state->substates[j], state->substates[j-1], MSGLEN);
    }

    state->counter++; 
}

void whips_sign(unsigned char* msg, unsigned char* output, rtimer_clock_t* end_live){

    dtls_hmac_update(whips_ctx.ctx, msg, MSGLEN);
    dtls_hmac_finalize(whips_ctx.ctx, whips_ctx.tx_state.substates[0]);
    
    dtls_hmac_init(whips_ctx.ctx, whips_ctx.whips_key2, 32); 
    dtls_hmac_update(whips_ctx.ctx,(unsigned char*) &whips_ctx.tx_state, 4+whips_ctx.nb_ele*SUBSTATELEN);
    dtls_hmac_finalize(whips_ctx.ctx, whips_ctx.temp);
    
    memcpy( output, whips_ctx.temp, whips_ctx.len );

    *end_live = RTIMER_NOW();
    
    whips_update_state(&whips_ctx.tx_state);
    dtls_hmac_init(whips_ctx.ctx, whips_ctx.whips_key, 32);
}

int whips_vrfy(unsigned char* msg, unsigned char* sig){
    
    dtls_hmac_update(whips_ctx.ctx, msg, MSGLEN);
    dtls_hmac_finalize(whips_ctx.ctx, whips_ctx.rx_state.substates[0]);
    
    dtls_hmac_init(whips_ctx.ctx, whips_ctx.whips_key2, 32); 
    dtls_hmac_update(whips_ctx.ctx,(unsigned char*) &whips_ctx.rx_state, 4+whips_ctx.nb_ele*SUBSTATELEN);
    dtls_hmac_finalize(whips_ctx.ctx, whips_ctx.temp);
    
    whips_update_state(&whips_ctx.rx_state);
    dtls_hmac_init(whips_ctx.ctx, whips_ctx.whips_key, 32);

    return memcmp( sig, whips_ctx.temp, whips_ctx.len );
}

void whips_deinit(){

    dtls_hmac_free(whips_ctx.ctx);
}
