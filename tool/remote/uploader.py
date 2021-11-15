#####################################################################
#
# uploader.py
#
#  purpose: uploads a file at logs/log.csv to the google drive
#  relies on .json in this directory and creds.txt in tool/google/.
#  will need to set up in browser on first run to get the creds.
#
#####################################################################

from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive
from datetime import datetime

from gdrive_ids import LOG_FOLDER_ID

filename = "log"
filepath = "../../logs/" + filename + ".csv"
time_str = datetime.now().strftime("%Y-%m-%d--%H-%M-%S")
upload_file = time_str + ".csv"

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
file = drive.CreateFile({'title':upload_file,'parents':[{'id': f'{LOG_FOLDER_ID}'}]})
file.SetContentFile(filepath)
file.Upload()
