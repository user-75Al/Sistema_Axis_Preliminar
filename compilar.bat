@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

echo ===================================================================
echo            SISTEMA AXIS - ARQUITECTURA EN CAPAS (C++)
echo ===================================================================
echo  Compilando codigo fuente C++...
echo -------------------------------------------------------------------

rem -------------------------------------------------------------------
rem  Deteccion de OpenCV (opcional para biometria facial)
rem -------------------------------------------------------------------
set "CV_ROOT="

if defined OPENCV_DIR if exist "%OPENCV_DIR%\include" set "CV_ROOT=%OPENCV_DIR%"
if not defined CV_ROOT if exist "C:\msys64\mingw64\include\opencv5" set "CV_ROOT=C:\msys64\mingw64"
if not defined CV_ROOT if exist "C:\msys64\mingw64\include\opencv4" set "CV_ROOT=C:\msys64\mingw64"
if not defined CV_ROOT if exist "C:\opencv\build\include" set "CV_ROOT=C:\opencv\build"
if not defined CV_ROOT if exist "C:\opencv\build" set "CV_ROOT=C:\opencv\build"

rem Solo activar OpenCV si hay bibliotecas compatibles con MinGW.
if defined CV_ROOT (
    set "CV_HAS_LIBS="
    if exist "!CV_ROOT!\x64\mingw\lib" set "CV_HAS_LIBS=1"
    if exist "!CV_ROOT!\lib" set "CV_HAS_LIBS=1"

    if not defined CV_HAS_LIBS (
        echo  OpenCV encontrado, pero sin bibliotecas MinGW. Se omite la biometria.
        set "CV_ROOT="
    )
)

set "CV_FLAGS="
set "CV_STD=-std=c++14"
set "CXX=g++"

if defined CV_ROOT (
    if exist "!CV_ROOT!\include\opencv5" (
        set "CV_STD=-std=c++17"
        set CV_INC=-I"!CV_ROOT!\include\opencv5"
    ) else if exist "!CV_ROOT!\include\opencv4" (
        set CV_INC=-I"!CV_ROOT!\include\opencv4"
    ) else (
        set CV_INC=-I"!CV_ROOT!\include"
    )

    set CV_LIB=
    if exist "!CV_ROOT!\x64\mingw\lib" set CV_LIB=-L"!CV_ROOT!\x64\mingw\lib"
    if exist "!CV_ROOT!\lib" set CV_LIB=!CV_LIB! -L"!CV_ROOT!\lib"

    rem OpenCV 5 traslada CascadeClassifier a opencv_xobjdetect.
    set CV_XOBJDETECT=
    if exist "!CV_ROOT!\lib\libopencv_xobjdetect.dll.a" set CV_XOBJDETECT=-lopencv_xobjdetect
    if exist "!CV_ROOT!\x64\mingw\lib\libopencv_xobjdetect.dll.a" set CV_XOBJDETECT=-lopencv_xobjdetect

    if exist "!CV_ROOT!\bin\g++.exe" set "CXX=!CV_ROOT!\bin\g++.exe"
    if exist "!CV_ROOT!\bin" set "PATH=!CV_ROOT!\bin;!PATH!"

    set CV_FLAGS=-DAXIS_HAVE_OPENCV !CV_INC! !CV_LIB! -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_objdetect !CV_XOBJDETECT! -lopencv_face

    echo  OpenCV detectado en: !CV_ROOT!
    echo  Compilador: !CXX!
    echo  Biometria facial: ACTIVADA
) else (
    echo  OpenCV no detectado. Se compilara sin biometria facial.
    echo  Para activarla, instale OpenCV y defina OPENCV_DIR.
)
echo -------------------------------------------------------------------

set "FILES="
for /r %%F in (*.cpp) do set "FILES=!FILES! "%%F""

"!CXX!" !CV_STD! !FILES! -I . !CV_FLAGS! -Wall -Wextra -o sistema_axis.exe

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
