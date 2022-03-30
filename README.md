
# SP-MAC

This repository provides a prototype implementation of SP-MAC and the benchmarking against UMAC and hardware-accelerated HMAC-SHA256 in Contiki-NG[1]. SP-MAC is a novel ProMAC resilient to network-level disturbance or interference that is proposed in the paper "Take a Bite of the Reality Sandwich: Revisiting the Security of Progressive Message Authentication Codes", accepted for publication at ACM WiSec'22. Additionally, the code to compute them as well as the computed Golomb Rulers and g-Sidon Sets are provided in a separate directory.

## MAC schemes implementation and benchmarking

### Getting started

To run our SP-MAC implementation, you first need to setup the Contiki-NG [build environment](https://github.com/contiki-ng/contiki-ng/wiki). To recreate the results from the paper, you additionally need special hardware, namely a Zolertia Re-Mote boards. However, you can also run the application natively in Linux or within the Cooja simulatation environment.

After you setup the environment, you can copy the *mac-schemes* folder into the top-level Contiki-NG directory.

#### Known Error Messages

* On macOS, we ran into the problem that `git submodule update --init --recursive` did nothing after cloning the contiki-ng repository. Using `git clone --recursive https://github.com/contiki-ng/contiki-ng.git` when cloning makes that step unnecessary and did work for us in that case.

* If you run contiki-ng via docker, as described in the Contiki-NG documentation, you might see the warning `WARNING: The requested image's platform (linux/386) does not match the detected host platform (linux/amd64) and no specific platform was requested`. This is apparently a bug in docker and the warning can be ignored.

### Evaluation Flags

Now you are ready to run the code. You can however still make adjustmets in the Makefile, to adopt it to your evaluation goal. The relevant `CFLAGS` that have to be used are the following three:

 - *DWITH_DTLS*: has to be set to 1
 - *DTAGLEN*: lets you choose the length of the evaluated MAC tag (1,2,4 are possible)
 - *DMSGLEN* sets the length of the evaluated messages

### Run the code

Depending on the hardware, you need slightly different commands to see the output of the application in your terminal

**Native execution on Linux**
```
make TARGET=native client
./client.native
```
*Note: As most Linux computers are way more powerful than the targeted hardware, you can expect that all measured timing are 0*

**Zolertia ReMote**
```
make TARGET=zoul BOARD=remote-revb savetarget
make client.upload && make login
```

## Golomb Rulers and g-Sidon Sets

The g-sidon-sets directory contains a *rulers.py* file which stores all used Golomb Rulers and g-Sidon Sets. The first index is *g* (the number of times a difference between two distinc elements is allowed) and the index indicates the order of the set. To compute additional sets, *compute_rulers.py* can be used, where the arrays *copies* and *length* can be used to indicate which *g* and *order* you want to look at.

Warning: The computation of some sets used in this paper took weeks on a server-grade CPU!

[1] https://github.com/contiki-ng/contiki-ng
