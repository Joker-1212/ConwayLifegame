cmake_minimum_required(VERSION 3.12)
project(SmartGameOfLife)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Python（用于pybind11）
find_package(Python REQUIRED COMPONENTS Development.Module)

# 添加pybind11
find_package(pybind11 CONFIG REQUIRED)

# 添加核心库
add_subdirectory(cpp_core)

# 创建Python模块
pybind11_add_module(game_environment
    python_bindings/game_bindings.cpp
)

# 链接核心库
target_link_libraries(game_environment PRIVATE game_environment_core)

# 安装目标
install(TARGETS game_environment DESTINATION .)
