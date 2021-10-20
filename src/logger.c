#include "common.h"
#include "logger.h"

/* Goal: Accept data in the form of a uid &? loc flag, map to user id and print to log.
 *
 */

#define MAX_NAME_LENGTH (50)
#define MAX_TIME_LENGTH (17)
#define MAX_USERFILE_LINE_LENGTH = (10 + 1 + MAX_NAME_LENGTH + 1)
#define MAX_LOGFILE_LINE_LENGTH = (MAX_TIME_LENGTH + 1 + MAX_NAME_LENGTH + 1)
// The users.csv file is formatted:
// 0xXXXXXXXX,NAME
// max length is 10+1+MNL+1 
// (uid + , + max name length + null term)

// The log lines are formatted: 
// DD/MM/YY|HH:MM:SS,NAME
// max length is 17+1+MNL
// (date-time + , + max name length)

// given a pointer to char array, set the first element
// to NAK (ASCII 21) to denote an error occurred.
void set_str_nak(char* cstr) {
    cstr[0] = 21;
}

// check error on string
// returns 1 if nak (error)
int check_str_nak(char* cstr) {
    if (cstr[0] == 21) {
        return 1;
    } else {
        return 0;
    }
}

// char* obuf is a char array of size MAX_NAME_LENGTH
void map_uid_to_name(uid UID, char* obuf) {
    


    //if (failed) {
    //    set_str_nak(name);
    //} 
}

// this func is called from threads in portio and is mutexed.
void update_log(uid UID) {
    char username[MAX_NAME_LENGTH];
    char timebuf[MAX_TIME_LENGTH]; 
    map_uid_to_name(UID, username);
    str_time(timebuf);

    // print to log
    FILE* fout = fopen(LOGFILE, "a");

    fprintf(fout, "%s,%s\n", timebuf, username);
    DBPRINT printf("-PRINT: %s,%s\n", timebuf, username);

    // resolution
    fclose(fout);
}

// given a ref to a file pointer, return an array of strings, one for each line in the file.
// it is assumed the file is working.
// NOTE: I don't need a full-blown csv api, I just need to give a uid and get a string.
// NOTE: I can do a lighter implementation.
//char** csv_reader(FILE** fp) {
//
//}

void csv_writer(char* line, FILE** fp) {

}
