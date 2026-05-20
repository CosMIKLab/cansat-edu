@echo off
REM CanSat beginner CLI for Windows — wraps PlatformIO commands.
REM Usage: cansat <run|build|monitor|new|help>

where pio >nul 2>&1
if %errorlevel% neq 0 (
    echo PlatformIO not found.
    echo Install it at: https://platformio.org/install/cli
    exit /b 1
)

set CMD=%1
if "%CMD%"=="" set CMD=help

if "%CMD%"=="run" (
    echo =^> Building and flashing your mission...
    pio run --target upload
    if %errorlevel%==0 (
        echo =^> Opening serial monitor ^(Ctrl+C to quit^)...
        pio device monitor
    )
    goto :eof
)

if "%CMD%"=="build" (
    echo =^> Building ^(checking for errors^)...
    pio run
    goto :eof
)

if "%CMD%"=="monitor" (
    echo =^> Opening serial monitor ^(Ctrl+C to quit^)...
    pio device monitor
    goto :eof
)

if "%CMD%"=="new" (
    copy mission_template.cpp src\mission.cpp >nul
    echo =^> mission.cpp has been reset to the starter template.
    goto :eof
)

echo.
echo   CanSat beginner framework
echo.
echo   Commands:
echo     cansat run      Build, flash, then open serial monitor
echo     cansat build    Build only ^(check for compile errors^)
echo     cansat monitor  Open serial monitor
echo     cansat new      Reset mission.cpp to the starter template
echo     cansat help     Show this message
echo.
echo   Edit src\mission.cpp to write your mission code.
echo.
