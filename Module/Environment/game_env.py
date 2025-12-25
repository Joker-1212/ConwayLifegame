import os
import sys
import numpy as np
import smart_life_core
from ..Configs.config import Config

class SmartGameEnv:
    def __init__(self, width=50, height=50, config_file=".\\config.txt"):
        # 确保配置文件存在
        if not os.path.exists(config_file):
            self._create_default_config(config_file)
            
        self.env = smart_life_core.GameEnvironment(width, height, config_file)
        self.width = width
        self.height = height
        self.configs = Config()
        
        # 从配置获取视野距离
        self.vision_d = self.configs["VISION"]
        self.state_size = (2 * self.vision_d + 1) ** 2
        self.action_size = 9  # 8个方向 + 不动
        
    def _create_default_config(self, config_file):
        """创建默认配置文件"""
        default_config = """# Smart Game of Life Configuration
# Minimum number of neighbors for a cell to survive
LIVE_MIN = 2

# Maximum number of neighbors for a cell to survive  
LIVE_MAX = 3

# Minimum number of neighbors for a cell to be born
BREED_MIN = 3

# Maximum number of neighbors for a cell to be born
BREED_MAX = 3

# Cell vision distance
VISION = 5

# Cell death probability
DEATH_RATE = 0.1

# Cell energy consume
ENERGY_CONSUMPTION = 0.1

# Cell energy restore rate
RESTORE_PROB = 0.1
RESTORE_VALUE = 0.2

# Grid size
ENV_WIDTH = 100
ENV_HEIGHT = 100
"""
        with open(config_file, 'w') as f:
            f.write(default_config)
        print(f"Created default config file: {config_file}")
    
    def reset(self, num_cells=100):
        """
        重置环境
        """
        self.env.initialize_random(num_cells)
        return self.get_observation()
    
    def is_position_empty(self, x, y):
        return self.env.is_position_empty(x, y)
    
    def step(self, actions=None, step=0):
        """
        执行一步
        """
        if actions is None:
            self.env.update()
        else:
            self.env.update_with_moves(actions)
        
        obs = self.get_observation()
        reward = self.__calculate_reward(step)
        done = self.__is_done()
        info = {
            'population': self.env.get_population(),
            'density': self.env.get_density()
        }
        
        return obs, reward, done, info
    
    def get_observation(self):
        """
        获取每一个细胞周围的邻居状态
        """
        try:
            return self.env.get_cell_states()
        except Exception as e:
            print(f"Error getting cell states: {e}")
            return np.array([])
    
    def __calculate_reward(self, step):
        """
        计算奖励
        """
        population = self.env.get_population()
        density = self.env.new_density()
        
        # 基础奖励：种群数量
        reward = population / (self.width * self.height)

        # 密度额外奖励或惩罚：避免过度拥挤或过于稀疏
        reward += 0.1
        if density <= (self.configs["LIVE_MIN"] - 1.0) / 8 or density >= (self.configs["LIVE_MAX"] + 1.0) / 8:
            reward -= 0.1
        
        if density <= (self.configs["BREED_MIN"] - 1.0) / 8 or density >= (self.configs["BREED_MAX"] + 1.0) / 8:
            reward -= 0.1

        # 额外奖励：种群存活时间
        reward += step * 0.002

        if step == 500:
            reward += population / (self.width * self.height) * 2

        return reward
    
    def __is_done(self):
        """
        检查是否结束（即细胞是否全部死亡）
        """
        population = self.env.get_population()
        return population == 0 or population >= self.width * self.height * 0.8
    
    def get_population(self):
        return self.env.get_population()
    
    def set_cell(self, x, y):
        self.env.set_cell(int(x), int(y))

    def remove_cell(self, x, y):
        self.env.remove_cell(int(x), int(y))
    
    def get_density(self):
        return self.env.get_density()
    
    def get_grid_state(self):
        """
        获取网格状态（用于可视化）
        """
        try:
            return self.env.get_grid_state()
        except Exception as e:
            print(f"Error getting grid state: {e}")
            return np.array([])
    
    def get_cells(self):
        """
        获取所有活细胞的具体信息
        """
        try:
            return self.env.get_cells()
        except Exception as e:
            print(f"Error getting cell positions: {e}")
            return []
    
    def reload_config(self):
        """
        重新加载配置
        """
        self.env.reload_config()
        self.vision_d = self.config_parser.get_int("Vision", 5)
        self.state_size = (2 * self.vision_d + 1) ** 2
    
    def print_config(self):
        """
        打印当前配置
        """
        self.env.print_config()
