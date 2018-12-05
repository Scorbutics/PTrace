@echo off
echo Chemin vers l'executable du processus
set /p a=
InjectItx64.exe -iCS Ptrace.dll %a%
pause