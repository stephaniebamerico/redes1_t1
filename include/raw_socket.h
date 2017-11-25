#ifndef __RAW_SOCKET__
#define __RAW_SOCKET__

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <net/ethernet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

int openRawSocket(char *device);

#endif