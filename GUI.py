import dearpygui.dearpygui as dpg
from agents.cell_agent import CellAgent
from models.policy_network import DQNetwork
import datetime
import train
import queue

class GUI:
    def __init__(self):
        self.env = None
        self.agent = None
        self.is_running = False
        self.is_training = False
        self.size = 8
        self.update_interval = 0.1
        self.debug = False
        self.step_count = 0
        self.edit_mode = True
        self.show_grid_line = True

        self.log_queue = queue.Queue()
        self.gui_ready = False
        

    def initialize_environment(self):
        pass

    def initialize_gui(self):
        pass
    

    def log(self, message):
        pass

    def run(self):
        pass

if __name__ == "__main__":
    gui = GUI()
    gui.run()

