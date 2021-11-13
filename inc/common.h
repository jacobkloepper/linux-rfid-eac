#ifndef __COMMON_H__
#define __COMMON_H__

// ARGUMENTS
#define NUM_PORTS (2)                       // number of /dev/ttyACMx ports used
#define DEBUG (0 || DEBUGV)                 // regular db mode (db printfs)
#define DEBUGV (1)                          // verbose db mode (extra db printfs)
#define DBREPORT (1)                        // 0: do not upload. 1: do upload.
#define LOGFILE ("logs/log.csv")            // path from project root to log file
#define USERFILE ("data/users.csv")         // path from project root to user data file

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
#include <semaphore.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

// STATIC DEFINES
#define UID_LENGTH (56)                                                                               // UID length in bits
#define PAYLOAD_LENGTH (8)                                                                            // length of the data packet in bytes
#define MAX_PORTS (7)
#define HALFBYTE_64(val, n) ( ( val & ((0xF000000000000000LL) >> (4*n)) ) >> ( 64 - (4*(n+1)) ) )     // return the nth (n on [0,15]) halfbyte of 64-bit val
#define DBPRINT if(DEBUG)                                                                             // debug print macro
#define DBPRINTV if (DEBUGV)                                                                          // verbose debug print macro

// TYPEDEFS
typedef uint64_t uid;           // a uid is 56 bits --- stick it in a uint64_t
typedef struct {
    uid UID;
    uint8_t DIRECTION;
} payload;                      // a payload from serial input is a uid and a direction flag byte

typedef int PORT;               // serial port file descriptors

// program state struct
typedef struct {
    int ACTIVE;                 // thread loops run while ACTIVE is set
    int KILLED_THREADS;         // tracks the number of pthreads killed; end main loop when all killed (KILLED_THREADS = NUM_PORTS)
} STATES;

extern STATES STATE;

// fill a string buffer with the hexstring of a 56-bit uid.
// hexstring have size UID_LENGTH/4 (bit -> hexbit) and +1 for '\0'.
static inline void uid_to_hexstring(uid val, char* buf) {
    snprintf(buf,UID_LENGTH/4+1,"%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx%llx",HALFBYTE_64(val,0),HALFBYTE_64(val,1),HALFBYTE_64(val,2),HALFBYTE_64(val,3),HALFBYTE_64(val,4),HALFBYTE_64(val,5),HALFBYTE_64(val,6),HALFBYTE_64(val,7),HALFBYTE_64(val,8),HALFBYTE_64(val,9),HALFBYTE_64(val,10),HALFBYTE_64(val,11),HALFBYTE_64(val,12),HALFBYTE_64(val,13));
}

// fill obuf with current date.
// obuf should be initialized to size MAX_TIME_LENGTH defined in inc/logger.h
static inline void str_time(char* obuf) {
    time_t rawtime;
    struct tm* info;

    time(&rawtime);
    info = localtime(&rawtime);

    sprintf(obuf, "%02d/%02d/%02d,%02d:%02d:%02d", info->tm_mday, info->tm_mon+1, info->tm_year-100, info->tm_hour, info->tm_min, info->tm_sec);

    return;
}

// return file size in bytes
// taken from stackoverflow.com/questions/8236
static inline off_t fsize(const char *filename) {
    struct stat st;

    if (stat(filename, &st) == 0) {
        return st.st_size;
    } 

    printf("-ERROR: could not get size of \"%s\"\n", filename);
    
    return -1;
}



#endif /* __COMMON_H__ */
