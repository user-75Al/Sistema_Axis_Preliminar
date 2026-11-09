@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

echo ===================================================================
echo            SISTEMA AXIS - ARQUITECTURA EN CAPAS (C++)
echo ===================================================================
echo  Compilando codigo fuente C++ (C++14)...
echo -------------------------------------------------------------------

rem -------------------------------------------------------------------
rem  Deteccion de OpenCV (opcional para biometria facial)
rem -------------------------------------------------------------------
set "CV_ROOT="

if defined OPENCV_DIR if exist "%OPENCV_DIR%\include" set "CV_ROOT=%OPENCV_DIR%"
if not defined CV_ROOT if exist "C:\opencv\build\include" set "CV_ROOT=C:\opencv\build"
if not defined CV_ROOT if exist "C:\msys64\mingw64\include\opencv4" set "CV_ROOT=C:\msys64\mingw64"
if not defined CV_ROOT if exist "C:\opencv\build" set "CV_ROOT=C:\opencv\build"

rem Solo activar OpenCV si hay bibliotecas compatibles con MinGW.
if defined CV_ROOT if not exist "!CV_ROOT!\x64\mingw\lib" if not exist "!CV_ROOT!\lib" (
    echo  OpenCV encontrado, pero sin bibliotecas MinGW. Se omite la biometria.
    set "CV_ROOT="
)

set "CV_FLAGS="

if defined CV_ROOT (
    if exist "!CV_ROOT!\include\opencv4" (
        set "CV_FLAGS=-DAXIS_HAVE_OPENCV -I"!CV_ROOT!\include\opencv4""
    ) else (
        set "CV_FLAGS=-DAXIS_HAVE_OPENCV -I"!CV_ROOT!\include""
    )

    if exist "!CV_ROOT!\x64\mingw\lib" set "CV_FLAGS=!CV_FLAGS! -L"!CV_ROOT!\x64\mingw\lib""
    if exist "!CV_ROOT!\lib" set "CV_FLAGS=!CV_FLAGS! -L"!CV_ROOT!\lib""

    set "CV_FLAGS=!CV_FLAGS! -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_objdetect -lopencv_face"
    echo  OpenCV detectado en: !CV_ROOT!
    echo  Biometria facial: ACTIVADA
) else (
    echo  OpenCV no detectado. Se compilara sin biometria facial.
    echo  Para activarla, instale OpenCV y defina OPENCV_DIR.
)

echo -------------------------------------------------------------------

set "FILES="
for /r %%F in (*.cpp) do set "FILES=!FILES! "%%F""

g++ -std=c++14 !FILES! -I . !CV_FLAGS! -Wall -Wextra -o sistema_axis.exe

if %errorlevel% equ 0 (
    echo [OK] Compilacion exitosa.
    echo -------------------------------------------------------------------
    echo  Iniciando sistema_axis.exe...
    echo ===================================================================
    sistema_axis.exe
) else (
    echo.
    echo ===================================================================
    echo  [ERROR] Ocurrio un error durante la compilacion.
    echo ===================================================================
    pause
)

endlocal
