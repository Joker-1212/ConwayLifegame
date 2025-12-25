import torch
import numpy as np
from Module.Environment.game_env import SmartGameEnv
from Module.Models.policy_network import DQNetwork
from Module.Agent.cell_agent import CellAgent
from Module.Configs.config import Config
import sys
import os


def evaluate(model_path, num_episodes=10):
    """
    评估模型在环境中的表现。
    
    Args:
        model_path (str): 预训练模型文件的路径（.pth 格式）。
        num_episodes (int): 评估的回合数，默认为10。
        
    Returns:
        list[dict]: 每个回合的评估结果列表，每个元素是一个包含以下键的字典：
            - 'episode': 回合索引（从0开始）
            - 'total_reward': 该回合累计奖励
            - 'steps': 该回合执行的总步数
            - 'final_population': 回合结束时的细胞种群数量
    """
    # 初始化环境
    env = SmartGameEnv(
        width=Config["ENV_WIDTH"],
        height=Config["ENV_HEIGHT"],
        config_file=Config["CONFIG_FILE"]
    )
    
    # 加载模型
    state_size = env.state_size      # 状态空间的维度
    action_size = env.action_size    # 动作空间的维度
    
    model = DQNetwork(state_size, Config["HIDDEN_SIZE"], action_size)
    # 加载预训练的权重
    model.load_state_dict(torch.load(model_path))
    # 创建智能体，设置 epsilon=0 以完全使用贪心策略
    agent = CellAgent(model, state_size, action_size)
    
    results = []  # 存储每个回合的评估结果
    
    for episode in range(num_episodes):
        # 重置环境，获取初始状态
        state = env.reset()
        total_reward = 0
        steps = 0
        
        # 运行一个完整的回合直到结束
        while True:
            # 智能体根据当前状态选择动作（评估时 epsilon=0，即完全贪心）
            actions = agent.act(state, epsilon=0.0)
            # 执行动作，获取下一个状态、奖励、结束标志等信息
            next_state, reward, done, _ = env.step(actions)
            
            # 更新状态和累计奖励
            state = next_state
            total_reward += reward
            steps += 1
            
            # 如果回合结束，跳出循环
            if done:
                break
        
        # 回合结束后获取当前环境的细胞种群数量，记录该回合的评估指标
        final_population = env.get_population()
        results.append({
            'episode': episode,
            'total_reward': total_reward,
            'steps': steps,
            'final_population': final_population
        })
        
        # 打印当前回合的评估结果
        print(f"Evaluation Episode {episode}: "
              f"Reward: {total_reward:.3f}, "
              f"Final Population: {final_population}")
    
    return results

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("No enough parameter, please provide the path to the model")
    else:
        if os.path.exists(sys.argv[1]) and sys.argv[1].endswith(".pth"):
            evaluate(sys.argv[1])
        else:
            print("File does not exist")
