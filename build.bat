@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo Smart Game of Life - Windows Build Script
echo ========================================

:: 检查必要的工具
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: CMake not found in PATH
    echo Please install CMake and add it to PATH
    exit /b 1
)

:: 设置构建目录
set BUILD_DIR=build
set CORE_BUILD_DIR=%BUILD_DIR%\cpp_core
set BINDINGS_BUILD_DIR=%BUILD_DIR%\python_bindings

:: 清理之前的构建
if exist %BUILD_DIR% (
    echo Cleaning previous build...
    rmdir /s /q %BUILD_DIR%
)

:: 创建构建目录
mkdir %BUILD_DIR%
mkdir %CORE_BUILD_DIR%
mkdir %BINDINGS_BUILD_DIR%

echo.
echo Step 1: Building C++ Core Library...
echo.

:: 构建核心库
cd %CORE_BUILD_DIR%
cmake ..\..\cpp_core -A x64
if %ERRORLEVEL% neq 0 (
    echo Error: CMake configuration failed for core library
    cd ..\..
    exit /b 1
)

cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Error: Build failed for core library
    cd ..\..
    exit /b 1
)

:: 复制生成的库文件到 cpp_core 目录
if exist "Release\smart_life_core.lib" (
    copy "Release\smart_life_core.lib" "..\..\cpp_core\smart_life_core.lib"
    echo Copied smart_life_core.lib to cpp_core directory
) else (
    echo Warning: Could not find smart_life_core.lib in Release directory
)

cd ..\..

echo.
echo Step 2: Building Python Bindings...
echo.

:: 构建 Python 绑定
cd %BINDINGS_BUILD_DIR%
cmake ..\..\python_bindings -A x64 ^
    -Dpybind11_DIR=%CONDA_PREFIX%\Lib\site-packages\pybind11\share\cmake\pybind11 ^
    -DPython_ROOT_DIR=%CONDA_PREFIX% ^
    -DCMAKE_PREFIX_PATH=%CONDA_PREFIX%\Library

if %ERRORLEVEL% neq 0 (
    echo Error: CMake configuration failed for Python bindings
    cd ..\..
    exit /b 1
)

cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Error: Build failed for Python bindings
    cd ..\..
    exit /b 1
)

cd ..\..

echo.
echo Step 3: Copying built modules...
echo.

:: 复制生成的模块
if exist "%BINDINGS_BUILD_DIR%\Release\smart_life_core.pyd" (
    copy "%BINDINGS_BUILD_DIR%\Release\smart_life_core.pyd" "smart_life_core.pyd"
    echo Copied smart_life_core.pyd to project root
) else if exist "%BINDINGS_BUILD_DIR%\smart_life_core.pyd" (
    copy "%BINDINGS_BUILD_DIR%\smart_life_core.pyd" "smart_life_core.pyd"
    echo Copied smart_life_core.pyd to Module\Environment\smart_life_core.pyd
) else (
    echo Warning: Could not find built Python module
)

copy .\Release\smart_life_core.pyd .\smart_life_core.pyd
copy .\Release\smart_life_core.pyd .\PyModels\Environment\smart_life_core.pyd

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo You can now run:
echo   python .\test_windows.py
echo   python .\GUI.py
echo   python .\train.py
echo   python .\evaluate.py .\Models\Default_model.pth
echo.
