@echo off
echo compiling boinkor
cd src
cd resource
del resource.o
windres resource.rc -o resource.o
cd ..
cd ..
gcc src/main.c src/resource/resource.o -lgdi32 -luser32 -lcomdlg32 -lcomctl32 -mwindows -o boinkor.exe && boinkor.exe