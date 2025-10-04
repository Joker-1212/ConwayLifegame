cmake_minimum_required(VERSION 3.12)
project(SmartLifeBindings)

# 查找 Python 和 PyBind11
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 REQUIRED)

# 包含目录
include_directories(
    ${pybind11_INCLUDE_DIRS}
    ../cpp_core/include
)

# Windows 特定设置
if(WIN32)
    if(MSVC)
        add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/EHsc>")
        add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/W3>")
    endif()
endif()

# 添加 PyBind11 模块
pybind11_add_module(smart_life_core
    pybind_wrapper.cpp
)

# 设置库文件路径
if(WIN32)
    # Windows 上使用 .lib 扩展名
    set(CORE_LIB_NAME "smart_life_core.lib")
else()
    set(CORE_LIB_NAME "libsmart_life_core.a")
endif()

# 链接核心库
target_link_libraries(smart_life_core PRIVATE
    ${CMAKE_SOURCE_DIR}/../cpp_core/${CORE_LIB_NAME}
)

# 设置 C++ 标准
target_compile_features(smart_life_core PRIVATE cxx_std_14)

# Windows 特定设置
if(WIN32)
    set_target_properties(smart_life_core PROPERTIES
        SUFFIX ".pyd"
        PREFIX ""
    )
else()
    set_target_properties(smart_life_core PROPERTIES
        SUFFIX ".so"
        PREFIX ""
    )
endif()

# 设置输出目录
set_target_properties(smart_life_core PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/..
)
