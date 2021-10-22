# linux-rfid-eac
Host controller for an RFID logging system. Log key-ins, collect logs on local device, (WIP) format logs into a report, and upload logs and reports to a remote.
Intended to be used on a raspberry pi with arduinos connected via usb. 

# Use
Run by calling `make` on a linux machine with gcc and which supports threading. 

The serial ports are configured by a script which assumes they take the name `/dev/ttyACMx` like usb-connected arduinos.

If another, more attractive setup should become apparent, this interface is easy to edit.

# Overview
## Data
[`users.csv`](data/users.csv) pairs users to 4-byte RFID keys.

TODO:
1. Integrate that python module to update the db simply.

[`logs`](logs/) contains logs. Simple interface, append a new line to the current log.

TODO:
1. Some filesize protection. Create new log file when > X bytes.

## Source
* [`main`](src/main.c) sets up states and interrupt signal to safely exit main loop. Calls `open_com(PORT* ports)` which spawns threads to handle reads off terminals and mutex log writes.

* [`portio`](src/portio.c) handles everything at the port end, such as setting up ttys, reading, error handling, and exiting.

* [`logger`](src/logger.c) handles local logging.

## Tools
* [`setup.sh`](tool/setup.sh) configures the serial ports to accept raw input.
* [`uploader`](tool/uploader/uploader.py) handles uploading logs to a google drive.

