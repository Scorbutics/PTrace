@echo off
echo Nom du processus en cours
set /p a=
InjectItx64.exe -i Ptrace.dll %a%
pause