@echo off

REM 날짜에서 - 를 지움 
echo.Date   : %date%
echo.Time   : %time%

set date1=%date:-=%
set date2=%date:/=%
set date3=%date2:~0,4%

set time2=%time: =0%
set time3=%time2::=%
set time4=%time2:~0,2%%time2:~3,2%

set Filename=repo_Data_Server_%date3%_%time4%.bundle
echo.FileName   : %filename% 

del *.bundle
git bundle create "%filename%" master

pause