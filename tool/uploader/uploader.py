# NB, I pretty much just followed google and pydrive api tutorials.
#
# Uploads a file at ../logs/log.csv to the google drive of the user in creds.txt
#   to set up creds, need to input data in web browser.

filename = "log.csv"
filepath = "../../logs/" + filename

from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive

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

file = drive.CreateFile({'title':filename})
file.SetContentFile(filepath)
file.Upload()
