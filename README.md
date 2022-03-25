
# SP-MAC

This repository provides a prototype implementation of SP-MAC and the benchmarking against UMAC and hardware-accelerated HMAC-SHA256 in Contiki-NG[1]. SP-MAC is a novel ProMAC resilient to network-level disturbance or interference that is proposed in the paper "Take a Bite of the Reality Sandwich: Revisiting the Security of Progressive Message Authentication Codes", accepted for publication at ACM WiSec'22. Additionally, the code to compute them as well as the computed Golomb Rulers and g-Sidon Sets are provided in a separate directory.

## MAC schemes implementation and benchmarking

### Getting started

To run our BP-MAC implementation, you first need to setup the Contiki-NG [build environment](https://github.com/contiki-ng/contiki-ng/wiki). To recreate the results from the paper, you additionally need special hardware, namely a Zolertia Re-Mote boards. However, you can also run the applcation natively in Linux or within the Cooja simulatation environment.

After you setup the environment, you can copy the spmac folder into the top-level Contiki-NG directory.

### Evaluation Flags

Now you are almost ready to run the code. The final adjustment needs to happen in the Makefile, to adopt it to you specific hardware setup and evaluation goal. The relevant flags that have to be used are the following four:

 - *DWITH_DTLS*: has to be set to 1
 - *DTAGLEN*: lets you choose the length of the evaluated MAC tag
 - *DMSGLEN* sets the length of the evaluated messages

### Run the code

Depending on the hardware, you need slightly different commands to see the output of the application in your terminal

**Native execution on Linux**
```
make TARGET=native client
./client.native
```

**Zolertia ReMote**
```
make TARGET=zoul BOARD=remote-revb savetarget
make client.upload && make login
```

## Golomb Rulers and g-Sidon Sets

The g-sidon-sets directory contains a *rulers.py* file which stores all used Golomb Rulers and g-Sidon Sets. The first index is *g* (the number of times a difference between two distinc elements is allowed) and the index indicates the order of the set. To compute additional sets, *compute_rulers.py* can be used, where the arrays *copies* and *length* can be used to indicate which *g* and *order* you want to look at.

Warning: The computation of some sets used in this paper took weeks on a server-grade CPU!

[1] https://github.com/contiki-ng/contiki-ng
