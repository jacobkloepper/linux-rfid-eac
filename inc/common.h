#ifndef __COMMON_H__
#define __COMMON_H__

// test to force make to rebuild
//#define AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA 0

// ARGUMENTS
#define NUM_PORTS (2)
#define DEBUG (0 || DEBUGV)
#define DEBUGV (1)
#define LOGFILE ("logs/log.csv")
#define USERFILE ("data/users.csv")

// INCLUDES
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

// STATIC DEFINES
#define UID_LENGTH (56)                                                                               // UID length in bits
#define PAYLOAD_LENGTH (8)                                                                            // length of the data packet in bytes
#define HALFBYTE_64(val, n) ( ( val & ((0xF000000000000000LL) >> (4*n)) ) >> ( 64 - (4*(n+1)) ) )     // return the nth (n on [0,7]) halfbyte of 32-bit val
#define DBPRINT if(DEBUG)                                                                             // debug print macro
#define DBPRINTV if (DEBUGV)                                                                          // verbose debug print macro

typedef uint64_t uid;
typedef struct {
    uid UID;
    uint8_t DIRECTION;
} payload;

typedef int PORT;

// program state struct
typedef struct {
    int ACTIVE;              // thread loops run while ACTIVE is set
    int KILLED_THREADS;      // tracks the number of pthreads killed; end main loop when all killed (KILLED_THREADS = NUM_PORTS)
} STATES;

extern STATES STATE;

// fill a string buffer with the hexstring of a 56-bit uid.
// hexstring have size UID_LENGTH/4 (bit -> hexbit) and +1 for '\0'.
static inline void uid_to_hexstring(uid val, char* buf) {
    snprintf(buf,UID_LENGTH/4+1,"%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx",HALFBYTE_64(val,0),HALFBYTE_64(val,1),HALFBYTE_64(val,2),HALFBYTE_64(val,3),HALFBYTE_64(val,4),HALFBYTE_64(val,5),HALFBYTE_64(val,6),HALFBYTE_64(val,7),HALFBYTE_64(val,8),HALFBYTE_64(val,9),HALFBYTE_64(val,10),HALFBYTE_64(val,11),HALFBYTE_64(val,12),HALFBYTE_64(val,13));
}

// print current time
static inline void str_time(char* obuf) {
    time_t rawtime;
    struct tm* info;

    time(&rawtime);
    info = localtime(&rawtime);

    strftime(obuf, 20, "%x,%X", info);

    return;
}


#endif /* __COMMON_H__ */
