@echo off
SET /P _threads= # of Threads [1-512]: 
SET /P _mem= Size of TT in MB [1-512]: 
SET /P _d= Show debug info [Y/N]? 
SET /P _p= Enabled pondering [Y/N]? 
IF /I "%_d%" EQU "Y" goto :debug
IF /I "%_p%" EQU "Y" goto :ponder

Sockfish_v2.2.exe -t %_threads% -s %_mem%
pause
end

:ponder
Sockfish_v2.2.exe -t %_threads% -s %_mem% -p
pause
end

:debug
IF /I "%_p%" EQU "Y" goto :d_ponder
Sockfish_v2.2.exe -t %_threads% -s %_mem% -d
pause
end

:d_ponder
Sockfish_v2.2.exe -t %_threads% -s %_mem% -d -p
pause
end