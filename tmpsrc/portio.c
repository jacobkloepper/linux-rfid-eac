#include "common.h"
#include "portio.h"
#include <semaphore.h>
#include <pthread.h>

/*
    Imported identifiers:
        #define _UID_LENGTH_ = 32 (in common.h)
        #define NUM_PORTS
        
        typedef uint32_t uid;
        typedef int PORT;

        typedef struct {
            int ACTIVE;
            int KILLED_THREADS;
        } STATES;

        extern STATES STATE;
*/


#define OFILENAME "logs/test.csv"
sem_t mutex;


// read from ports, get uint uid val
uid scan(int serial_port) {
    if (serial_port < 0) {
        printf("Error %d opening %d\n", errno, serial_port);
        //printf("Error %d from open\n", errno);
    }
    
    // set up teletypewriter
    struct termios tty;
    if (tcgetattr(serial_port, &tty) != 0) {
        printf("Error %d from tcgetattr on port %d\n", errno, serial_port);
    }

    tty.c_cc[VMIN] = _UID_LENGTH_; 
    cfsetispeed(&tty,B115200);
    
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %d from tcsetattr on port %d\n", errno, serial_port); 
    }
    tcflush(serial_port, TCIFLUSH);

    // Read 4 bytes from port
    uint8_t buf[4];
    int read_return = read(serial_port, &buf, 4); 
    if (read_return != 4) {
        // read failed, just go to next loop
        printf("read failed: read_return = %d\n", read_return);
        printf("contents of buffer: \n[");
        for (int i = 0; i < 4; i++) {
            printf("%u", buf[i]);
            if (i != 3) {
                printf("-");
            }
        }
        printf("]\n");
        //printf("Errno: %s\n", strerror(errno));
        return 0;
    }
    
    // uid is 32 bits, the buffer elements are bytes indexed from MSB
    uid result = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | (buf[3] << 0);

    return result;
}

// PORTS
void setup_ports(PORT* ports) {
    ports[0] = open("/dev/ttyACM0", O_RDONLY); 
    //ports[1] = open("/dev/ttyACM1", O_RDONLY); 

    for (int i = 0; i < NUM_PORTS; i++) {
        if (ports[i] == -1) {
            printf("Error: port %d could not be opened.\n", i);
            exit(1);
        }
    }
}

void close_ports(PORT* ports) {
    for (int i = 0; i < NUM_PORTS; i++) {
        close(ports[i]);
    }
}

uid read_port(PORT serial_port) {
    uid KEY = scan(serial_port);
    return KEY;
}

// ASYNC
void* thread(void* arg) {
    // get a uid
    uid KEY;
    char timebuf[20];

    char KEYSTR[_UID_LENGTH_/4 + 3] = {'\0'};
    while (STATE.ACTIVE) {
        KEY = 0;
        KEY = read_port(*(PORT*)arg);

        // if key read, write (CRITICAL SECTION)
        if (KEY != 0) {
            // setup: get hexstr and open file
            uid_to_hexstring(KEY, KEYSTR);
            str_time(timebuf);
            FILE* fout = fopen(OFILENAME, "a");
            sem_wait(&mutex);
            
            fprintf(fout, "%s,%s\n", timebuf, KEYSTR);
            printf("PRINT: %s,%s\n", timebuf, KEYSTR); // debug

            // takedown: close file
            fclose(fout);
            sem_post(&mutex);
        }
    } 

    STATE.KILLED_THREADS++;

    return NULL;
}


// PUBLIC
// for now arg is unused.
void open_com(PORT* ports) {
    // set up threading
    pthread_t t_acm0, t_acm1;
    sem_init(&mutex, 0, 1);

    // create threads to scan each port
    pthread_create(&t_acm0, NULL, thread, &ports[0]);
    pthread_create(&t_acm1, NULL, thread, &ports[1]);

    // program gets here after SIGINT sent (therefore reads done)
    sem_destroy(&mutex);
}
