# reset the log file
# run from project root ("sh tool/reset_log.sh")

rm -f logs/log.csv
touch logs/log.csv
printf "Date,Time,Name,Direction\n" >> logs/log.csv
