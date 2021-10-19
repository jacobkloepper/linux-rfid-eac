// GOAL: Go back to naive implementation, see what's up

#include <stdio.h>
#include "common.h"
#include "portio.h"

// MACRO
#define DEBUG 1
#define DBPRINT if(DEBUG)

STATES STATE;

// Signal handlers
// maybe in this handler fill all ports with zero-valued uids?
static void SIGINT_HANDLER(int signum) {
    DBPRINT write(STDOUT_FILENO, "SIGINT\n", 8); 
    STATE.ACTIVE = 0;
    return;
}

//char* check_uid(char* uid) {
//}

void setup_state() {
    STATE.ACTIVE = 1;
    STATE.KILLED_THREADS = 0;
} 

int main() {
    // the scan call opens and closes the provided port, returning the 
    // read unique identifier (typedef uid := unsigned long)
    PORT ports[2]; // intended implementation has 7
    setup_state();
    setup_ports(ports);
    
    // Set interrupt signal object 
    struct sigaction sa;
    sa.sa_handler = SIGINT_HANDLER;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        puts("SIGINT handle not set.");
        return -1;
    }

    // open comms
    // creates N threads to handle N readers, each is looped on STATE.ACTIVE
    //  when STATE.ACTIVE is turned off, each thread closes and increments STATE.KILLED_THREADS
    //  when hardcoded number of threads have died, close ports and end program.
    open_com(ports);

    // main loop: wait until all threads are killed
    while (STATE.KILLED_THREADS < 2); 

    close_ports(ports);

    return 0;
}
