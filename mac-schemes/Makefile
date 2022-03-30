CONTIKI=..
CONTIKI_PROJECT = client
TINYDTLS_PATH := os/net/security/tinydtls

MAKE_WITH_DTLS ?= 1

ifeq (${wildcard $(CONTIKI)/$(TINYDTLS_PATH)/Makefile},)
${error Could not find the tinyDTLS submodule. Please run "git submodule update --init" and try again}
endif

CFLAGS += -DWITH_DTLS=1 -DTAGLEN=1 -DMSGLEN=50 -O2

MODULES += mac-schemes/tinydtls-support
MODULES += $(TINYDTLS_PATH) ${addprefix $(TINYDTLS_PATH)/,aes sha2 ecc}


PROJECT_SOURCEFILES += truncated.c minimac.c whips.c spmac.c cumac.c

include $(CONTIKI)/Makefile.include

cleanall: clean
	rm -f symbols.c symbols.h *.wismote

.PHONY: cleanall
