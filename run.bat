@echo off
g++ -g ./src/main.cpp -o client.exe -I./include -L./lib -lmingw32 -lSDL2main -lSDL2 -mwindows
if %errorlevel% neq 0 pause