@echo off
setlocal
cd /d "%~dp0"
if "%TOOLCHAIN%"=="" set TOOLCHAIN=C:\msys64\mingw32\bin
set PATH=%TOOLCHAIN%;%PATH%
echo Using toolchain at "%TOOLCHAIN%"
echo Pass --skip-existing to skip compile commands when matching .o already exists.

echo Building intermediate: "C:\Builder Projects\Base64\MainWindowResource.o"
if /I "%~1"=="--skip-existing" if exist "C:\Builder Projects\Base64\MainWindowResource.o" goto :skip_1
"%TOOLCHAIN%\windres.exe" --target=pe-i386 "MainWindowResource.rc" "C:\Builder Projects\Base64\MainWindowResource.o"
if errorlevel 1 goto :fail
:skip_1
echo Building intermediate: "C:\Builder Projects\Base64\WinMainUnit.o"
if /I "%~1"=="--skip-existing" if exist "C:\Builder Projects\Base64\WinMainUnit.o" goto :skip_2
"%TOOLCHAIN%\g++.exe" -c -m32 -fpermissive -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-sign-compare -Wno-deprecated-declarations -Wattributes -Wno-unknown-pragmas -Wno-missing-field-initializers -D_UNICODE -DUNICODE -DNOVTABLE= -D_WIN32_WINNT=0x0501 -DNDEBUG "WinMainUnit.cpp" -o "C:\Builder Projects\Base64\WinMainUnit.o"
if errorlevel 1 goto :fail
:skip_2
echo Building intermediate: "C:\Builder Projects\Base64\MainWindowUnit.o"
if /I "%~1"=="--skip-existing" if exist "C:\Builder Projects\Base64\MainWindowUnit.o" goto :skip_3
"%TOOLCHAIN%\g++.exe" -c -m32 -fpermissive -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-sign-compare -Wno-deprecated-declarations -Wattributes -Wno-unknown-pragmas -Wno-missing-field-initializers -D_UNICODE -DUNICODE -DNOVTABLE= -D_WIN32_WINNT=0x0501 -DNDEBUG "MainWindowUnit.cpp" -o "C:\Builder Projects\Base64\MainWindowUnit.o"
if errorlevel 1 goto :fail
:skip_3
echo Building intermediate: "C:\Builder Projects\Base64\EncodeDecodeThreadUnit.o"
if /I "%~1"=="--skip-existing" if exist "C:\Builder Projects\Base64\EncodeDecodeThreadUnit.o" goto :skip_4
"%TOOLCHAIN%\g++.exe" -c -m32 -fpermissive -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-sign-compare -Wno-deprecated-declarations -Wattributes -Wno-unknown-pragmas -Wno-missing-field-initializers -D_UNICODE -DUNICODE -DNOVTABLE= -D_WIN32_WINNT=0x0501 -DNDEBUG "EncodeDecodeThreadUnit.cpp" -o "C:\Builder Projects\Base64\EncodeDecodeThreadUnit.o"
if errorlevel 1 goto :fail
:skip_4
echo Build finished successfully.
goto :eof
:fail
echo Build failed with error %errorlevel%.
pause
exit /b %errorlevel%
