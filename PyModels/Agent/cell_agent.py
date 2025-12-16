import torch
import numpy as np
import torch.nn.functional as F

class CellAgent:
    def __init__(self, model, state_size, action_size=9):
        self.model = model
        self.state_size = state_size
        self.action_size = action_size
    
    # cell = CellAgent(model, 9, 9)
    # cell.act(0, 1.0) -> CellAgent.act(cell, 0, 1.0)
    def act(self, state, epsilon=0.0):
        if state.size == 0:
            return []
            
        if np.random.random() < epsilon:
            # 随机动作
            return np.random.randint(0, self.action_size, len(state)).tolist()
        else:
            # 模型预测
            self.model.eval()
            with torch.no_grad():
                state_tensor = torch.FloatTensor(state)
                q_values = self.model(state_tensor)
                actions = torch.argmax(q_values, dim=1)
            return actions.numpy().tolist()
        # self.model.eval()
        # with torch.no_grad():
        #     state_tensor = torch.FloatTensor(state)
        #     q_values = self.model(state_tensor)
        #     actions = torch.argmax(q_values, dim=1)
        # return actions.numpy().tolist()
    
    def get_action_probabilities(self, state):
        if state.size == 0:
            return torch.zeros(0, self.action_size)
            
        state_tensor = torch.FloatTensor(state)
        action_logits = self.model(state_tensor)
        return F.softmax(action_logits, dim=1)
