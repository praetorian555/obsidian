@echo off
setlocal

set SOURCE_DIR=%~dp0
if "%SOURCE_DIR:~-1%"=="\" set SOURCE_DIR=%SOURCE_DIR:~0,-1%
set BUILD_MSVC=%SOURCE_DIR%build\release
set BUILD_WSL=%SOURCE_DIR%build\release-wsl-gcc
set INSTALL_WIN32=%SOURCE_DIR%\install-dir\obsidian\win32
set INSTALL_LINUX=%SOURCE_DIR%\install-dir\obsidian\linux

set LLVM_PATH_WIN=F:\LLVM
set LLVM_PATH_LINUX=/usr/lib/llvm-20

set WSL_SOURCE=/mnt/d/Dev/obsidian
set WSL_BUILD=%WSL_SOURCE%/build/release-wsl-gcc
set WSL_INSTALL=%WSL_SOURCE%/install-dir/obsidian/linux

REM Extract version from CMakeLists.txt
for /f "tokens=2 delims=()" %%a in ('findstr /r "^project" "%SOURCE_DIR%\CMakeLists.txt"') do (
    for /f "tokens=3" %%v in ("%%a") do set PROJECT_VERSION=%%v
)
echo Project version: %PROJECT_VERSION%

REM ==================== MSVC Release ====================
echo.
echo ====================================================
echo  Building MSVC Release...
echo ====================================================
cmake -S "%SOURCE_DIR%" -B "%BUILD_MSVC%" -G "Visual Studio 17 2022" -DOBS_LLVM_PATH="%LLVM_PATH_WIN%"
if %errorlevel% neq 0 (
    echo ERROR: MSVC configure failed!
    exit /b 1
)

cmake --build "%BUILD_MSVC%" --config Release
if %errorlevel% neq 0 (
    echo ERROR: MSVC build failed!
    exit /b 1
)

cmake --install "%BUILD_MSVC%" --config Release --prefix "%INSTALL_WIN32%"
if %errorlevel% neq 0 (
    echo ERROR: MSVC install failed!
    exit /b 1
)

echo MSVC Release installed to: %INSTALL_WIN32%

REM ==================== GCC Release (WSL) ====================
echo.
echo ====================================================
echo  Building GCC Release via WSL...
echo ====================================================
wsl cmake -S "%WSL_SOURCE%" -B "%WSL_BUILD%" -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DOBS_LLVM_PATH="%LLVM_PATH_LINUX%"
if %errorlevel% neq 0 (
    echo ERROR: WSL configure failed!
    exit /b 1
)

wsl cmake --build "%WSL_BUILD%" --config Release
if %errorlevel% neq 0 (
    echo ERROR: WSL build failed!
    exit /b 1
)

wsl cmake --install "%WSL_BUILD%" --config Release --prefix "%WSL_INSTALL%"
if %errorlevel% neq 0 (
    echo ERROR: WSL install failed!
    exit /b 1
)

REM Replace libclang.so symlink with the actual file so Windows tar can zip it
wsl bash -c "rm -f '%WSL_INSTALL%/libclang.so' && cp -L '%LLVM_PATH_LINUX%/lib/libclang.so' '%WSL_INSTALL%/libclang.so'"
echo GCC Release installed to: %INSTALL_LINUX%

REM ==================== Package ====================
echo.
echo ====================================================
echo  Packaging...
echo ====================================================
pushd "%SOURCE_DIR%\install-dir"
tar -a -cf obsidian-%PROJECT_VERSION%.zip obsidian
popd
if %errorlevel% neq 0 (
    echo ERROR: Packaging failed!
    exit /b 1
)

echo.
echo ====================================================
echo  Build complete!
echo  Win32: %INSTALL_WIN32%
echo  Linux: %INSTALL_LINUX%
echo  Package: %SOURCE_DIR%\install-dir\obsidian-%PROJECT_VERSION%.zip
echo ====================================================