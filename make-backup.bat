@echo on
@call :GET_WIN_VERSION
@call :GET_CURRENT_DATE

@SET PRJNAME=encoder-emu-hw

:: -----------------------------------------------
@SET VERFILE=ver.h
@set VERSION=0.0.0
@for /f "tokens=3" %%A in ('findstr /C:"#define VER_NUM " "%VERFILE%"') do @(
    @set "VERSION=%%~A"
)
@echo VER:  %VERSION%

:: -----------------------------------------------

@for /f "tokens=1-3 delims=/. " %%a in ('date /t') do (set dat=%%c%%b%%a)
@for /f "tokens=1-2 delims=:" %%a in ('time /t') do (set tim=%%a%%b)
@SET DT=%dat%-%tim%

:: -----------------------------------------------

@SET BACKUPDIR=..\%PRJNAME%-backups
@SET BACKUPNAME=%PRJNAME%-src-%VERSION%-%osVer%-%osArch%-%DT%

:: -----------------------------------------------

@SET tmpdir=%PRJNAME%-src-%VERSION%-%osVer%-%osArch%-%DT%
@rmdir /s /q %tmpdir% 2>NUL

@set target=..\%PRJNAME%-%VERSION%-%osVer%-%osArch%-%DT%
@mkdir %target% >NUL

xcopy /S /Y /V /H /Q * %target%\* >NUL
@if "%ERRORLEVEL%" NEQ "0" (
    @echo "ERRORLEVEL: %ERRORLEVEL%"
    @echo ERROR: can't copy to %target%
    @pause
    @goto :EOF
)

@mkdir %BACKUPDIR% 2>NUL
7z a -x!*.o %BACKUPDIR%\%BACKUPNAME%.7z %target% 
@if "%ERRORLEVEL%" NEQ "0" (
    @echo "ERRORLEVEL: %ERRORLEVEL%"
    @echo ERROR: Can't pack %tmpdir% to %BACKUPDIR%\%BACKUPNAME%.7z
)
@rmdir /s /q ..\%tmpdir%\

:: -----------------------------------------------
:: pause
@goto :EOF


:: -----------------------------------------------
:GET_CURRENT_DATE
@set dt=%date: =0%
@set dt=%dt:~6,4%%dt:~3,2%%dt:~0,2%
@set tm=%time: =0%
@set tm=%tm:~0,2%%tm:~3,2%%tm:~6,2%
@set DT=%dt%-%tm%
@echo Date/Time stamp: %DT%
@exit /b 0

:: -----------------------------------------------
:GET_WIN_VERSION
:: set TESTstring="Microsoft Windows [Version 39.0.99999]"
:: set TESTstring="Microsoft Windows [Version 10.0.22621]"
:: set TESTstring="Microsoft Windows [Version 10.0.22000]"
:: set TESTstring="Microsoft Windows [Version 10.0.10240]"
:: set TESTstring="Microsoft Windows [Version 6.3.9600]"
:: set TESTstring="Microsoft Windows [Version 6.1.7601]"
:: set TESTstring="Microsoft Windows XP [Version 5.1.2600]"
:: set TESTstring="Microsoft Windows [Version 4.10]"

@set osVer=unkn
@for /f "tokens=3-7 delims=[.] " %%i in ('ver') do @(if %%i==XP (set osVerFul=%%k.%%l) else (if %%j geq 10 (set osVerFul=%%j.%%k.%%l) else (set osVerFul=%%j.%%k)))
@set os_ver=%osVerFul%
@if %osVerFul:~0,1% gtr 3 set os_ver=0%osVerFul%

:: If we live to see W12, replace 10.0.44000 below with the correct build
:: and modify W11 line to add "lss 10.0.44000"
:: if %os_ver% GEQ 10.0.44000 set osVer=Windows 12

@if %os_ver% GEQ 10.0.22000 set osVer=win11
@if %os_ver% GEQ 10.0.10240 if %os_ver% lss 10.0.22000 set osVer=win10
@if %os_ver% equ 06.3 set osVer=win81
@if %os_ver% equ 06.2 set osVer=win8
@if %os_ver% equ 06.1 set osVer=win7
@if %os_ver% equ 06.0 set osVer=vista
@if %os_ver% equ 05.2 set osVer=xp64
@if %os_ver% equ 05.1 set osVer=xp
@if %os_ver% equ 05.0 set osVer=w2000
@if %os_ver% lss 05.0 set osVer=winme 
:: or 98 or less

@set osArch=%PROCESSOR_ARCHITECTURE:~-2%
@if %osArch% equ 86 set osArch=32

:: echo Detected %osVer%-%osArch% (%osVerFul%)
@if %os_ver% lss 06.3 echo Only Windows 8.1 or above are supported !

@echo Detected: %osVer%-%osArch% (%osVerFul%)
@exit /b 0
