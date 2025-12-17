import torch
import torch.optim as optim
import torch.nn.functional as F
import numpy as np
from Module.Environment.game_env import SmartGameEnv
from Module.Models.policy_network import DQNetwork
from Module.Agent.cell_agent import CellAgent
from Module.Utils.experience_replay import ExperienceReplay
from Module.Configs.config import Config
import os

def train():
    # 初始化环境
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    configs = Config()

    env = SmartGameEnv(
        width=configs["ENV_WIDTH"],
        height=configs["ENV_HEIGHT"],
        config_file=configs["CONFIG_FILE"]
    )
    
    # 打印当前配置
    # print("Initial configuration:")
    # env.print_config()
    
    # 初始化模型和智能体
    state_size = env.state_size 
    action_size = env.action_size

    
    policy_net = DQNetwork(state_size, action_size, configs["HIDDEN_SIZE"]) # 策略网络
    target_net = DQNetwork(state_size, action_size, configs["HIDDEN_SIZE"]) # 目标网络
    target_net.load_state_dict(policy_net.state_dict()) # 
    
    agent = CellAgent(policy_net, state_size, action_size)
    optimizer = optim.Adam(policy_net.parameters(), lr=configs["LEARNING_RATE"])
    replay_buffer = ExperienceReplay(configs["BUFFER_SIZE"])
    
    epsilon = configs["EPSILON_START"]
    episode_rewards = []
    
    for episode in range(configs["MAX_EPISODES"]):
        state = env.reset(int(configs["INITIAL_CELLS_POTION"] * configs["ENV_WIDTH"] * configs["ENV_HEIGHT"]))
        # print(f"Initial population: {env.get_population()}, Density: {env.get_density():.3f}")
        total_reward = 0
        steps = 0
        
        # for i in env.get_grid_state():
        #     for j in i:
        #         print('#' if j == 1 else ' ', end='')
        #     print()
        # print("--------------------------------")

        while steps < configs["MAX_STEPS"]:
            # 获取动作
            initial = env.get_population()
            actions = agent.act(state, epsilon)
            
            # 执行动作
            next_state, reward, done, _ = env.step(actions, steps)
            
            # 存储经验
            # 注意：这里需要为每个细胞单独存储经验
            if len(state) > 0:
                for i in range(len(state)):
                    replay_buffer.push(
                        state[i], actions[i], reward,  # type: ignore
                        next_state[i] if i < len(next_state) else np.zeros(state_size),
                        done
                    )
            
            state = next_state
            total_reward += reward
            steps += 1

            # for i in env.get_grid_state():
            #     for j in i:
            #         print('#' if j == 1 else ' ', end='')
            #     print()
            # print("--------------------------------")
            # print(f"Step: {steps}, Reward: {reward:.3f}, Total Reward: {total_reward:.3f}, Population: {env.get_population()}, Decline: {(initial - env.get_population()) / initial:.3f}", end='\n')

            
            # 训练模型
            if len(replay_buffer) > configs["BATCH_SIZE"]:
                train_model(policy_net, target_net, optimizer, replay_buffer, configs)
            
            if done:
                break
        
        # 更新目标网络
        if episode % configs["TARGET_UPDATE"] == 0:
            target_net.load_state_dict(policy_net.state_dict())
        
        # 衰减epsilon
        epsilon = max(configs["EPSILON_END"], epsilon * configs["EPSILON_DECAY"])
        
        episode_rewards.append(total_reward)
        print(f"Episode {episode}, Reward: {total_reward:.3f}, "
              f"Population: {env.get_population()}, Epsilon: {epsilon:.3f}, Steps: {steps}")
    
    # 保存模型
    torch.save(policy_net.state_dict(), "trained_model.pth")

def train_model(policy_net, target_net, optimizer, replay_buffer, config):
    """
    train_model 的 Docstring
    
    :param policy_net: 要训练的策略网络
    :param replay_buffer: 经验回放
    :param config: 配置分析器
    """
    states, actions, rewards, next_states, dones = replay_buffer.sample(config["BATCH_SIZE"])
    
    states = torch.FloatTensor(states)
    actions = torch.LongTensor(actions)
    rewards = torch.FloatTensor(rewards)
    next_states = torch.FloatTensor(next_states)
    dones = torch.BoolTensor(dones)
    
    # 计算当前Q值
    current_q = policy_net(states).gather(1, actions.unsqueeze(1))
    
    # 计算目标Q值
    next_q = target_net(next_states).max(1)[0].detach()
    target_q = rewards + (config["GAMMA"] * next_q * ~dones)
    
    # 计算损失
    loss = F.mse_loss(current_q.squeeze(), target_q)
    
    # 反向传播
    optimizer.zero_grad()
    loss.backward()
    optimizer.step()

if __name__ == "__main__":
    train()
