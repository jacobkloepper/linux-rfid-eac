import serial
import sys
import subprocess
from time import sleep

DEBUG = False
#DEBUG = True

def read_rfid(cmd):
    # First, remove any RFID tag near reader
    print("Remove RFID tags from reader\n")

    # Verify COM setup
    print("Checking COM port...", flush=True)
    good_connection = False
    while (good_connection == False):
        try:
            reader = serial.Serial(port='COM3', baudrate=115200, stopbits=serial.STOPBITS_TWO)
            print("Connection good.")
            good_connection = True
        except:
            input("Could not read serial port. Check connection and try again. Press enter to continue.")

    print()

    # Get name
    if (cmd == "usermod"):
        wait_for_input = True
        while (wait_for_input):
            try:
                (firstname, lastname) = input("Input user name...\n").split()
                wait_for_input = False
            except ValueError:
                print("Format: Firstname Lastname")

        print()

    # Get ID 
    print("Present RFID tag to reader", flush=True)
    reader.reset_input_buffer()
    byte_data = reader.read(size=32)


    # Turn ID into hexstring. hexstring is what usermod.py cares about. It can be thought of as a return value.
    bitstring = str(byte_data, 'utf-8')
    decimal = int(bitstring, 2)
    hexstring = "{0:0{1}x}".format(decimal, 8)

    print("")

    if (DEBUG):
        print("User: {} {}".format(firstname, lastname))
        print("  ID: {}".format(hexstring))

    reader.reset_input_buffer()
    reader.close()

    if (cmd == "usermod"):
        return (firstname, lastname, hexstring)
    elif (cmd == "check"):
        return hexstring