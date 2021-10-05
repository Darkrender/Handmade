@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"

mkdir build
pushd build
cl -Zi ../code/win32_handmade.cpp user32.lib Gdi32.lib
popd