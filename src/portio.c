///////////////////////////////////////////////////////////////////////////////////////////////////
//
// portio.c
//
//  purpose: to set up and interface with the serial ports on the host.
//  ports are scanned asynchronously, filling a 64-bit payload buffer from the uC outputs.
//  
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "portio.h"
#include "logger.h"

/*
    Imported identifiers:
        #define UID_LENGTH = 56 
        #define PAYLOAD_LENGTH = 8
        #define NUM_PORTS
        
        typedef uint64_t uid;
        typedef int PORT;

        typedef struct {
            int ACTIVE;
            int KILLED_THREADS;
        } STATES;

        extern STATES STATE;
*/

sem_t mutex;
sem_t datamutex;

// dynamic assignments
char ALL_PORTS[MAX_PORTS][15] = {"/dev/ttyACM0", "/dev/ttyACM1", "/dev/ttyACM2", "/dev/ttyACM3", "/dev/ttyACM4", "/dev/ttyACM5", "/dev/ttyACM6"};
pthread_t ALL_THREADS[NUM_PORTS] = {0};

// read from ports, get uint uid val
payload scan(int serial_port) {
    // initialize return value 
    payload result;
    result.UID = 0;
    result.DIRECTION = 9;

    if (serial_port < 0) {
        printf("!ERROR: %d opening %d\n", errno, serial_port);
    }
    
    // set up teletypewriter
    struct termios tty;
    if (tcgetattr(serial_port, &tty) != 0) {
        printf("!ERROR: %d from tcgetattr on port %d\n", errno, serial_port);
    }

    tty.c_cc[VMIN] = UID_LENGTH; 
    cfsetispeed(&tty,B115200);
    
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("!ERROR: %d from tcsetattr on port %d\n", errno, serial_port); 
    }
    tcflush(serial_port, TCIFLUSH);

    // Read 8 bytes from port
    uint8_t buf[PAYLOAD_LENGTH];
    int read_return = read(serial_port, &buf, PAYLOAD_LENGTH); 

    if (read_return < PAYLOAD_LENGTH) {
        // read failed, just go to next loop
        printf("!ERROR: failed read -- got %d byte(s)\n", read_return);
        printf("\tBUFFER DUMP: [");
        for (int i = 0; i < PAYLOAD_LENGTH; i++) {
            printf("%u", buf[i]);
            if (i != 3) {
                printf("-");
            }
        }
        printf("]\n");
        return result;
    }

    // extract direction byte
    switch (buf[PAYLOAD_LENGTH-1]) {
        case 0xFF:
            // entering
            result.DIRECTION = 1;
            DBPRINTV printf("UPDATE: set direction entering\n");
            break;
        case 0x88:
            // exiting
            result.DIRECTION = 0;
            DBPRINTV printf("UPDATE: set direction exiting\n");
            break;
        default:
            // error
            DBPRINT printf("!ERROR: invalid direction byte: %u\n", buf[PAYLOAD_LENGTH-1]);
    }
    
    // uid is 56 bits, the buffer elements are bytes indexed from MSB
    // shift the bytes and OR to fill the 56 bit value (NOTE: we IGNORE the last byte, which denotes direction (in/out).
    // note: buf[] is an array of uint8_t so we cast each element to uint64_t before lshifting. 
    result.UID = ((uint64_t)buf[0] << 56) | ((uint64_t)buf[1] << 48) | ((uint64_t)buf[2] << 40) | ((uint64_t)buf[3] << 32) | ((uint64_t)buf[4] << 24) | ((uint64_t)buf[5] << 16) | ((uint64_t)buf[6] << 8);
    DBPRINTV {
        char dbbuf[UID_LENGTH] = {0};
        uid_to_hexstring(result.UID, dbbuf);
        printf("UPDATE: read %s\n", dbbuf);
    }

    return result;
}


// PORTS
void setup_ports(PORT* ports) {
    for (int i = 0; i < NUM_PORTS; i++) {
        ports[i] = open(ALL_PORTS[i], O_RDONLY);
        DBPRINT printf("--OPEN: Port %d\n", i);

        if (ports[i] == -1) {
            printf("!ERROR: Port %d could not be opened.\n", i);
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

payload read_port(PORT serial_port) {
    DBPRINTV printf("-START: scanning serial %d\n", serial_port);
    payload DATA = scan(serial_port);
    DBPRINTV printf("--READ: on port %d\n", serial_port);
    return DATA;
}

// ASYNC
// TODO: dynamic paths
void get_new_userfile() {
    sem_wait(&datamutex);
    if (access("data/new", F_OK) == 0) {
        DBPRINTV printf("UPDATE: new userfile available\n");
        update_userfile();
    } else {
        //DBPRINTV printf("UPDATE: new userfile unavailable\n");
    }
    sem_post(&datamutex);
}

void* thread(void* arg) {
    // get a payload
    payload DATA;
    get_new_userfile();

    while (STATE.ACTIVE) {
        // get payload
        DATA = read_port(*(PORT*)arg);
        
        // check if new user file available
        get_new_userfile();

        // if key read, write to log (CRITICAL SECTION)
        if (DATA.UID != 0) {
            sem_wait(&mutex);
            sem_wait(&datamutex);
            update_log(DATA);
            sem_post(&datamutex);
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
    sem_init(&datamutex, 1, 1);

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
    sem_destroy(&datamutex);
}
