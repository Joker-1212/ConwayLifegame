import os

class Config:
    """
    配置用于模型训练的参数
    """

    def __init__(self):
        self.configs = dict()

        self.configs["ENV_WIDTH"] = 100
        self.configs["ENV_HEIGHT"] = 100
        self.configs["CONFIG_FILE"] = "config.txt"
        
        # 训练参数
        self.configs["LEARNING_RATE"] = 1e-3
        self.configs["GAMMA"] = 0.99
        self.configs["BATCH_SIZE"] = 32
        self.configs["BUFFER_SIZE"] = 10000
        self.configs["TARGET_UPDATE"] = 100
        self.configs["INITIAL_CELLS_POTION"] = 0.5 
        # 模型参数
        self.configs["HIDDEN_SIZE"] = 256
        
        # 训练控制
        self.configs["MAX_EPISODES"] = 1000
        self.configs["MAX_STEPS"] = 1000
        self.configs["EPSILON_START"] = 1.0
        self.configs["EPSILON_END"] = 0.01
        self.configs["EPSILON_DECAY"] = 0.995

        self.load_from_file()

    def load_from_file(self, config_file="config.txt"):
        """
        从配置文件中加载参数
        
        :param config_file: 配置文件路径
        """
        if os.path.exists(config_file):
            with open(config_file, "r") as f:
                while (str := f.readline()) != "":
                    if str.startswith("#") or str.strip() == "":
                        continue
                    key, value = str.split("=")
                    key = key.strip()
                    value = value.strip()
                    if value.replace('.', '', 1).isdigit():
                        if '.'in value:
                            value = float(value)
                        else:
                            value = int(value)
                    self.configs[key] = value
    
    def __setitem__(self, key, value):
        self.configs[key] = value
    
    def __getitem__(self, key):
        return self.configs[key]
