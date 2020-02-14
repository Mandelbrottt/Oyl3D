@echo off

pushd %~dp0\..\..\

echo Checking if submodules need updates...
echo.

git submodule update --init --recursive

echo.
echo All submodules are up to date.
echo. 

popd