################################################################
#
# get-ids.py
#
#  purpose: get the three necessary folder IDs from 
#  the google drive associated with the account in creds.txt
#
#
################################################################

from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive

def get_file_dict() -> dict:
    file_list = drive.ListFile({'q': "'root' in parents and trashed=false"}).GetList()
    file_dict = {}

    # fill dict with all files found
    for file_ptr in file_list:
        file_dict[file_ptr['title']] = file_ptr['id']

    return file_dict

# check/update creds
gauth = GoogleAuth()
gauth.LoadCredentialsFile("creds.txt")
if gauth.credentials is None:
    gauth.LocalWebserverAuth()
elif gauth.access_token_expired:
    gauth.Refresh()
else:
    gauth.Authorize()
gauth.SaveCredentialsFile("creds.txt")

# open handler
drive = GoogleDrive(gauth)


# get ids
file_dict = get_file_dict()

# check found files against expected files; if expected but not found, create and upload.
expected_names = ["reports", "logs", "data"]
for name in expected_names:
    if (name not in file_dict):
        drive.CreateFile({'title': name, 'mimeType': 'application/vnd.google-apps.folder'}).Upload()

# now that all expecteds are uploaded, get the file dict again
file_dict = get_file_dict()

# have all necessary IDs, create script to hold them
with open("../remote/gdrive_ids.py", "w") as fh:
    fh.write(f"REPORT_FOLDER_ID = \"{file_dict['reports']}\"\n")
    fh.write(f"LOG_FOLDER_ID = \"{file_dict['logs']}\"\n")
    fh.write(f"USER_FOLDER_ID = \"{file_dict['data']}\"\n")
