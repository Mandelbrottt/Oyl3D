@echo off

pushd %~dp0

set REARM_ENGINE_DIR=..\..\

@REM Relative to Engine directory
set BINARIES_DIR=Binaries\
set THIRD_PARTY_DIR=%BINARIES_DIR%ThirdParty\
set PREMAKE_EXE=%THIRD_PARTY_DIR%premake5.exe

popd