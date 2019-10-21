#Script para chamar outro script em caso de modificação

from os import path, system
import platform
from time import sleep
import requests

def windows_loop():
        file = 'programa.py'
        url = 'http://192.168.25.9/set?{}=on'

        create_time = path.getctime(file)
        
        while(True):
                system('ECHO|SET /p="."')
                mod_time = path.getmtime(file)
                if mod_time != create_time:
                        system('cls')                
                        exit_status = system(file)
                        if exit_status == 0:
                                requests.get(url.format('green'))
                        else:
                                requests.get(url.format('red'))
                        create_time = mod_time                

                sleep(1)

def main():
        if platform.system() == 'Windows':
                windows_loop()
        elif platform.system() == 'Linux':
                system('sh check_tests.sh')
        else: 
                print('unsupported platform')

if __name__ == "__main__":
        main()