#!/bin/sh

tput clear
read -p "Input file name:" name

if [ -f $name ]
then
  echo "Current status of [$name] is:"
  ls -l $name
else
  echo "Can't find the file [$name]!"
  exit
fi
count=0
test_count=0
oldstatus=`ls -l $name | cut -c 36-43`
while [ $count -ne 2 ]; do
    sleep 2
    newstatus=`ls -l $name | cut -c 36-43`
    if [ "$oldstatus" = "$newstatus" ]
    then
      test_count=`expr $test_count + 1`
      echo
      echo "test file's status ..."
      echo
      if [ "$test_count" -eq 10 ]
      then
          echo "test number exceed ten!"
          exit
      else
          continue
      fi
    else
      echo
      echo "file [$name] size changed!"
      oldstatus="$newstatus"
      change_count=`expr $change_count + 1`
      test_count=0
    fi
done
echo "Change number exceed two, test end!"