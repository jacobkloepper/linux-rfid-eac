#include "common.h"

// port stuff
void setup_ports(PORT* ports);
void close_ports(PORT* ports);
uid read_port(PORT serial_port);

// serial stuff
void* thread(void* arg);

// 'main'
void open_com(PORT* ports);
