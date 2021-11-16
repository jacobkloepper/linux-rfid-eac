crontab -u $USER -l > tmp_crontab

final_line=`tail -n 1 tmp_crontab`
if [ "$final_line" == "0 0 * * * cd /home/pi/source/repos/linux-rfid-eac/ && make upload" ]
then
    printf "crontab already set up!\n"
else 
    echo "0 0 * * * cd /home/pi/source/repos/linux-rfid-eac/ && make upload" >> tmp_crontab
    crontab -u $USER tmp_crontab
fi
rm tmp_crontab
echo "Finished installing"
