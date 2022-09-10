@echo off

call %~dp0\Common.bat

pushd %~dp0\"%REARM_ENGINE_DIR%"

"%PREMAKE_EXE%" vs2019

popd