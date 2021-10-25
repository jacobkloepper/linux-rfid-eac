#include "common.h"

// port stuff
void setup_ports(PORT* ports);
void close_ports(PORT* ports);
payload read_port(PORT serial_port);

// serial stuff
void* thread(void* arg);

// com commands
void open_com(PORT* ports);
void close_com();
