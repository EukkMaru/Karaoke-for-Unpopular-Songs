@echo off
g++ -g ./src/client.cpp ./src/KaraokeTrack.cpp -o karaoke_client.exe -I./include -L./lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lwinmm -mwindows
if %errorlevel% neq 0 pause