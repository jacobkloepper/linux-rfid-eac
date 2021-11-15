#include "common.h"
#include "logger.h"

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

// run python script to download users.csv from google drive.
// this func called when data/new is found to exist.
// data/new is created whenever a new userfile is uploaded.
// data/new is deleted at the end of this func.
void update_userfile() {
    DBPRINT printf("UPDATE: Downloading user file\n");
    FILE* dl_p = popen("sh tool/remote/wrappers/run-dl.sh", "r");
    pclose(dl_p);

    // delete new file
    FILE* rm_p = popen("rm  data/new", "r");
    pclose(rm_p);

    // TODO: check conds on popen?

}

// run python script to generate and upload report to google drive.
// func called on each log update.
void update_report() {
    if (DBREPORT) {
        pthread_t th;
        pthread_create(&th, NULL, rp_thread, NULL);
    } else {
        printf("UPDATE: Skipping report upload\n");
    }
}

void* rp_thread() {
    DBPRINT printf("UPDATE: Uploading new report\n");
    FILE* up_p = popen("sh tool/remote/wrappers/run-rp.sh", "r");
    pclose(up_p);
    DBPRINT printf("UPDATE: Uploaded new report\n");
    return NULL;
}

// char* obuf is a char array of size MAX_NAME_LENGTH
void map_uid_to_name(uid UID, char* obuf) {
    int SUCCEEDED = 0;

    // Buffers for lines of userfile and UID token
    char flinebuf[MAX_USERFILE_LINE_LENGTH];
    char uidbuf[HEXUID_LENGTH];
    
    // get uidstr from UID
    char uidstr[HEXUID_LENGTH];
    uid_to_hexstring(UID, uidstr);
    DBPRINTV printf("UPDATE: UID = %llu | uidstr = %s\n", UID, uidstr);
    
    // read from file, parse lines
    FILE* fin = fopen(USERFILE, "r");
    
    // if first token matches uid, set obuf to second token.
    while(fgets(flinebuf, MAX_USERFILE_LINE_LENGTH, fin) != NULL) {
        strncpy(uidbuf, flinebuf, HEXUID_LENGTH-1);
        uidbuf[HEXUID_LENGTH-1] = '\0';

        if (strcmp(uidbuf, uidstr) == 0) {
            // this line matches, set output buffer to name
            strncpy(obuf, flinebuf+HEXUID_LENGTH, MAX_NAME_LENGTH-1);
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

// copy file with path fn_in to fn_out
void fcpy(const char* fn_in, const char* fn_out) {
    FILE* fin = fopen(fn_in, "r");
    FILE* fout = fopen(fn_out, "w");

    if (fin == NULL) {
        printf("-ERROR: Could not open input file [%s]\n", fn_in);
        exit(1);
    }
    if (fout == NULL) {
        fclose(fin);
        printf("-ERROR: Could not open output file [%s]\n", fn_out);
        exit(1);
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, fin)) != -1) {
        fwrite(line, nread, 1, fout);
    }

    free(line);

    fclose(fin);
    fclose(fout);
}

// clear a file with name fn
void fclear(const char* fn) {
    FILE* fp = fopen(fn, "w");
    if (fp == NULL) {
        printf("-ERROR: could not open file to clear\n");
        exit(1);
    }
    fclose(fp);
}

// get number from cache, update cache (+1)
unsigned int get_lognum() {
    unsigned int lognum = 0;

    const char* fn_cache = "logs/cache.txt";
    FILE* fcache = fopen(fn_cache, "r");
    if (fcache == NULL) {
        printf("-ERROR: Could not open cache for reads\n");
        // TODO: do some handling instead of crashing
        exit(1);
    }
    fscanf(fcache, "%u", &lognum);
    freopen(fn_cache, "w", fcache);
    if (fcache == NULL) {
        printf("-ERROR: Could not open cache for writes\n");
        // TODO: do some handling instead of crashing
        exit(1);
    }
    fprintf(fcache, "%u", lognum+1);
    fclose(fcache);

    return lognum;
}

void buffer_log() {
    char buf_fn[50];

    snprintf(buf_fn, 49, "%s%u_%s", LOGDIR, get_lognum(), "log.csv");

    fcpy(LOGFILE, buf_fn);
    fclear(LOGFILE);
}

// this func is called from threads in portio and is mutexed.
void update_log(payload DATA) {
    // first, check log size
    // if above 10MB, buffer log and create new one
    if (fsize(LOGFILE) > MAX_FILESIZE_BYTES) {
        printf("UPDATE: buffering log\n");
        buffer_log();
    }
    
    // get elements of log line
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
    } else {
        DBPRINT printf("-ERROR: failed to set username\n");
    }

    // after printing to log, generate and upload new report
    update_report();

}
