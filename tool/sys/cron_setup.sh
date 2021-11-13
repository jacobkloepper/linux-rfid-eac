crontab -u $USER -l > tmp_crontab
echo "0 0 * * * cd /home/pi/source/repos/linux-rfid-eac/ && make upload" >> tmp_crontab
crontab -u $USER tmp_crontab
rm tmp_crontab
