# send SIGINT to process
# convenient if ssh into host (so you cannot CTRL+C)

PROGNAME="linux-rfid-eac"
PID=`pidof $PROGNAME`

if [ -z $PID ]
then
    printf "Cannot find PID of $PROGNAME\n"
else
    kill -s INT $PID
fi
