# linux-rfid-eac
Host controller for an RFID logging system. Log key-ins, collect logs on local device and upload to a remote server, and manage user IDs on the same remote server.

Intended to be run on a raspberry pi with arduinos connected via usb.

# Installation
The only installation required is to edit a user's crontab to schedule daily log uploads. This can be done manually or by running `make install` from the project root.
This `make` target simply runs [`tool/sys/cron_setup.sh`](tool/sys/cron_setup.sh).

# Use
Host should be a linux machine. Program *should* be POSIX-compliant everywhere, but I make no guarantees.
The serial ports are configued by a script assuming they take the name `/dev/ttyACMx` like usb-connected arduinos.

If another, more attractive setup should become apparent, this interface should be easy to edit.

Build and run by calling `make` from project root.

# Overview

## data
Holds [`users.csv`](data/users.csv) which pairs usernames to 7-byte UIDs.

Also, contains a blank file `new` when a new user file is uploaded to remote. Once the file is downloaded, `new` is deleted.

## logs
Contains a log `.csv` which is updated on every key-in.

This log is parsed and uploaded as a report on each log update. The scripts in `tool/google/` and `tool/remote/` are set up to control the uploads to a configurable Google Drive. Configuration details are in `tool/google/`.

## firmware
* [`reader.ino`](firmware/reader.ino) is programmed onto the arduinos. `many_readers.ino` was an attempt to set up an i2c bus to have many readers on a single arduino, but the reader-per-port approach is preferable for asynchronous use.

## src
* [`main.c`](src/main.c) sets up states and signal handlers. Calls `open_com(ports)` which spawns threads to handle incoming data asynchronously.
* [`portio.c`](src/portio.c) handles the ports, for example setting them up, reading, error handling, and closing.
* [`logger.c`](src/logger.c) handles logging, including access of user file to pair users and UIDs.

## Tools
* [`google`](tool/google/) contains google api auth used by following tools.
* [`remote`](tool/remote/) contains scripts to upload logs and reports, and download user files from remote.
* [`sys`](tool/sys/) contains scripts to interface with the os.
* [`umod`](tool/umod/) contains a script to update the user file and upload it to remote.
