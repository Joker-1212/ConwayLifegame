cmake_minimum_required(VERSION 3.12)
project(SmartLifeCore)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows 特定设置
if(WIN32)
    # 设置运行时库
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/EHsc>")
        add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/W3>")
    endif()
    
    # Windows 上静态库的命名
    set(CMAKE_STATIC_LIBRARY_PREFIX "")
    set(CMAKE_STATIC_LIBRARY_SUFFIX ".lib")
endif()

# 添加源文件
set(SOURCES
    src/game_environment.cpp
    src/config_parser.cpp
)

# 创建静态库
add_library(smart_life_core STATIC ${SOURCES})

# 包含目录
target_include_directories(smart_life_core PUBLIC include)

# 设置输出目录
set_target_properties(smart_life_core PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Windows 导出符号
if(WIN32)
    target_compile_definitions(smart_life_core PRIVATE SMART_LIFE_CORE_EXPORT)
endif()
