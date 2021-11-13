#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "common.h"

#define LOGDIR ("logs/")
#define HEXUID_LENGTH (15)
#define MAX_NAME_LENGTH (50)
#define MAX_TIME_LENGTH (17)
#define MAX_DIR_LENGTH (4)
#define MAX_USERFILE_LINE_LENGTH (10 + 1 + MAX_NAME_LENGTH + 1)
#define MAX_LOGFILE_LINE_LENGTH (MAX_TIME_LENGTH + 1 + MAX_NAME_LENGTH + 1 + MAX_DIR_LEGNTH + 1)
#define MAX_FILESIZE_BYTES (10000000)

// The users.csv file is formatted:
// 0xXXXXXXXX,NAME
// max length is 10+1+MNL+1 
// (uid + , + max name length + nullterm)

// The log lines are formatted: 
// DD/MM/YY|HH:MM:SS,NAME,IN/OUT
// max length is MTL+1+MNL+1+MDL+1
// (date-time + , + max name length + , + max dir length + nullterm)

void set_str_nak(char* cstr);
int check_str_nak(char* cstr);
void map_uid_to_name(uid UID, char* obuf);
void update_userfile();
void update_report();
void* rp_thread();


void fcpy(const char* fin, const char* fout);
void fclear(const char* fn);
unsigned int get_lognum();
void buffer_log();

void update_log(payload DATA);

#endif /* __LOGGER_H__ */
