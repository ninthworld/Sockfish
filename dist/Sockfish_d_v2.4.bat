@echo off
SET /P _threads= # of Threads [1-512]: 
SET /P _mem= Size of TT in MB [1-512]: 
SET /P _d= Show debug info [Y/N]? 
SET /P _n= Use null move [Y/N]? 
IF /I "%_d%" EQU "Y" goto :debug
IF /I "%_n%" EQU "Y" goto :null

Sockfish_v2.4.exe -t %_threads% -s %_mem%
pause
end

:debug
IF /I "%_n%" EQU "Y" goto :nulld
Sockfish_v2.4.exe -t %_threads% -s %_mem% -d
pause
end

:null
Sockfish_v2.4.exe -t %_threads% -s %_mem% -n
pause
end

:nulld
Sockfish_v2.4.exe -t %_threads% -s %_mem% -d -n
pause
end