# linux-rfid-eac
Host controller for an RFID logging system. Log key-ins, collect logs on local device and upload to a remote server, and manage user IDs on the same remote server.

Intended to be run on a raspberry pi with arduinos connected via usb.

# Use
Host must be a linux machine with gcc.
The serial ports are configued by a script assuming they take the name `/dev/ttyACMx` like usb-connected arduinos.

If another, more attractive setup should become apparent, this interface should be easy to edit.

Build and run by calling `make` from project root.

# Overview

## data
Holds [`users.csv`](data/users.csv) which pairs usernames to 7-byte UIDs.

Also, contains a blank file `new` when a new user file is uploaded to remote. Once the file is downloaded, `new` is deleted.

## logs
Contains a log `.csv` which is updated on every key-in.

Currently, upload logs to remote by calling `make upload`. This should be done after every key-in.

## src
* [`main.c`](src/main.c) sets up states and signal handlers. Calles `open_com(ports)` which spawns threads to handle incoming data asynchronously.
* [`portio.c`](src/portio.c) handles the ports, for example setting them up, reading, error handling, and closing.
* [`logger.c`](src/logger.c) handles logging, including access of user file to pair users and UIDs.

## Tools
* [`google`](tool/google/) contains google api auth used by following tools.
* [`remote`](tool/remote/) contains scripts to upload logs and download user files from remote.
* [`umod`](tool/umod/) contains a script to update the user file and upload it to remote.