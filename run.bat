@echo off
g++ -fexec-charset=UTF-8 -g ./src/client.cpp ./src/KaraokeTrack.cpp -o karaoke_client.exe -I./include -I./include/SDL2 -L./lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer -lwinmm
if %errorlevel% neq 0 pause