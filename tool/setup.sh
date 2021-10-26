# linux-rfid-eac setup
#   sets up rfid reader terminals to read raw 
#   (ie, do not convert incoming bytes into ascii chars).

PORTS=`ls /dev/ttyACM*`

for port in $PORTS
do
    stty -F $port 115200 raw
done
