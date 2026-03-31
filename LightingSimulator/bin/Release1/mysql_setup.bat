@echo off
"D:\Program Files\MySQL\MySQL Server 8.4\bin\mysql.exe" -u root -e "ALTER USER 'root'@'localhost' IDENTIFIED BY '123456';"
"D:\Program Files\MySQL\MySQL Server 8.4\bin\mysql.exe" -u root -p123456 -e "CREATE DATABASE IF NOT EXISTS ivs_lcd CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;"
"D:\Program Files\MySQL\MySQL Server 8.4\bin\mysql.exe" -u root -p123456 -e "SHOW DATABASES;"
pause
