#ifndef __COMMON_H__
#define __COMMON_H__

#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

#define _UID_LENGTH_ 32
#define HALFBYTE(val, n) ( ( val & ((0xF0000000) >> (4*n)) ) >> ( 32 - (4*(n+1)) ) )
#define TRUE 1
#define FALSE 0
#define NUM_PORTS 2

typedef uint32_t uid;
typedef int PORT;

typedef struct {
    int ACTIVE; // 
    int KILLED_THREADS;
} STATES;

extern STATES STATE;

// extract nth halfbyte from uid (zero indexed from most significant halfbyte)
// hexstring have size _UID_LENGTH_/4 + 3 (with 4 byte uids, this is 11) 
// to include "0x" prefix and null terminator
static inline void uid_to_hexstring(uid val, char* buf) {
    snprintf(buf, _UID_LENGTH_/4 + 3, "0x%x%x%x%x%x%x%x%x", HALFBYTE(val, 0), HALFBYTE(val, 1), HALFBYTE(val, 2), HALFBYTE(val, 3), HALFBYTE(val, 4), HALFBYTE(val, 5), HALFBYTE(val, 6), HALFBYTE(val, 7)); 
}

// print current time
static inline void str_time(char* obuf) {
    time_t rawtime;
    struct tm* info;

    time(&rawtime);
    info = localtime(&rawtime);

    strftime(obuf, 20, "%x|%X", info);

    return;
}


#endif /* __COMMON_H__ */
