# linux-rfid-eac setup
#   sets up rfid reader terminals to send read raw

PORTS=`ls /dev/ttyACM*`

for port in $PORTS
do
    stty -F $port 115200 raw
done
