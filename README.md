# linux-rfid-eac
Host controller for an RFID logging system. Log key-ins, collect logs on local device, and (WIP) upload logs to a remote, where logs can be formatted into a report.

# Overview
## Data
[`users.csv`](data/users.csv) pairs users to 4-byte RFID keys.

TODO:
1. Some sort of encryption?
2. Integrate that python module to update the db simply.

[`logs`](logs/) contains logs. Simple interface, append a new line to the current log.

TODO:
1. Some filesize protection. Create new log file when > X bytes.
2. Some sort of time logging. Maybe append a "DD/MM/YYYY" line on midnight.

## Source
* [main](src/main.c) sets up states and interrupt signal to safely exit main loop. Calls `open_com(PORT* ports)` which spawns threads to handle reads off terminals and mutex log writes.

* [portio](src/portio.c) handles everything at the port end, such as setting up ttys, reading, error handling, and exiting.

* [logger](src/logger.c) handles local logging.

* [uploader](src/uploader.c) (WIP) handles uploading logs to a remote.
