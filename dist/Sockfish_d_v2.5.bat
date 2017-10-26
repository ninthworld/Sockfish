@echo off
SET /P _threads= # of Threads [1-512]: 
SET /P _mem= Size of TT in MB [1-512]: 
SET /P _d= Show debug info [Y/N]? 
IF /I "%_d%" EQU "Y" goto :debug

Sockfish_v2.5.exe -t %_threads% -s %_mem%
pause
end

:debug
Sockfish_v2.5.exe -t %_threads% -s %_mem% -d
pause
end