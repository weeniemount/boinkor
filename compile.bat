@echo off
echo compiling boinkor
cd src
cd resource
del icon.o
windres icon.rc -o icon.o
cd ..
cd ..
gcc src/main.c src/resource/icon.o -lgdi32 -luser32 -lcomdlg32 -mwindows -o boinkor.exe && boinkor.exe