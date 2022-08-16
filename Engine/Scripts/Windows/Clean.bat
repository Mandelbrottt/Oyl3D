@echo off

call %~dp0\Common.bat

pushd %~dp0\"%REFLY_ENGINE_DIR%"

"%PREMAKE_EXE%" clean
pause

popd