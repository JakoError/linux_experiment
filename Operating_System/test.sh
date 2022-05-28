#
# Monitor for file status
#
clear
echo "Input file name:"
read fname
if [ -f $fname ]
then
    echo "Current status of [$fname] is:"
    ls -l $fname
else
    echo "Can't find the file [$fname]!"
    exit
fi
change_count=0
test_count=0
oldstatus=`ls -l $fname | cut -c 36-43`

while [ $change_count -ne 2 ]
do
    sleep 2
    newstatus=`ls -l $fname | cut -c 36-43`
    if [ "$oldstatus" = "$newstatus" ]
    then
        test_count=`expr $test_count + 1`
        echo
        echo "test file's status ..."
        echo
        if [ $test_count -eq 10 ]
        then
            echo "test number exceed ten!"
            exit
        else
            continue
        fi
    else
        echo
        echo "file [$fname] size changed!"
        oldstatus="$newstatus"
        change_count=`expr $change_count + 1`
        test_count=0
    fi
done
echo "Change number exceed two, end!"

