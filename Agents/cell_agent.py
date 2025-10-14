import torch
import torch.nn as nn
import numpy as np
import torch.nn.functional as F

class CellAgent:
    def __init__(self, model, state_size, action_size=9) -> list[int]:
        self.model = model
        self.state_size = state_size
        self.action_size = action_size

    def act(self, state, epsilon=0.0):
        if state.size == 0:
            return []
        
        # ε-贪婪策略选择动作
        if np.random.random() < epsilon:
            return np.random.randint(0, self.action_size, size=state.shape[0]).tolist()
        else:
            self.model.eval()
            with torch.no_grad():
                state_tensor = torch.FloatTensor(state)
                q_values = self.model(state_tensor)
                actions = torch.argmax(q_values, dim=1)
            return actions.cpu().numpy().tolist()
        
    def get_action_probabilities(self, state):
        if state.size == 0:
            return torch.zeros(0, self.action_size)

        # 前馈传播获取动作概率
        state_tensor = torch.FloatTensor(state)
        action_logits = self.model(state_tensor)
        return F.softmax(action_logits, dim=-1)
