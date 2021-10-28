#!/bin/bash

rm -f data/users.csv

# Need to run python script from its dir
cd tool/remote

python3 downloader.py

# move downloaded file
if test -e users.csv
then
    mv users.csv ../../data/
else
    echo "-ERROR: User data not downloaded."
fi

# return home explicitly
cd ../..
