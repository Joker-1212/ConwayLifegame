import os

class Config:
    # 环境参数
    ENV_WIDTH = 100
    ENV_HEIGHT = 100
    CONFIG_FILE = "config.txt"
    
    # 训练参数
    LEARNING_RATE = 1e-3
    GAMMA = 0.99
    BATCH_SIZE = 32
    BUFFER_SIZE = 10000
    TARGET_UPDATE = 100
    INITIAL_CELLS_POTION = 0.5 
    # 模型参数
    HIDDEN_SIZE = 128
    
    # 训练控制
    MAX_EPISODES = 1000
    MAX_STEPS = 1000
    EPSILON_START = 1.0
    EPSILON_END = 0.01
    EPSILON_DECAY = 0.995

    
    @classmethod
    def load_from_file(cls, config_file="config.txt"):
        """从配置文件加载参数（如果需要的话）"""
        if os.path.exists(config_file):
            # 这里可以添加从配置文件读取参数的逻辑
            # 目前我们主要依赖C++端的配置
            pass
