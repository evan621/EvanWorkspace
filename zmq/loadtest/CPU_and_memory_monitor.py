#!/usr/bin/python
import os
import sys
import logging
import imp
import socket
import time


# write "data" to file-filename  
def writeFile(filename, data):    
    f = open(filename, "w")  
    f.write(data)  
    f.write("\n")
    print "write pid file"
    f.close() 
    
    
def Find_PID(kyword,input_file):
    print "I come in find pid!" 
    pid = ''   
    f = open(input_file,'r')     
    fpid = f.readlines()   
    for line in fpid:        
        line = str(line)       
        if kyword in line:            
            pid = line[9:14]          
            print "process id: ",pid
            return pid  
    return pid   
    
def execCmd(cmd):
    r = os.popen(cmd)
    text = r.read()
    r.close()
    return text   
    
def exe_command(command,is_Pid):
    received = ''
            
    # if is_Pid = 1, get PID           
    if(is_Pid == 1):                
        received = execCmd(command) 
        # print "command out results:", received 
    else:            
           execCmd(command)
           print "system is creating the cpu and memory file, waiting...."
           time.sleep(37); 
           sys.exit()                 
    return received
    

pid_file = "pid_file.txt"  
cpuload_file = "cpuload_file.txt"   

 
# main function
if __name__ == '__main__':
	
    # Get PID
    
    Pid_index = 1
    # kyword = 'hwserver'
    kyword = sys.argv[1]
     
    pid_cmd = 'ps -Af' 
       
    PID_Num = ''
    loop = 0
    while PID_Num == '' and loop != 5:
        loop += 1
        all_pro = exe_command(pid_cmd,Pid_index) 
        writeFile(pid_file,all_pro) 
        PID_Num = Find_PID(kyword,pid_file) 
        
        print "PID is: ",PID_Num
        # delete pid file for the next time to use
        os.remove(pid_file)         
    if PID_Num == '':
        sys.exit(1)  
    
    # Get Cpuload
    cpuload_index = 2     
    cpuload_cmd = '/usr/bin/top -bcp ' + PID_Num + ' -d 1.2 > cpuload_file.txt &'
    print"cpuload_cmd is: ",cpuload_cmd     
    cpuload_value = exe_command(cpuload_cmd,cpuload_index) 
    #writeFile(cpuload_file,cpuload_value) 