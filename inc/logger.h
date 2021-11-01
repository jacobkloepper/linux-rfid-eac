#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "common.h"

void set_str_nak(char* cstr);
int check_str_nak(char* cstr);
void map_uid_to_name(uid UID, char* obuf);
void update_userfile();

void update_log(payload DATA);

#endif /* __LOGGER_H__ */
