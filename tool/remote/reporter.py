#####################################################
#
# reporter.py
#
#  purpose: parse log file into report file, upload.
#  uses pydrive wrapper for Google Drive API
#
#####################################################


from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive
import csv
from datetime import date

from gdrive_ids import REPORT_FOLDER_ID

############################################
def get_up_file(drive):
    # Check drive if filename already exists. If it does, extract its id
    upload_id = None
    file_list = drive.ListFile({'q':f"'{REPORT_FOLDER_ID}' in parents and trashed=False"}).GetList()
    # get all copies of day's report
    today_list = [file_itr for file_itr in file_list if file_itr['title'] == upload_file]

    # if dupes, delete all but one copy; extract the survivor's
    if (len(today_list) > 1):
        for i in range(len(today_list)-1):
            today_list[i].Delete()
        upload_id = today_list[len(today_list)-1]['id']

    # if one copy, get its ID
    if (len(today_list) == 1):
        upload_id = today_list[0]['id']

    # if file already in drive, reupload with new data. otherwise upload file
    if (upload_id is not None):
        return drive.CreateFile({'id':upload_id, 'parents':[{'id':f'{REPORT_FOLDER_ID}'}]})
    else:
        return drive.CreateFile({'title':upload_file, 'parents':[{'id':f'{REPORT_FOLDER_ID}'}]})

#######################################

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

# get the cached file line number
line_offset = 0
with open("cache", "r") as f:
    line_offset = int(f.read())
if (line_offset > 3):
    line_offset = line_offset - 3

# cache update flag
cache_updated = False

with open("../../logs/log.csv", "r", newline='') as f:
    # skip cached lines according to cached offset
    cache_line_no = 0
    for _ in range(line_offset):
        cache_line_no += 1
        next(f)

    fh = csv.reader(f, delimiter=",")
    for row in fh:
        # continue updating line no for caching until hit
        if (not cache_updated):
            cache_line_no += 1
            
        if (row[0] == datestr):
            if (not cache_updated):
                with open("cache", "w") as cache_f:
                    if (cache_line_no > 1):
                        cache_line_no -= 1
                    cache_f.write(str(cache_line_no))
                cache_updated = True
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

up_file = get_up_file(drive)


up_file.SetContentFile("report.csv")
try:
    up_file.Upload()
except:
    up_file = get_up_file(drive)
    up_file.Upload()
