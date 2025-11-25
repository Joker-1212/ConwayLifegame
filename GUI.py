import dearpygui.dearpygui as dpg
from agents.cell_agent import CellAgent
from models.policy_network import DQNetwork
import datetime
import train
import queue
import threading
import os
import sys
import subprocess

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

    def log(self, message, level="INFO"):
        """
        记录日志信息到日志队列中。
        
        :param message: 日志信息
        :param level: 日志等级
        """
        timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        formatted_message = f"[{timestamp}] [{level}] {message}"
        self.log_queue.put(formatted_message)

    def process_log(self):
        """
        处理积压的日志消息
        为保证线程安全，该函数须在主线程调用
        """
        if not self.gui_ready:
            return 
        
        try:
            while not self.log_queue.empty():
                log_message = self.log_queue.get_nowait()

                current_logs = dpg.get_value("Logs")
                if current_logs is None:
                    current_logs = ""
                
                lines = current_logs.split('\n') + [log_message]
                if len(lines) > 50:
                    lines = lines[-50:]

                dpg.set_value("Logs", '\n'.join(lines))
        except Exception as e:
            print(f"Error processing log messages: {e}")

    def simulation_loop(self):
        pass

    def run(self):
        self.log("Game Starting")
        if not self.initialize_environment():
            self.log("Failed to initialize environment.")
            return
        
        self.initialize_gui()

        # Start the simulation loop in a separate thread
        simulation_thread = threading.Thread(target=self.simulation_loop, daemon=True)
        simulation_thread.start()

        self.log("GUI Started successfully")

        while dpg.is_dearpygui_running():
            dpg.render_dearpygui_frame()
        
        dpg.destroy_context()



if __name__ == "__main__":
    gui = GUI()
    gui.run()

