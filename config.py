import torch

class Config:
    # 环境参数
    GRID_SIZE = 20
    VISION_DISTANCE = 5
    DEATH_PROBABILITY = 0.1
    
    # 训练参数
    DEVICE = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    LEARNING_RATE = 0.001
    GAMMA = 0.99  # 折扣因子
    BATCH_SIZE = 32
    BUFFER_SIZE = 10000
    NUM_EPISODES = 1000
    MAX_STEPS = 200
    
    # 网络参数
    HIDDEN_SIZE = 128
    
    # 探索参数
    EPSILON_START = 1.0
    EPSILON_END = 0.1
    EPSILON_DECAY = 0.995
    
config = Config()
