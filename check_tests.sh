#!/bin/sh

DEF_TIME=`stat -c %Z programa.py`
IP="http://192.168.25.9"
GREEN="/set?green=on"
RED="/set?red=on"

while true
do
	MOD_TIME=`stat -c %Z programa.py`

	if [ "$MOD_TIME" != "$DEF_TIME" ]
	then	    
	    python programa.py
	    
		if [ "$?" = "0" ]
	    then            	
		    curl -s $IP$GREEN > /dev/null
            echo " ◙"
	    else		    
		    curl -s $IP$RED > /dev/null
	    fi
	    
	    DEF_TIME=$MOD_TIME
	fi
	sleep 1    
	echo -n "."
done