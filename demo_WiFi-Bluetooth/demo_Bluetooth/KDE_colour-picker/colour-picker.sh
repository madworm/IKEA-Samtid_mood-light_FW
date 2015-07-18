#!/bin/bash

# adjust as needed
BT_INTERFACE="";

if [[ -z $BT_INTERFACE ]]; then
	echo -e "\nBluetooth interface not set!";
	echo -e "Might be /dev/rfcomm0\n";
	exit 0;
fi;

KDIALOG_BIN=`which kdialog`;

if [[ -z $KDIALOG_BIN ]]; then
	echo -e "\nkdialog not found!\n";
	exit 0;
fi;

while(true); do
	RGB=`$KDIALOG_BIN --getcolor`;
	RED=$(echo $RGB | cut --bytes=2,3);
	RED=$((16#$RED));
	GREEN=$(echo $RGB | cut --bytes=4,5);
	GREEN=$((16#$GREEN));
	BLUE=$(echo $RGB | cut --bytes=6,7);
	BLUE=$((16#$BLUE));
	echo "SET:mode=1&red=$RED&green=$GREEN&blue=$BLUE&";
	echo "SET:mode=1&red=$RED&green=$GREEN&blue=$BLUE&" > $BT_INTERFACE;
done;

