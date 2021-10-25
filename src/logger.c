#include "common.h"
#include "logger.h"

/* Goal: Accept data in the form of a uid &? loc flag, map to user id and print to log.
 *
 */

#define MAX_NAME_LENGTH (50)
#define MAX_TIME_LENGTH (17)
#define MAX_DIR_LENGTH (4)
#define MAX_USERFILE_LINE_LENGTH (10 + 1 + MAX_NAME_LENGTH + 1)
#define MAX_LOGFILE_LINE_LENGTH (MAX_TIME_LENGTH + 1 + MAX_NAME_LENGTH + 1 + MAX_DIR_LEGNTH + 1)
// The users.csv file is formatted:
// 0xXXXXXXXX,NAME
// max length is 10+1+MNL+1 
// (uid + , + max name length + nullterm)

// The log lines are formatted: 
// DD/MM/YY|HH:MM:SS,NAME,IN/OUT
// max length is MTL+1+MNL+1+MDL+1
// (date-time + , + max name length + , + max dir length + nullterm)

// given a pointer to char array, set the first element
// to NAK (ASCII 21) to denote an error occurred.
void set_str_nak(char* cstr) {
    cstr[0] = 21;
}

// check error on string
// returns 0 if nak (error)
int check_str_nak(char* cstr) {
    if (cstr[0] == 21) {
        return 0;
    } else {
        return 1;
    }
}

// set the first \n to \0 in name string
// purpose: remove nl on fgets return string that is statically over-allocated.
void set_nl_to_null(char* cstr) {
    for (int i = 0; i < MAX_NAME_LENGTH; i++) {
        if (cstr[i] == '\n') {
            cstr[i] = '\0';
            break;
        }
    }
}

// char* obuf is a char array of size MAX_NAME_LENGTH
void map_uid_to_name(uid UID, char* obuf) {
    int SUCCEEDED = 0;

    // Buffers for lines of userfile and UID token
    char flinebuf[MAX_USERFILE_LINE_LENGTH];
    char uidbuf[11];
    
    // get uidstr from UID
    char uidstr[11];
    uid_to_hexstring(UID, uidstr);
    DBPRINTV printf("UPDATE: UID = %llu | uidstr = %s\n", UID, uidstr);
    
    // read from file, parse lines
    FILE* fin = fopen(USERFILE, "r");
    
    // if first token matches uid, set obuf to second token.
    while(fgets(flinebuf, MAX_USERFILE_LINE_LENGTH, fin) != NULL) {
        strncpy(uidbuf, flinebuf, 10);
        uidbuf[10] = '\0';

        if (strcmp(uidbuf, uidstr) == 0) {
            // this line matches, set output buffer to name
            strncpy(obuf, flinebuf+11, MAX_NAME_LENGTH-1);
            set_nl_to_null(obuf);
            SUCCEEDED = 1;

            //DBPRINTV printf("UPDATE: flinebuf = %s\tuidbuf = %s\n", flinebuf, uidbuf);
            break;
        }

        //DBPRINTV printf("UPDATE: flinebuf = %s\tuidbuf = %s\n", flinebuf, uidbuf);
    }

    fclose(fin);

    if (!SUCCEEDED) {
        set_str_nak(obuf);
        DBPRINT printf("!ERROR: failed to get username\n");
    } 
}

void set_dir(uint8_t DIRECTION, char* obuf) {
    if (DIRECTION == 1) {
        strncpy(obuf, "in", MAX_DIR_LENGTH);
        obuf[MAX_DIR_LENGTH-1] = '\0';
    } else if (DIRECTION == 0) {
        strncpy(obuf, "out", MAX_DIR_LENGTH);
        obuf[MAX_DIR_LENGTH-1] = '\0';
    } else {
        printf("!ERROR: invalid direction in set_dir(): %u\n", DIRECTION);
    }
}

// this func is called from threads in portio and is mutexed.
void update_log(payload DATA) {
    char username[MAX_NAME_LENGTH];
    char timebuf[MAX_TIME_LENGTH]; 
    char dirbuf[MAX_DIR_LENGTH];
    map_uid_to_name(DATA.UID, username);
    str_time(timebuf);
    set_dir(DATA.DIRECTION, dirbuf);

    // print to log if valid username
    if (check_str_nak(username)) {
        FILE* fout = fopen(LOGFILE, "a");

        fprintf(fout, "%s,%s,%s\n", timebuf, username, dirbuf);
        DBPRINT printf("-PRINT: %s,%s,%s\n", timebuf, username, dirbuf);

        // resolution
        fclose(fout);
    }
}
