import dearpygui.dearpygui as dpg
from agents.cell_agent import CellAgent
from models.policy_network import DQNetwork
from environment.game_env import SmartGameEnv
import datetime
import train
import queue
import threading
import os
import sys
import subprocess
from config import Config

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
        self.cell_size = 8

        self.log_queue = queue.Queue()
        self.gui_ready = False
        self.debug_mode = True

        self.configs = Config()
        

    def initialize_environment(self):
        """
        初始化游戏环境和细胞 Agent
        """
        try:
            # 初始化游戏环境
            self.log("Initializing environment...")
            self.env = SmartGameEnv(
                width=self.configs["ENV_WIDTH"],
                height=self.configs["ENV_HEIGHT"],
                config_file="./config.txt"
            )
            self.env.reset(0)
            self.log(f"Environment initiallized with width: {self.configs['ENV_WIDTH']}, height: {self.configs['ENV_HEIGHT']}")

            # 初始化细胞 Agent
            state = self.env.state_size
            action = self.env.action_size
            model = DQNetwork(state, action, self.configs["HIDDEN_SIZE"])
            self.agent = CellAgent(model, state, action)
            self.log("Cell Agents initiallized")
            return True
        except Exception as e:
            self.log(f"Error initializing environment: {e}", level="ERROR")
            return False

    def initialize_gui(self):
        """
        初始化 GUI 环境
        """
        dpg.create_context()

        with dpg.font_registry():
            dpg.bind_font(dpg.add_font("./Font/Genshin.ttf", 16))

        with dpg.window(tag="Main"):
            # 控制面板
            with dpg.child_window(width=350):
                dpg.add_text("Smart Game of Life")
                dpg.add_separator()

                with dpg.group(horizontal=True):
                    dpg.add_button(label="Start", callback=self.start_simulation)
                    dpg.add_button(label="Pause", callback=self.pause_simulation)
                    dpg.add_button(label="Reset", callback=self.reset_simulation)
                    dpg.add_button(label="Step", callback=self.step_simulation)
                dpg.add_separator()

                # Debug controls
                with dpg.collapsing_header(label="Debug Controls", default_open=True):
                    dpg.add_checkbox(
                        label="Debug Mode",
                        default_value=self.debug_mode,
                        callback=lambda s, d: setattr(self, "debug_mode", d)
                    )
                    dpg.add_checkbox(
                        label="Edit Mode",
                        default_value=self.edit_mode,
                        callback=lambda s, d: setattr(self, "edit_mode", d)
                    )
                    dpg.add_checkbox(
                        label="Show Grid Line",
                        default_value=self.show_grid_line,
                        callback=self.toggle_grid_line
                    )
                    dpg.add_button(label="Export State", callback=self.export_state)
                    dpg.add_button(label="Force GC", callback=self.force_garbage_collection)
                    dpg.add_button(label="Clear Log", callback=self.clear_log)
                    dpg.add_button(label="Clear Grid", callback=self.clear_grid)
                
                # Simulation Control
                with dpg.collapsing_header(label="Simulation Control", default_open=True):
                    dpg.add_slider_float(
                        label="Update Interval",
                        default_value=self.update_interval,
                        min_value=0.01,
                        max_value=1.0,
                        callback=self.change_update_interval
                    )
                    dpg.add_slider_int(
                        label="Cell Size",
                        default_value=self.cell_size
                    )
                
                # Training Control
                with dpg.collapsing_header(label="Training Control", default_open=False):
                    dpg.add_button(label="Start Training", callback=self.start_training)
                    dpg.add_button(label="Stop Training", callback=self.stop_training)
                    dpg.add_slider_float(
                        label="Epsilon",
                        default_value=0.1,
                        min_value=0.0,
                        max_value=1.0,
                        tag="epsilon_slider"
                    )
                
                    # Model Loading
                    dpg.add_separator()
                    dpg.add_text("Model Loading")
                    dpg.add_button(label="Load Model", callback=self.load_model)

                # Rule Setting
                with dpg.collapsing_header(label="Rule Setting", default_open=False):
                    dpg.add_text("Game Rules")
                    dpg.add_input_int(label="Survive Min", default_value=2, tag="rule_survive_min")
                    dpg.add_input_int(label="Survive Max", default_value=3, tag="rule_survive_max")
                    dpg.add_input_int(label="Breed Min", default_value=3, tag="rule_breed_min")
                    dpg.add_input_int(label="Breed Max", default_value=3, tag="rule_breed_max")
                    dpg.add_input_float(label="Death Rate", default_value=0.1, tag="rule_death_rate")
                    # TODO: 用 Config 配置默认参数
                    dpg.add_input_float(label="Energy Consumption", default_value=0.1, tag="rule_energy_consumption")
                    dpg.add_input_float(label="Energy Gain", default_value=0.2, tag="rule_energy_gain")
                    dpg.add_input_float(label="Energy Gain Probability", default_value=0.1, tag="rule_energy_gain_prob")
                    # ENDTODO: 用 Config 配置默认参数
                    dpg.add_button(label="Apply Rules", callback=self.apply_rules)
                    dpg.add_button(label="Reload Rules", callback=self.reload_rules)
                
                # Trining Function

                with dpg.collapsing_header(label="Auto Training"):
                    dpg.add_text("One-Click Training")
                    dpg.add_button(label="Start Auto Training", callback=self.auto_training)
                    dpg.add_progress_bar(label="Training Progress", default_value=0.0, tag="training progress")
                    dpg.add_text("Status: Ready", tag="training_status")

                # Statistics Display
                with dpg.collapsing_header(label="Statistics", default_open=True):
                    dpg.add_text("Population: 0", tag="stat_population")
                    dpg.add_text("Density: 0.000", tag="stat_density")
                    dpg.add_text("Episode: 0", tag="stat_episode")
                    dpg.add_text("Reward: 0.000", tag="stat_reward")
                    dpg.add_text("Steps: 0", tag="stat_steps")
                    dpg.add_text("Steps/Sec: 0.0", tag="stat_steps_per_sec")
                    dpg.add_text("Avg Reward: 0.000", tag="stat_avg_reward")
                    dpg.add_text("Training Loss: 0.000", tag="stat_loss")
                
                # Configuration
                with dpg.collapsing_header(label="Configuration"):
                    dpg.add_input_int(label="Grid Width", default_value=self.configs["ENV_WIDTH"], tag="config_width")
                    dpg.add_input_int(label="Grid Height", default_value=self.configs["ENV_HEIGHT"], tag="config_height")
                    dpg.add_input_float(label="Initial Density", default_value=self.configs["INITIAL_CELLS_POTION"], tag="config_density")
                    dpg.add_button(label="Apply Config", callback=self.apply_config)
            
            # 游戏网格
            with dpg.child_window():
                # Grid Displayer
                with dpg.tab_bar():
                    with dpg.tab(label="Grid Visualization"):
                        dpg.add_text("Game Grid - Click to toggle cells(Edit mode should be enabled)")
                        dpg.add_separator()

                        with dpg.drawlist(
                            width=self.configs["ENV_WIDTH"] * self.cell_size,
                            height=self.configs["ENV_HEIGHT"] * self.cell_size,
                            tag="grid_drawlist"
                        ):
                            pass
                        
                        with dpg.item_handler_registry(tag="grid_click_handler"):
                            dpg.add_item_clicked_handler(callback=self.grid_click_callback)
                        dpg.bind_item_handler_registry("grid_drawlist", "grid_click_handler")
                    
                    # Debug Info
                    with dpg.tab(label="Debug Infomation"):
                        dpg.add_text("Debug Logs")
                        dpg.add_input_text(
                            multiline=True,
                            readonly=True,
                            height=300,
                            width=-1,
                            tag="debug_log"
                        )

                        dpg.add_separator()
                        dpg.add_text("Environment State")

        self.gui_ready = True

        dpg.create_viewport(title="Smart Game of Life", width=1400, height=900)
        dpg.setup_dearpygui()
        dpg.set_primary_window("Main", True)
        dpg.show_viewport()

    def apply_config(self):
        pass

    def auto_training(self):
        pass

    def grid_click_callback(self):
        pass

    def apply_rules(self):
        pass

    def reload_rules(self):
        pass

    def load_model(self):
        pass

    def start_training(self):
        pass

    def stop_training(self):
        pass

    def change_update_interval(self):
        pass

    def start_simulation(self):
        pass

    def export_state(self):
        pass

    def pause_simulation(self):
        pass

    def step_simulation(self):
        pass

    def reset_simulation(self):
        pass

    def toggle_grid_line(self):
        pass

    def force_garbage_collection(self):
        pass

    def clear_log(self):
        pass

    def clear_grid(self):
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
        """
        启动游戏
        """
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
            self.process_log()
            dpg.render_dearpygui_frame()
        
        dpg.destroy_context()



if __name__ == "__main__":
    gui = GUI()
    gui.run()

