import os
import sys
import argparse

import user
import usermod_ux
import data_handler

__DEBUG__ = True
#__DEBUG__ = False

def um_add(user):
    h.add_user(user)

def um_edit_rm(user, id):
    h.edit_user(user, "rm", id)

def um_edit_add(user, id):
    h.edit_user(user, "add", id)


# Parser stuff:
p = argparse.ArgumentParser(description='Edit user file for RFID logging.')
p.add_argument('--add', metavar=('<firstname>','<lastname>','<id>'), nargs=3, help='add a user')
p.add_argument('--rm', metavar=('<firstname>','<lastname>'), nargs=2, help='remove a user')
p.add_argument('--edit', metavar=('<firstname>','<lastname>','{add | rm}', '<id>'), nargs=4, type=str, help='edit a user')
p.add_argument('--clear', action='store_true', help='delete all users')
p.add_argument('--check', action='store_true', help='check id on rfid tag')
args = p.parse_args()

# Processing:
h = data_handler.data_handler()

# ADD A USER
if (args.add):
    print("Adding user with name [{} {}] and id [{}]...".format(args.add[0], args.add[1], args.add[2]))
    u = user.User(firstname=args.add[0], lastname=args.add[1], id=args.add[2])
    h.add_user(u)
    
# REMOVE USER
elif (args.rm):
    print("Removing user with name [{} {}]...".format(args.rm[0], args.rm[1]))
    u = user.User(firstname=args.rm[0], lastname=args.rm[1])
    h.rm_user(u)
    
# EDIT USER
elif (args.edit):
    action = "add" if args.edit[2]=="add" else "rm"
    print("Editing user with name [{} {}] to {} the id [{}]...".format(args.edit[0], args.edit[1], action, args.edit[3]))
    u = user.User(firstname=args.edit[0], lastname=args.edit[1])
    h.edit_user(u, action, args.edit[3])
    
# CHECK TAG
elif (args.check):
    reading = True
    while (reading == True):
        print("Checking ID...\n")
        hexstring = usermod_ux.read_rfid("check")

        lstr = list(hexstring)
        
        # Put spaces between each byte
        for i in range(len(lstr)+int(len(lstr)/2)):
            if (i%3 == 0 and i > 0):
                lstr.insert(i-1,' ')

        formatted = ''.join(lstr)

        print("ID read: {}\n".format("".join(formatted)))
        y = input("Would you like to read another tag? (y/n) ")

        if (y == 'n'):
            reading = False
            print("Exiting program...")


# CLEAR TABLE
elif (args.clear):
    y = input("Are you sure? (y/n) ")
    
    if (y == "y"):
        print("Clearing table...")
        h.clear_table()

 # NO ARGS GIVEN, run the UX script
else:
    (firstname, lastname, hexstring) = usermod_ux.read_rfid("usermod")
    u = user.User(firstname=firstname, lastname=lastname, id=hexstring)

    # If user not in table, add them
    if (not h.in_table(u)):
        print("Adding user...")
        um_add(u)
    
    # If key already assigned to user, remove it
    elif (h.in_user(u, hexstring)):
        print("Removing tag from user...")
        um_edit_rm(u, hexstring)

    # If key not assigned to user, add it
    elif (not h.in_user(u, hexstring)):
        print("Adding tag to user...")
        um_edit_add(u, hexstring)


# POST-PARSE HANDLING
if (__DEBUG__ == True):
    h.print_table()

h.c.close()

print("Done")
