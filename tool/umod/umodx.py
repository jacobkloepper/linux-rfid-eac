import serial
import sys
import subprocess
from time import sleep

DATA_FOLDER_ID = '140syaUz8WLdJvehzuR38F8L2TSigQZBU'


# First, remove any RFID tag near reader
print("Remove RFID tags from reader\n")

# Verify port setup
print("Checking port...", flush=True)
good_connection = False
while (good_connection == False):
    try:
        reader = serial.Serial(port="/dev/ttyACM0", baudrate=115200, stopbits=serial.STOPBITS_TWO)
        print("Connection good.")
        good_connection = True
    except:
        print("Could not read port. Verify connection and try again.")
        input("Press enter to retry.")

print()

# Get name
valid_name_rx = False
while (valid_name_rx == False):
    try:
        (firstname, lastname) = input("Input name [Firstname Lastname]:\n").split()
        valid_name_rx = True
    except ValueError:
        print("Format: Firstname Lastname")
        print("    eg. Kevin Bieksa")
        print("    eg. Alex Burrows")
        print()

print()

# Get ID 
print("Present RFID tag to reader", flush=True)
reader.reset_input_buffer()
byte_data = reader.read(size=7)

print()

# Resolve opened port
reader.reset_input_buffer()
reader.close()

# Print to data file
original_stdout = sys.stdout
with open("../../data/users.csv", "a") as f:
    # redirect stdout
    sys.stdout = f

    # print uid bytewise
    for byte in byte_data:
        print("%02x" % (byte), end="")

    # print comma then name
    print(",{} {}".format(firstname, lastname))

    # resolve redirected stdout
    sys.stdout = original_stdout
    
# Upload file to google drive

from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive

filename = "users"
filepath = "../../data/" + filename + ".csv"
upload_name = filename + ".csv"

# Get/read auth
gauth = GoogleAuth()
gauth.LoadCredentialsFile("../google/creds.txt")
if gauth.credentials is None:
    gauth.LocalWebserverAuth()
elif gauth.access_token_expired:
    gauth.Refresh()
else:
    gauth.Authorize()
gauth.SaveCredentialsFile("../google/creds.txt")


# upload file
drive = GoogleDrive(gauth)

# upload into folder in google drive.
# if creating a new drive or folder to upload to, need to get the new folder id as below.

# Get the id of the users.csv in drive if exists
update_id = None
file_list = drive.ListFile({'q':"'140syaUz8WLdJvehzuR38F8L2TSigQZBU' in parents and trashed=False"}).GetList()
for file_itr in file_list:
    if file_itr['title'] == upload_name:
        update_id = file_itr['id']

# Second, upload the new users.csv to the old id or create if wasn't previously in drive
if (update_id is not None):
    file = drive.CreateFile({'id':update_id, 'parents':[{'id': '140syaUz8WLdJvehzuR38F8L2TSigQZBU' }]})
else:
    file = drive.CreateFile({'title':upload_name, 'parents':[{'id': '140syaUz8WLdJvehzuR38F8L2TSigQZBU' }]})
file.SetContentFile(filepath)
file.Upload()


# Create data/new to indicate a file has been uploaded.
with open("../../data/new", "w") as f:
    pass
