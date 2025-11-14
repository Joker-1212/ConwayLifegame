import os
import sys
import numpy as np
import smart_life_core

class SmartGameEnv:
    def __init__(self, width=50, height=50, config_file="config.txt"):
        # 确保配置文件存在
        pass
        
    def _create_default_config(self, config_file):
        """创建默认配置文件"""
        pass
    
    def reset(self, num_cells=100):
        """重置环境"""
        self.env.initialize_random(num_cells)
        return self._get_observation()
    
    def step(self, actions=None):
        """执行一步"""
        if actions is None:
            self.env.update()
        else:
            self.env.update_with_moves(actions)
        
        obs = self._get_observation()
        reward = self._calculate_reward()
        done = self._is_done()
        info = {
            'population': self.env.get_population(),
            'density': self.env.get_density()
        }
        
        return obs, reward, done, info
    
    def _get_observation(self):
        """获取观察状态"""
        try:
            return self.env.get_cell_states()
        except Exception as e:
            print(f"Error getting cell states: {e}")
            return np.array([])
    
    def _calculate_reward(self):
        """计算奖励"""
        population = self.env.get_population()
        density = self.env.get_density()
        
        # 基础奖励：种群数量
        reward = population / (self.width * self.height)
        
        # 密度惩罚：避免过度拥挤或过于稀疏
        if density > 0.7 or density < 0.1:
            reward -= 0.1
            
        return reward
    
    def _is_done(self):
        """检查是否结束"""
        population = self.env.get_population()
        return population == 0 or population >= self.width * self.height * 0.8
    
    def get_population(self):
        return self.env.get_population()
    
    def get_density(self):
        return self.env.get_density()
    
    def get_grid_state(self):
        """获取网格状态（用于可视化）"""
        try:
            return self.env.get_grid_state()
        except Exception as e:
            print(f"Error getting grid state: {e}")
            return np.array([])
    
    def get_cell_positions(self):
        """获取细胞位置信息"""
        try:
            return self.env.get_cell_positions()
        except Exception as e:
            print(f"Error getting cell positions: {e}")
            return []
    
    def reload_config(self):
        """重新加载配置"""
        self.env.reload_config()
        self.vision_d = self.config_parser.get_int("Vision", 5)
        self.state_size = (2 * self.vision_d + 1) ** 2
    
    def print_config(self):
        """打印当前配置"""
        self.env.print_config()
