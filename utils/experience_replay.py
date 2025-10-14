import random
from collections import deque
import numpy as np

class ExperienceReplay:
    def __init__(self, max_size=10000):
        self.memory = deque(maxlen=max_size)

    def push(self, experience):
        self.memory.append(experience)

    def sample(self, batch_size):
        batch = random.sample(self.buffer, batch_size)
        state, action, reward, next_state, done = map(np.stack, zip(*batch))
        return state, action, reward, next_state, done

    def __len__(self):
        return len(self.memory)
