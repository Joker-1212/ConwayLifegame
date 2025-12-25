import random
from collections import deque
import numpy as np


class ExperienceReplay:
    """
    经验回放缓冲区，用于存储和采样智能体的经验（状态、动作、奖励等）。
    
    在强化学习中，经验回放通过打破经验之间的相关性来提高训练的稳定性。
    缓冲区采用固定大小的先进先出队列，当达到最大容量时自动丢弃最旧的经验。
    
    Attributes:
        buffer (deque): 存储经验元组的双端队列。
    """
    
    def __init__(self, max_size=10000):
        """
        初始化经验回放缓冲区。
        
        Args:
            max_size (int): 缓冲区的最大容量，默认为10000。
        """
        self.buffer = deque(maxlen=max_size)

    def push(self, state, action, reward, next_state, done):
        """
        将一条经验（状态、动作、奖励、下一个状态、结束标志）存入缓冲区。
        
        Args:
            state: 当前状态，可以是任意类型（通常是 numpy 数组或张量）。
            action: 执行的动作，可以是整数（离散动作）或浮点数（连续动作）。
            reward: 获得的奖励，标量值。
            next_state: 下一个状态，与 state 类型相同。
            done: 布尔值，表示该回合是否结束。
        """
        self.buffer.append((state, action, reward, next_state, done))

    def sample(self, batch_size):
        """
        从缓冲区中随机采样一批经验。
        
        Args:
            batch_size (int): 采样的批次大小。
            
        Returns:
            tuple: 包含五个元素的元组 (state_batch, action_batch, reward_batch, next_state_batch, done_batch)，
                   每个元素都是 numpy 数组，形状为 (batch_size, ...)。
            
        Raises:
            ValueError: 如果缓冲区中的经验数量小于 batch_size。
        """
        if len(self.buffer) < batch_size:
            raise ValueError(f"缓冲区中只有 {len(self.buffer)} 条经验，无法采样 {batch_size} 条")
        
        # 随机采样 batch_size 条经验
        batch = random.sample(self.buffer, batch_size)
        
        # 将 batch 中的元组按列解压，然后使用 np.stack 将每个列堆叠成数组
        # zip(*batch) 将 [(s1,a1,r1,ns1,d1), (s2,a2,r2,ns2,d2), ...] 转换为
        # ( (s1,s2,...), (a1,a2,...), (r1,r2,...), (ns1,ns2,...), (d1,d2,...) )
        # map(np.stack, ...) 对每个列应用 np.stack，将列表转换为 numpy 数组
        state, action, reward, next_state, done = map(np.stack, zip(*batch))
        
        return state, action, reward, next_state, done

    def __len__(self):
        """
        返回缓冲区中当前存储的经验数量。
        
        Returns:
            int: 缓冲区中的经验条数。
        """
        return len(self.buffer)

