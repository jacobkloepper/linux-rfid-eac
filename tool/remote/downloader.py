######################################################################################
#
# downloader.py
#
#  purpose: download a new user file
#  called from the main loop if it detects a dummy file data/new.
#
######################################################################################

from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive

from gdrive_ids import USER_FOLDER_ID

filename = "users"
filepath = "../../data/" + filename + ".csv"


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


# download file
drive = GoogleDrive(gauth)

# test
file_list = drive.ListFile({'q': f"'{USER_FOLDER_ID}' in parents and trashed=false"}).GetList()

for file in file_list:
    file.GetContentFile(file['title'])
