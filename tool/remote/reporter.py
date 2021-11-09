from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive
import csv
from datetime import date

## NOTE: 
##   Only considers one in/out pair.
##   As it stands, will report the first "in" and last "out" in a day

# Create report file
today = date.today()
datestr = today.strftime("%d/%m/%y")

# fills the lines of the output .csv. key = name, val = [in_time, out_time]
report_dict = dict()
# checks off a user's first "in" entry. key = name, val = True/False
check_dict = dict()

with open("../../logs/log.csv", "r", newline='') as f:
    fh = csv.reader(f, delimiter=",")
    for row in fh:
        if (row[0] == datestr):
            # row:
            #   row[0]: dd/mm/yy 
            #   row[1]: hh/mm/ss
            #   row[2]: Firstname Lastname
            #   row[3]: in || out
            if (row[2] not in report_dict):
                report_dict[row[2]] = [None]*2
                check_dict[row[2]] = False

            if (row[3] == "in"):
                if (check_dict[row[2]] == False):
                    check_dict[row[2]] = True
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

# Check drive if filename already exists. If it does, extract its id
upload_id = None
file_list = drive.ListFile({'q':"'18mDYLQSFrF0SlVmw7o54BjxSQuFVy9Ya' in parents and trashed=False"}).GetList()
for file_itr in file_list:
    if file_itr['title'] == upload_file:
        upload_id = file_itr['id']

# if file already in drive, reupload with new data. otherwise upload file
if (upload_id is not None):
    file = drive.CreateFile({'id':upload_id, 'parents':[{'id':'18mDYLQSFrF0SlVmw7o54BjxSQuFVy9Ya'}]})
else:
    file = drive.CreateFile({'title':upload_file, 'parents':[{'id':'18mDYLQSFrF0SlVmw7o54BjxSQuFVy9Ya'}]})

file.SetContentFile("report.csv")
file.Upload()
