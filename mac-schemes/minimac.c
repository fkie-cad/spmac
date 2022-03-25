#include "contiki-net.h"
#include "tinydtls.h"
#include "dtls-hmac.h"

#include <string.h>

#include "minimac.h"

typedef struct __attribute__((__packed__)) {
    uint32_t counter;
    unsigned char msg[16/TAGLEN][MSGLEN];
} state_t;


struct minimac_ctx
{
    state_t tx_state;
    state_t rx_state;
    dtls_hmac_context_t* ctx;
    unsigned char minimac_key[32];
    unsigned char temp[32];
    int len;
    int nb_ele;
} minimac_ctx;

void minimac_init(unsigned char* key, int taglen){
    
    minimac_ctx.tx_state.counter = rand();
    minimac_ctx.rx_state.counter = minimac_ctx.tx_state.counter;
   
    memcpy( minimac_ctx.minimac_key, key, 32 );

    minimac_ctx.ctx = dtls_hmac_new( key, 32 );
    dtls_hmac_init(minimac_ctx.ctx, minimac_ctx.minimac_key, 32);

    minimac_ctx.len = taglen;
    minimac_ctx.nb_ele = 16/TAGLEN;    

    int i,j;
    for(i=0; i<minimac_ctx.nb_ele; i++){
	    for(j=0; j<MSGLEN; j++){
            unsigned char temp = rand();	    
            minimac_ctx.tx_state.msg[i][j] = temp;
            minimac_ctx.rx_state.msg[i][j] = temp;
	    }
	}
}

void minimac_update_state(state_t* state){
    int j;
    for(j=minimac_ctx.nb_ele-1; j>0; j--){
        memcpy( state->msg[j], state->msg[j-1], MSGLEN);
    }

    state->counter++; 
}

void minimac_sign(unsigned char* msg, unsigned char* output, rtimer_clock_t* end_live){

    memcpy(minimac_ctx.tx_state.msg[0], msg, MSGLEN );
    
    dtls_hmac_update(minimac_ctx.ctx,(unsigned char*) &(minimac_ctx.tx_state), 4+minimac_ctx.nb_ele*MSGLEN);
    dtls_hmac_finalize(minimac_ctx.ctx, minimac_ctx.temp);
    
    memcpy( output, minimac_ctx.temp, minimac_ctx.len );

    *end_live = RTIMER_NOW();
    
    minimac_update_state(&(minimac_ctx.tx_state));
    dtls_hmac_init(minimac_ctx.ctx, minimac_ctx.minimac_key, 32);
}

int minimac_vrfy(unsigned char* msg, unsigned char* sig){
    
    memcpy(minimac_ctx.rx_state.msg[0], msg, MSGLEN );
    
    dtls_hmac_update(minimac_ctx.ctx,(unsigned char*) &(minimac_ctx.rx_state), 4+minimac_ctx.nb_ele*MSGLEN);
    dtls_hmac_finalize(minimac_ctx.ctx, minimac_ctx.temp); 
    
    minimac_update_state(&(minimac_ctx.rx_state));
    dtls_hmac_init(minimac_ctx.ctx, minimac_ctx.minimac_key, 32);

    return memcmp( sig, minimac_ctx.temp, minimac_ctx.len );
}

void minimac_deinit(){

    dtls_hmac_free(minimac_ctx.ctx);
}
