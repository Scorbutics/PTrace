@echo off
echo Pid du processus en cours
set /p a=
InjectItx64.exe -i PTrace.dll -pid %a%
pause