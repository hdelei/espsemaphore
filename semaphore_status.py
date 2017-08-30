import sys
import requests

'''
This program get the last exit status code from terminal
and make a url request based on it's state
        Usage: 
	Linux: ./program_to_get_exit_status.py 
	       ./this_program.py $?
	Windows: python program_to_get_exit_status.py
		 python this_program.py %errorlevel%
'''

def check_status(exit_code):
        return 'green' if exit_code == 0 else 'red'		
		
if __name__ == "__main__":
	
	exit_code = int(sys.argv[1])
	url = 'http://192.168.25.37/gpio/' + check_status(exit_code))
	requests.get(url)
