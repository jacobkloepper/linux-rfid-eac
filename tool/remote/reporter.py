from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive
import csv
from datetime import date

## NOTE: 
##   Only considers one in/out pair.
##   As it stands, will report the last "in" and last "out" in a day

# Create report file
today = date.today()
datestr = today.strftime("%d/%m/%y")

report_dict = dict() 


with open("../../logs/log.csv", "r", newline='') as f:
    fh = csv.reader(f, delimiter=",")
    for row in fh:
        if (row[0] == datestr):
            # row:
            #   row[0]: dd/mm/yy 
            #   row[1]: hh/mm/ss
            #   row[2]: Firstname Lastname
            #   row[3]: in OR out
            if (row[2] not in report_dict):
                report_dict[row[2]] = [None]*2

            if (row[3] == "in"):
                report_dict[row[2]][0] = row[1]
            elif (row[3] == "out"):
                report_dict[row[2]][1] = row[1]

with open("report.csv", "w", newline='') as f:
    fh = csv.writer(f, delimiter=",")
    fh.writerow(["Name", "Entering", "Exiting"])
    for item in report_dict:
        fh.writerow([item, report_dict[item][0], report_dict[item][1]])
 

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

drive = GoogleDrive(gauth)

# Upload to remote
upload_file = datestr
file = drive.CreateFile({'title':upload_file,'parents':[{'id':'18mDYLQSFrF0SlVmw7o54BjxSQuFVy9Ya'}]})
file.SetContentFile("report.csv")
file.Upload()
