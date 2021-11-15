# These variables contain the string id of the folders needed by the scripts to interface with the google drive.
# To find these IDs:
#   0. Make your dummy google account, set up the auth and creds and all.
#   1. In that account's google drive, create three folders: one for reports, one for logs, and one for user data.
#   2. Enter each folder, your web browser URL will look something like [drive.google.com/drive/u/{X}/folders/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#   3. The symbols in the final section of the path is that folder's id. Replace the three strings below with the appropriate strings.

# TODO: do this automatically? Can probably even make the directories from pydrive.

REPORT_FOLDER_ID = "18mDYLQSFrF0SlVmw7o54BjxSQuFVy9Ya"
LOG_FOLDER_ID = "1IyhxkZgba7VCs7q6yzp1-aQXpjCgVM9F"
USER_FOLDER_ID = "140syaUz8WLdJvehzuR38F8L2TSigQZBU"
