@echo off

set doUpdate=F

if not exist "Engine\vendor\glfw\premake5.lua" set doUpdate=T
if not exist "Engine\vendor\imgui\premake5.lua" set doUpdate=T

REM Update if any submodules are not initialised
dir /b /a "Engine\vendor\glm\*" | >nul findstr "^" || set doUpdate=T
dir /b /a "Engine\vendor\spdlog\*" | >nul findstr "^" || set doUpdate=T
dir /b /a "Engine\vendor\glfw\*" | >nul findstr "^" || set doUpdate=T
dir /b /a "Engine\vendor\imgui\*" | >nul findstr "^" || set doUpdate=T

if "%doUpdate%"=="T" (
    echo One or more git submodules was not configured correctly. Updating...
    echo.
    git submodule deinit --all -f
    git submodule init
    git submodule update --remote
    echo.
    echo Update Complete.
    echo.
) else (
    echo All git submodules are up to date.
    echo.
)

call vendor\bin\premake\premake5.exe vs2017
pause