add_library(game_environment_core STATIC
    src/game_environment.cpp
)

# 包含目录
target_include_directories(game_environment_core PUBLIC include)

# 编译定义
target_compile_definitions(game_environment_core PRIVATE 
    VERSION=\"1.0.0\"
)
