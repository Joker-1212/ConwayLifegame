#!/bin/bash

# ========================================
# Smart Game of Life - Linux Build Script
# ========================================

set -e  # 遇到错误立即退出

echo "========================================"
echo "Smart Game of Life - Linux Build Script"
echo "========================================"

# 检查必要的工具
check_dependency() {
    if ! command -v "$1" &> /dev/null; then
        echo "错误: 未找到 $1"
        echo "请安装 $1 并将其添加到 PATH"
        exit 1
    fi
}

echo "检查依赖项..."
check_dependency "cmake"
check_dependency "g++"
check_dependency "python3"
check_dependency "pip3"

# 检查Python开发包
if ! python3 -c "import sysconfig; print(sysconfig.get_path('include'))" &> /dev/null; then
    echo "错误: Python开发包未安装"
    echo "请安装 python3-dev 或 python3-devel 包"
    echo "例如:"
    echo "  Ubuntu/Debian: sudo apt-get install python3-dev"
    echo "  Fedora/RHEL: sudo dnf install python3-devel"
    echo "  Arch Linux: sudo pacman -S python"
    exit 1
fi

# 检查pybind11是否安装
if ! python3 -c "import pybind11" &> /dev/null; then
    echo "警告: pybind11 Python包未安装"
    echo "正在安装pybind11..."
    pip3 install pybind11
fi

# 设置构建目录
BUILD_DIR="build"
CORE_BUILD_DIR="$BUILD_DIR/cpp_core"
BINDINGS_BUILD_DIR="$BUILD_DIR/python_bindings"

# 清理之前的构建
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning previous build..."
    rm -rf "$BUILD_DIR"
fi

# 创建构建目录
mkdir -p "$BUILD_DIR"
mkdir -p "$CORE_BUILD_DIR"
mkdir -p "$BINDINGS_BUILD_DIR"

echo ""
echo "Step 1: Building C++ Core Library..."
echo ""

# 构建核心库
cd "$CORE_BUILD_DIR"
cmake ../../cpp_core -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "Error: CMake configuration failed for core library"
    cd ../..
    exit 1
fi

cmake --build . --config Release
if [ $? -ne 0 ]; then
    echo "错误: C++核心库构建失败"
    cd ../..
    exit 1
fi

# 复制生成的库文件到 cpp_core 目录
if [ -f "libsmart_life_core.a" ]; then
    cp libsmart_life_core.a ../../cpp_core/libsmart_life_core.a
    echo "Copied libsmart_life_core.lib to cpp_core directory"
elif [ -f "smart_life_core.a" ]; then
    cp smart_life_core.a ../../cpp_core/libsmart_life_core.a
    echo "Copied smart_life_core.a to cpp_core directory"
else
    echo "Warning: Could not find smart_life_core.lib in Release directory"
    # 尝试查找其他可能的库文件
    find . -name "*.a" -type f | head -5 | while read lib; do
        echo "Find library file: $lib"
    done
fi

cd ../..

echo ""
echo "Step 2: Building Python Bindings..."
echo ""

# 获取Python信息
PYTHON_EXECUTABLE=$(which python3)
PYTHON_VERSION=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")
PYTHON_INCLUDE_DIR=$(python3 -c "import sysconfig; print(sysconfig.get_path('include'))")
PYTHON_LIBRARY=$(python3 -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR'))")

# echo "Python 信息:"
# echo "  可执行文件: $PYTHON_EXECUTABLE"
# echo "  版本: $PYTHON_VERSION"
# echo "  包含目录: $PYTHON_INCLUDE_DIR"
# echo "  库目录: $PYTHON_LIBRARY"

# 构建 Python 绑定
cd "$BINDINGS_BUILD_DIR"

# 尝试查找pybind11的CMake配置
PYBIND11_CMAKE_DIR=""
if [ -d "/usr/local/share/cmake/pybind11" ]; then
    PYBIND11_CMAKE_DIR="/usr/local/share/cmake/pybind11"
elif [ -d "/usr/share/cmake/pybind11" ]; then
    PYBIND11_CMAKE_DIR="/usr/share/cmake/pybind11"
else
    # 尝试通过pip安装的路径查找
    PYBIND11_PATH=$(python3 -c "import pybind11; import os; print(os.path.join(os.path.dirname(pybind11.__file__), 'share', 'cmake', 'pybind11'))")
    if [ -d "$PYBIND11_PATH" ]; then
        PYBIND11_CMAKE_DIR="$PYBIND11_PATH"
    fi
fi

if [ -n "$PYBIND11_CMAKE_DIR" ]; then
    # echo "找到 pybind11 CMake 目录: $PYBIND11_CMAKE_DIR"
    cmake ../../python_bindings \
        -DCMAKE_BUILD_TYPE=Release \
        -DPython_ROOT_DIR=$(dirname $(dirname $PYTHON_EXECUTABLE)) \
        -DPYBIND11_CMAKE_DIR="$PYBIND11_CMAKE_DIR"
else
    # echo "警告: 未找到 pybind11 CMake 目录，尝试自动查找..."
    cmake ../../python_bindings \
        -DCMAKE_BUILD_TYPE=Release \
        -DPython_ROOT_DIR=$(dirname $(dirname $PYTHON_EXECUTABLE))
fi

if [ $? -ne 0 ]; then
    echo "Error: Build failed for Python bindings"
    cd ../..
    exit 1
fi

cmake --build . --config Release
if [ $? -ne 0 ]; then
    echo "Error: Build failed for Python bindings"
    cd ../..
    exit 1
fi

cd ../..

echo ""
echo "Step 3: Copying built modules..."
echo ""

# 复制生成的模块
MODULE_FOUND=false

# 查找生成的模块文件
find "$BINDINGS_BUILD_DIR" -name "*.so" -type f | while read module; do
    # echo "找到模块: $module"
    cp "$module" "./smart_life_core.so"
    echo "Copied smart_life_core.so to project root"
    MODULE_FOUND=true
    
    # 同时复制到Module/Environment目录
    if [ -d "Module/Environment" ]; then
        cp "$module" "Module/Environment/smart_life_core.so"
        echo "Copied to Module/Environment/smart_life_core.so"
    fi
done

if [ "$MODULE_FOUND" = false ]; then
    echo "Warning: Could not find built Python module"
    echo "在以下位置查找:"
    find "$BINDINGS_BUILD_DIR" -type f -name "*.so" -o -name "*.pyd" | head -10
fi

# 设置执行权限
chmod +x smart_life_core.so 2>/dev/null || true

echo ""
echo "========================================"
echo "Build completed successfully!"
echo "========================================"
echo ""
echo "You can now run"
echo "  python3 test_windows.py"
echo "  python3 GUI.py"
echo "  python3 train.py"
echo "  python3 evaluate.py ./Models/Default_model.pth"
echo ""
echo "Notice: On Linux, the extension for modules is .so instead of .pyd."
echo ""

