#!/bin/bash

# Need to run python script from its dir
cd tool/remote

python3 reporter.py

# delete created file "report.csv"
rm -f report.csv

# return home explicitly
cd ../..

