@echo off

echo Resetting submodules...
echo.

git submodule deinit --all -f
git submodule init
git submodule update --remote

cd Engine/vendor/imgui
git checkout docking
cd ../../..

echo.
echo Reset Complete.
echo.

call vendor\bin\premake\premake5.exe vs2017
pause