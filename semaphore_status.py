import sys
import requests

'''
this program takes the last status of the terminal 
and creates an http request based on its exit state
    Usage: 
	Linux: ./program_to_get_exit_status.py 
	       ./semaphore_status.py $?
	Windows: python program_to_get_exit_status.py
			 python semaphore_status.py %errorlevel%
'''

IP_ADDRESS = 'ESP_IP_ADDRESS' #change this variable

def check_status( exit_code ):
        return 'green=on' if exit_code == 0 else 'red=on'		
		
if __name__ == "__main__":
	
	exit_code = int( sys.argv[1] )
	url = 'http://{}/set?{}'.format( IP_ADDRESS, check_status( exit_code ) )
	requests.get( url )