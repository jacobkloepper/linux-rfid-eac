# NB, I pretty much just followed google and pydrive api tutorials.
#
# Uploads a file at ../../logs/log.csv to the google drive of the user in creds.txt
#   to set up creds on first time, need to input data in web browser.
#   TODO: I don't know when or why or even if the creds expire.

from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive

filename = "users"
filepath = "../../data/" + filename + ".csv"

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


# download file
drive = GoogleDrive(gauth)

# test
file_list = drive.ListFile({'q': "'140syaUz8WLdJvehzuR38F8L2TSigQZBU' in parents and trashed=false"}).GetList()

for file in file_list:
    file.GetContentFile(file['title'])
