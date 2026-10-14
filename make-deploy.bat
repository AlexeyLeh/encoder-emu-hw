@echo on
@call :GET_CURRENT_DATE

:: -----------------------------------------------
@setlocal enabledelayedexpansion

@SET PRJNAME=encoder-emu-hw
@SET EXEDIR=Debug\Exe
@SET EXEFILE=Debug\Exe\encoder-emu-hw.d90
@SET DEPLOY_DIR=..\%PRJNAME%-deploy

@IF NOT EXIST "%PRJNAME%.ewd" (
    @echo.
    @echo ERROR: not found %PRJNAME%.ewd - directory not for any operations
    @echo.
    @pause
    @exit /b 1
)

@IF NOT EXIST "%EXEFILE%" (
    @echo.
    @echo ERROR: not found %EXEFILE% - nothing to deploy
    @echo.
    @pause
    @exit /b 1
)

:: -----------------------------------------------
@set VERSION=0.0.0
@for /f "tokens=3" %%A in ('findstr /C:"#define VER_NUM " "ver.h"') do @(
    @set "VERSION=%%~A"
)
@echo VER:  %VERSION%

:: -----------------------------------------------
@for /f "tokens=1-3 delims=/. " %%a in ('date /t') do @(@set dat=%%c%%b%%a)
@for /f "tokens=1-2 delims=:" %%a in ('time /t') do @(@set tim=%%a%%b)
:: @SET DT=%dat%-%tim%
@SET DT=%dat%-%tim%

:: -----------------------------------------------
@SET TARGETNAME=%PRJNAME%-%DT%

@SET TARGET_BIN_DIR=..\%PRJNAME%-bin-%VERSION%-%DT%
@SET TARGET_SRC_DIR=..\%PRJNAME%-src-%VERSION%-%DT%

@rd /s /q %TARGET_BIN_DIR% 2>NUL 
@rd /s /q %TARGET_SRC_DIR% 2>NUL
                      
:: -----------------------------------------------
xcopy /E /Y /H /Q %EXEDIR%\* %TARGET_BIN_DIR%\%EXEDIR%\*
@if "%ERRORLEVEL%" NEQ "0" (
    @echo.
    @echo ERROR: copy error - %EXEDIR% to %TARGET_BIN_DIR%
    @echo.
    @pause
    @exit /b 1
)
@erase /F /Q %TARGET_BIN_DIR%\*.zip 2>NUL

xcopy /S /Y /H /Q * %TARGET_SRC_DIR%\*
@if "%ERRORLEVEL%" NEQ "0" (
    @echo.
    @echo ERROR: copy error - sources to %TARGET_SRC_DIR%
    @echo.
    @pause
    @exit /b 1
)
:: @rd /s /q %TARGET_SRC_DIR%\Debug\Obj 2>NUL
:: @rd /s /q %TARGET_SRC_DIR%\Debug\List 2>NUL

:: -----------------------------------------------
@del /f /q %TARGET_BIN_DIR%.zip 2>NUL
@del /f /q %TARGET_SRC_DIR%.zip 2>NUL

7z a -tzip %TARGET_BIN_DIR%.zip %TARGET_BIN_DIR%
@if "%ERRORLEVEL%" NEQ "0" (
    @echo error for %TARGET_BIN_DIR%.zip
    @pause
    @exit /b 1
)
@rd /s /q %TARGET_BIN_DIR% 2>NUL 

7z a -tzip %TARGET_SRC_DIR%.zip %TARGET_SRC_DIR%
@if "%ERRORLEVEL%" NEQ "0" (
    @echo error for %TARGET_SRC_DIR%.zip
    @pause
    @exit /b 1
)
@rd /s /q %TARGET_SRC_DIR% 2>NUL

:: -----------------------------------------------
@mkdir %DEPLOY_DIR% 2>NUL
@move /Y %TARGET_BIN_DIR%.zip %DEPLOY_DIR%\
@move /Y %TARGET_SRC_DIR%.zip %DEPLOY_DIR%\

:: -----------------------------------------------
@echo.
@echo The package was built successfully
@echo.


:: -----------------------------------------------
:GET_CURRENT_DATE
@set dt=%date: =0%
@set dt=%dt:~6,4%%dt:~3,2%%dt:~0,2%
@set tm=%time: =0%
@set tm=%tm:~0,2%%tm:~3,2%%tm:~6,2%
@set DT=%dt%-%tm%
@echo Date/Time stamp: %DT%
exit /b 0
