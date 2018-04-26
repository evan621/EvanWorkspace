#!/bin/bash

function GetPID #User #Name
{
	PsUser=$1
	PsName=$2
	
	pid=`ps -u $PsUser | grep $PsName | grep -v grep | grep -v dbx |grep -v tail|grep -v start|grep -v stop | sed -n 1p | awk '{print $1}'`
	
	echo $pid
}

function GetCPU 
{
	CpuValue=`ps -p $1 -o pcpu`
}


PID=`GetPID root sshd`

echo $PID