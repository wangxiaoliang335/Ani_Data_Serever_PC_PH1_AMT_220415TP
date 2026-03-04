@echo off

cd /d %~dp0

for %%i in (*.bundle) do (
echo %%i 
git pull %%i master
)
pause