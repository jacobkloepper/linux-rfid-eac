# NB, I pretty much just followed google and pydrive api tutorials.
#
# Uploads a file at ../../logs/log.csv to the google drive of the user in creds.txt
#   to set up creds on first time, need to input data in web browser.
#   TODO: I don't know when or why or even if the creds expire.

from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive
from datetime import datetime

filename = "log"
filepath = "../../logs/" + filename + ".csv"
time_str = datetime.now().strftime("%Y-%m-%d--%H-%M-%S")
upload_file = time_str + ".csv"


# Get/read auth
gauth = GoogleAuth()
gauth.LoadCredentialsFile("creds.txt")
if gauth.credentials is None:
    gauth.LocalWebserverAuth()
elif gauth.access_token_expired:
    gauth.Refresh()
else:
    gauth.Authorize()
gauth.SaveCredentialsFile("creds.txt")


# upload file
drive = GoogleDrive(gauth)

# upload into folder in google drive.
# if creating a new drive or folder to upload to, need to get the new folder id as below.
file = drive.CreateFile({'title':upload_file,'parents':[{'id': '1IyhxkZgba7VCs7q6yzp1-aQXpjCgVM9F'}]})
file.SetContentFile(filepath)
file.Upload()
