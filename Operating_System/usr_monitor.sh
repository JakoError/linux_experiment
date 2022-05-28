#!/bin/sh
#
# This program monitor user login
#
if [ $# -ne 1 ]; then
  echo "Usage: usr_monitor username"
  exit
fi
echo "You will monitor [$1]"
echo
ulist=$(who | cut -f 1 -d " ")
echo "Current user list is:$ulist"
echo
uname=$(who | grep "$1")
while uname=$(who | grep "$1")
  test -z "$uname"
do
  echo "waiting user [$1] ..."
  sleep 5
done
echo "user [$1] is logon"
