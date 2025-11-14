import torch
import numpy as np
from environment.game_env import SmartGameEnv
from models.policy_network import DQNetwork
from agents.cell_agent import CellAgent
from config import Config

def evaluate(model_path, num_episodes=10):
    # 初始化环境
    env = SmartGameEnv(
        width=Config.ENV_WIDTH,
        height=Config.ENV_HEIGHT,
        config_file=Config.CONFIG_FILE
    )
    
    # 加载模型
    state_size = env.state_size
    action_size = env.action_size
    
    model = DQNetwork(state_size, Config.HIDDEN_SIZE, action_size)
    model.load_state_dict(torch.load(model_path))
    agent = CellAgent(model, state_size, action_size)
    
    results = []
    
    for episode in range(num_episodes):
        state = env.reset()
        total_reward = 0
        steps = 0
        
        while True:
            actions = agent.act(state, epsilon=0.0)  # 评估时不使用随机性
            next_state, reward, done, _ = env.step(actions)
            
            state = next_state
            total_reward += reward
            steps += 1
            
            if done:
                break
        
        final_population = env.get_population()
        results.append({
            'episode': episode,
            'total_reward': total_reward,
            'steps': steps,
            'final_population': final_population
        })
        
        print(f"Evaluation Episode {episode}: "
              f"Reward: {total_reward:.3f}, "
              f"Final Population: {final_population}")
    
    return results

if __name__ == "__main__":
    evaluate("trained_model.pth")
