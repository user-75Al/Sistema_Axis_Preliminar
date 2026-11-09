@echo off
setlocal
cd /d "%~dp0"

rem Agrega las DLL de OpenCV/MSYS2 al PATH para ejecutar el binario.
if exist "C:\msys64\mingw64\bin" set "PATH=C:\msys64\mingw64\bin;%PATH%"
if defined OPENCV_DIR if exist "%OPENCV_DIR%\bin" set "PATH=%OPENCV_DIR%\bin;%PATH%"

if not exist "sistema_axis.exe" (
    echo No existe sistema_axis.exe.
    echo Ejecute primero compilar.bat.
    pause
    exit /b 1
)

sistema_axis.exe
endlocal
