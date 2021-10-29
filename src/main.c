/////////////////////////////////////
//
//  rfid-eac
//      set up n serial ports to accept raw data in parallel.
//      intended environment: 
//          arduinos connected via usb to linux host
//              realized as /dev/ttyACMx
//          adafruit pn532 readers connected to the arduinos
//              adafruit [] to multiplex the i2c comms from readers to one uC
//      major issue:
//          physical wiring. walls are highly inconvenient. bit of a gordian knot in the ceiling where i want to be.
//              
// TODO:
//      1. format logs to reports, upload logs
//
////////////////////////////////////

#include "common.h"
#include "portio.h"
#include "logger.h"

/*
    Imported identifiers:
        #define NUM_PORTS

        typedef int PORT;

        typedef struct {
            int ACTIVE;
            int KILLED_THREADS;
        } STATES;

        extern volatile STATES STATE;
*/

// DB MACRO

STATES STATE;

// Signal handlers
// maybe in this handler fill all ports with zero-valued uids?
static void SIGINT_HANDLER() {
    DBPRINT write(STDOUT_FILENO, "\tSIGINT\n", 8); 
    STATE.ACTIVE = 0;
    return;
}

// initial state: active and all threads alive
void setup_state() {
    STATE.ACTIVE = 1;
    STATE.KILLED_THREADS = 0;
} 

int main() {
    // debug checking
    DBPRINTV {
        printf("VERBOSE DEBUG MODE\n");
    } else DBPRINT {
        printf("DEBUG MODE\n");
    }

    // setup ports
    PORT ports[NUM_PORTS]; // TODO: intended implementation has 7
    setup_state();
    setup_ports(ports);
    
    // Set interrupt signal object 
    struct sigaction sa;
    sa.sa_handler = SIGINT_HANDLER;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        printf("!ERROR: SIGINT handle not set\n");
        return -1;
    }

    // open comms
    // creates N threads to handle N readers, each is looped on STATE.ACTIVE
    //  when STATE.ACTIVE is turned off, each thread closes and increments STATE.KILLED_THREADS
    //  when hardcoded number of threads have died, close ports and end program.
    open_com(ports);

    // main loop: stall until all threads are killed
    while (STATE.KILLED_THREADS < NUM_PORTS); 
    DBPRINT printf("UPDATE: all threads killed\n");

    close_ports(ports);
    DBPRINT printf("UPDATE: closed all ports\n");
    close_com();

    return 0;
}
