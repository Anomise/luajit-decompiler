@echo off
REM Build script for LuaJIT Decompiler v2 with Elsword modified header support
REM Supports: Visual Studio (cl.exe) and MinGW (g++)

setlocal enabledelayedexpansion

echo ========================================
echo LuaJIT Decompiler v2 Build Script
echo ========================================
echo.

REM Check for Visual Studio compiler
where cl >nul 2>&1
if %errorlevel% == 0 (
    echo [*] Found Visual Studio compiler (cl.exe)
    goto :build_msvc
)

REM Check for MinGW/GCC compiler
where g++ >nul 2>&1
if %errorlevel% == 0 (
    echo [*] Found MinGW/GCC compiler (g++)
    goto :build_mingw
)

REM Check for Visual Studio in common locations
set VS_FOUND=0

if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    echo [*] Found Visual Studio 2022, initializing...
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
    set VS_FOUND=1
)

if %VS_FOUND%==0 if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    echo [*] Found Visual Studio 2022 Professional, initializing...
    call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
    set VS_FOUND=1
)

if %VS_FOUND%==0 if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
    echo [*] Found Visual Studio 2019, initializing...
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
    set VS_FOUND=1
)

if %VS_FOUND%==0 if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" (
    echo [*] Found Visual Studio 2017, initializing...
    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
    set VS_FOUND=1
)

REM Verify cl.exe is available after initialization
if %VS_FOUND%==1 (
    where cl >nul 2>&1
    if %errorlevel% == 0 (
        goto :build_msvc
    ) else (
        echo [WARNING] Visual Studio found but cl.exe not available after initialization.
        echo [WARNING] Trying alternative method...
        set VS_FOUND=0
    )
)

REM No compiler found
echo [ERROR] No C++ compiler found!
echo.
echo Please install one of the following:
echo   1. Visual Studio (with C++ development tools)
echo   2. MinGW-w64 (https://www.mingw-w64.org/)
echo.
echo Or manually set up the compiler environment and run this script again.
pause
exit /b 1

:build_msvc
echo.
echo [*] Building with Visual Studio compiler...
echo.

REM Compile all source files
REM /J flag makes char unsigned (required by this codebase)
REM /std:c++20 required for designated initializers, std::bit_cast, and std::unordered_map::contains
cl /EHsc /std:c++20 /O2 /W3 /J ^
   main.cpp ^
   bytecode\bytecode.cpp ^
   bytecode\prototype.cpp ^
   lua\lua.cpp ^
   ast\ast.cpp ^
   /Fe:luajit-decompiler-v2.exe ^
   /link /SUBSYSTEM:CONSOLE user32.lib comdlg32.lib shlwapi.lib

if %errorlevel% == 0 (
    echo.
    echo [SUCCESS] Build completed! Executable: luajit-decompiler-v2.exe
) else (
    echo.
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

goto :end

:build_mingw
echo.
echo [*] Building with MinGW/GCC compiler...
echo.

REM Compile all source files
g++ -std=c++17 -O2 -Wall -Wextra ^
    main.cpp ^
    bytecode/bytecode.cpp ^
    bytecode/prototype.cpp ^
    lua/lua.cpp ^
    ast/ast.cpp ^
    -o luajit-decompiler-v2.exe

if %errorlevel% == 0 (
    echo.
    echo [SUCCESS] Build completed! Executable: luajit-decompiler-v2.exe
) else (
    echo.
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

:end
echo.
pause

