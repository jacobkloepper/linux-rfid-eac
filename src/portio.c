#include "common.h"
#include "portio.h"
#include "logger.h"
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

#define MAX_PORTS (7)
sem_t mutex;

// dynamic assignments
char ALL_PORTS[MAX_PORTS][15] = {"/dev/ttyACM0", "/dev/ttyACM1", "/dev/ttyACM2"};
pthread_t ALL_THREADS[NUM_PORTS] = {0};

// read from ports, get uint uid val
uid scan(int serial_port) {
    if (serial_port < 0) {
        printf("\t!!!ERROR!!! %d opening %d\n", errno, serial_port);
    }
    
    // set up teletypewriter
    struct termios tty;
    if (tcgetattr(serial_port, &tty) != 0) {
        printf("\t!!!ERROR!!! %d from tcgetattr on port %d\n", errno, serial_port);
    }

    tty.c_cc[VMIN] = _UID_LENGTH_; 
    cfsetispeed(&tty,B115200);
    
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("\t!!!ERROR!!! %d from tcsetattr on port %d\n", errno, serial_port); 
    }
    tcflush(serial_port, TCIFLUSH);

    // Read 4 bytes from port
    uint8_t buf[4];
    int read_return = read(serial_port, &buf, 4); 

    if (read_return < 4) {
        // read failed, just go to next loop
        printf("\t!!!ERROR!!!: failed read -- got %d byte(s)\n", read_return);
        printf("\tBUFFER DUMP: [");
        for (int i = 0; i < 4; i++) {
            printf("%u", buf[i]);
            if (i != 3) {
                printf("-");
            }
        }
        printf("]\n");
        return 0;
    }
    
    // uid is 32 bits, the buffer elements are bytes indexed from MSB
    // shift the bytes and OR to fill the 32 bit value.
    uid result = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | (buf[3] << 0);

    return result;
}


// PORTS
void setup_ports(PORT* ports) {
    for (int i = 0; i < NUM_PORTS; i++) {
        ports[i] = open(ALL_PORTS[i], O_RDONLY);
        DBPRINT printf("--OPEN: Port %d\n", i);

        if (ports[i] == -1) {
            printf("\t!!!ERROR!!!: Port %d could not be opened.\n", i);
            exit(1);
        }
    }
}

void close_ports(PORT* ports) {
    for (int i = 0; i < NUM_PORTS; i++) {
        close(ports[i]);
        DBPRINT printf("-CLOSE: port %d\n", i);
    }
}

uid read_port(PORT serial_port) {
    DBPRINT printf("-START: scanning serial %d\n", serial_port);
    uid KEY = scan(serial_port);
    DBPRINT printf("--READ: on port %d\n", serial_port);
    return KEY;
}

// ASYNC
void* thread(void* arg) {
    // get a uid
    uid KEY;

    while (STATE.ACTIVE) {
        KEY = 0xFFFFFFFF;
        KEY = read_port(*(PORT*)arg);

        // if key read, write to log (CRITICAL SECTION)
        if (KEY != 0) {
            sem_wait(&mutex);
            update_log(KEY);
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
    sem_init(&mutex, 0, 1);

    // create threads to scan each port
    for (int i = 0; i < NUM_PORTS; i++) {
        pthread_create(&ALL_THREADS[i], NULL, thread, &ports[i]);
        DBPRINT printf("-SPAWN: thread for port %d\n", i);
    }

}

void close_com() {
    // program gets here after SIGINT sent (therefore reads done)
    DBPRINT printf("UPDATE: destroying mutex\n");
    sem_destroy(&mutex);
}
