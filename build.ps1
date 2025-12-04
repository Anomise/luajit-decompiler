# PowerShell build script for LuaJIT Decompiler v2 with Elsword modified header support
# Supports: Visual Studio (cl.exe) and MinGW (g++)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "LuaJIT Decompiler v2 Build Script" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$compilerFound = $false
$useMSVC = $false

# Check for Visual Studio compiler
$clPath = Get-Command cl -ErrorAction SilentlyContinue
if ($clPath) {
    Write-Host "[*] Found Visual Studio compiler (cl.exe)" -ForegroundColor Green
    $compilerFound = $true
    $useMSVC = $true
}

# Check for MinGW/GCC compiler
if (-not $compilerFound) {
    $gppPath = Get-Command g++ -ErrorAction SilentlyContinue
    if ($gppPath) {
        Write-Host "[*] Found MinGW/GCC compiler (g++)" -ForegroundColor Green
        $compilerFound = $true
        $useMSVC = $false
    }
}

# Try to find Visual Studio and initialize environment
if (-not $compilerFound) {
    $vsPaths = @(
        "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat",
        "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat",
        "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
    )
    
    foreach ($vsPath in $vsPaths) {
        if (Test-Path $vsPath) {
            Write-Host "[*] Found Visual Studio, initializing..." -ForegroundColor Yellow
            $vsDir = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $vsPath))
            $vcvarsPath = Join-Path $vsDir "VC\Auxiliary\Build\vcvars64.bat"
            
            # Initialize VS environment
            cmd /c "`"$vcvarsPath`" && set" | ForEach-Object {
                if ($_ -match '^([^=]+)=(.*)$') {
                    [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2])
                }
            }
            
            $compilerFound = $true
            $useMSVC = $true
            break
        }
    }
}

if (-not $compilerFound) {
    Write-Host "[ERROR] No C++ compiler found!" -ForegroundColor Red
    Write-Host ""
    Write-Host "Please install one of the following:" -ForegroundColor Yellow
    Write-Host "  1. Visual Studio (with C++ development tools)" -ForegroundColor Yellow
    Write-Host "  2. MinGW-w64 (https://www.mingw-w64.org/)" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Or manually set up the compiler environment and run this script again." -ForegroundColor Yellow
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""

if ($useMSVC) {
    Write-Host "[*] Building with Visual Studio compiler..." -ForegroundColor Cyan
    Write-Host ""
    
    $sourceFiles = @(
        "main.cpp",
        "bytecode\bytecode.cpp",
        "bytecode\prototype.cpp",
        "lua\lua.cpp",
        "ast\ast.cpp"
    )
    
    $args = @("/EHsc", "/std:c++17", "/O2", "/W3") + $sourceFiles + @("/Fe:luajit-decompiler-v2.exe", "/link", "/SUBSYSTEM:CONSOLE")
    
    & cl.exe $args
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host ""
        Write-Host "[SUCCESS] Build completed! Executable: luajit-decompiler-v2.exe" -ForegroundColor Green
    } else {
        Write-Host ""
        Write-Host "[ERROR] Build failed!" -ForegroundColor Red
        Read-Host "Press Enter to exit"
        exit 1
    }
} else {
    Write-Host "[*] Building with MinGW/GCC compiler..." -ForegroundColor Cyan
    Write-Host ""
    
    $sourceFiles = @(
        "main.cpp",
        "bytecode/bytecode.cpp",
        "bytecode/prototype.cpp",
        "lua/lua.cpp",
        "ast/ast.cpp"
    )
    
    & g++ -std=c++17 -O2 -Wall -Wextra $sourceFiles -o luajit-decompiler-v2.exe
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host ""
        Write-Host "[SUCCESS] Build completed! Executable: luajit-decompiler-v2.exe" -ForegroundColor Green
    } else {
        Write-Host ""
        Write-Host "[ERROR] Build failed!" -ForegroundColor Red
        Read-Host "Press Enter to exit"
        exit 1
    }
}

Write-Host ""
Read-Host "Press Enter to exit"

