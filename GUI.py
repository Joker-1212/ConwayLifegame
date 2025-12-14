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
import time
import json
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
        self.last_update_time = time.time()
        self.frame_count = 0
        self.stats = {
            'population': 0,
            'density': 0.0,
            'episode': 0,
            'reward': 0.0,
            'steps_per_sec': 0,
            'average_reward': 0.0,
            'total_steps': 0,
            'training_loss': 0.0
        }

    def initialize_environment(self):
        """
        初始化游戏环境和细胞 Agent
        """
        try:
        # 初始化游戏环境
            self.log("Initializing environment...")
            self.configs.load_from_file()
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
            with dpg.group(horizontal=True):
            # 控制面板
                with dpg.child_window(width=400):
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
                            default_value=self.cell_size,
                            min_value=2,
                            max_value=20,
                            callback=self.change_cell_size
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
                        dpg.add_input_int(label="Survive Min", default_value=self.configs["LIVE_MIN"], tag="rule_live_min")
                        dpg.add_input_int(label="Survive Max", default_value=self.configs["LIVE_MAX"], tag="rule_live_max")
                        dpg.add_input_int(label="Breed Min", default_value=self.configs["BREED_MIN"], tag="rule_breed_min")
                        dpg.add_input_int(label="Breed Max", default_value=self.configs["BREED_MAX"], tag="rule_breed_max")
                        dpg.add_input_int(label="Vision Distance", default_value=self.configs["VISION"], tag="rule_vision")
                        dpg.add_input_float(label="Death Rate", default_value=self.configs["DEATH_RATE"], tag="rule_death_rate")
                        dpg.add_input_float(label="Energy Consume", default_value=self.configs["ENERGY_CONSUMPTION"], tag="rule_energy_consumption")
                        dpg.add_input_float(label="Energy Gain", default_value=self.configs['RESTORE_VALUE'], tag="rule_restore_value")
                        dpg.add_input_float(label="Energy Gain\nProbability", default_value=self.configs['RESTORE_PROB'], tag="rule_restore_prob")
                        dpg.add_button(label="Apply Rules", callback=self.apply_rules)
                        dpg.add_button(label="Reload Rules", callback=self.reload_rules)
                    
                    # Training Function
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
                        with dpg.tab(label="Grid"):
                            dpg.add_text("Game Grid - Click to toggle cells(Edit mode should be enabled)")
                            dpg.add_separator()

                            with dpg.drawlist(
                                width=self.configs["ENV_WIDTH"] * self.cell_size,
                                height=self.configs["ENV_HEIGHT"] * self.cell_size,
                                tag="grid_drawlist"
                            ):
                                # 
                                self.draw_grid()
                            
                            with dpg.item_handler_registry(tag="grid_click_handler"):
                                dpg.add_item_clicked_handler(callback=self.grid_click_callback)
                            dpg.bind_item_handler_registry("grid_drawlist", "grid_click_handler")
                        
                        # Logs
                        with dpg.tab(label="Logs"):
                            dpg.add_text("Debug Logs")
                            dpg.add_input_text(
                                multiline=True,
                                readonly=True,
                                height=300,
                                width=-1,
                                tag="Logs"
                            )

                            dpg.add_separator()
                            dpg.add_text("Environment State")

        self.gui_ready = True

        dpg.create_viewport(title="Smart Game of Life", width=1400, height=900)
        dpg.setup_dearpygui()
        dpg.set_primary_window("Main", True)
        dpg.show_viewport()

    def change_cell_size(self, sender, app_data):
        """
        Cell Size 滑动条的回调函数
        """
        self.cell_size = app_data
        if dpg.does_alias_exist("grid_drawlist"):
            dpg.configure_item("grid_drawlist",
                               width=self.configs['ENV_WIDTH'] * self.cell_size,
                               height=self.configs['ENV_HEIGHT'] * self.cell_size)

        self.draw_grid()
        self.log(f"Cell size changed to {app_data}")

    def apply_config(self):
        """
        应用新的环境配置
        """
        try:
            new_width = dpg.get_value("config_width")
            new_height = dpg.get_value("config_height")
            new_density = dpg.get_value("config_density")

            if new_width <= 0 or new_height <= 0:
                self.log("Invalid grid dimensions", "ERROR")
                return

            self.configs["ENV_WIDTH"] = new_width
            self.configs["ENV_HEIGHT"] = new_height
            self.configs["INITIAL_CELLS_POTION"] = new_density

            # 重新初始化环境
            self.initialize_environment()

            # 更新GUI显示
            dpg.configure_item("grid_drawlist",
                               width=new_width * self.cell_size,
                               height=new_height * self.cell_size)
            self.draw_grid()
            self.update_statistics()
            self.log(f"Configuration applied: {new_width}x{new_height}, density={new_density:.2f}")
        except Exception as e:
            self.log(f"Error applying config: {e}", "ERROR")

    def auto_training(self):
        pass

    def grid_click_callback(self, sender):
        """
        网格点击事件的 Handler
        """

        if not self.edit_mode:
            self.log("Edit mode is disabled. Enable edit mode to toggle cell state.")
            return
        
        if self.is_running:
            self.log("Edit is prohibited while simulating. Please pause the simulation first", "WARNING")
            return
        
        try:
            # 获取鼠标位置
            mouse_pos = dpg.get_mouse_pos()
            if not dpg.does_alias_exist("grid_background"):
                self.log("Grid draw area not found", "ERROR")
            item_pos = dpg.get_item_pos("grid_background")
            item_size = dpg.get_item_rect_size("grid_drawlist")

            # 计算网格坐标
            if (mouse_pos[0] < item_pos[0] or mouse_pos[1] < item_pos[1] or
                mouse_pos[0] > item_pos[0] + item_size[0] + 65 or
                mouse_pos[1] > item_pos[1] + item_size[1] + 10):
                # 此时点击区域位于图像外
                return
            
            rx = int((mouse_pos[0] - item_pos[0] - 10) / self.cell_size)
            ry = int((mouse_pos[1] - item_pos[1] - 65) / self.cell_size)
            #NOTICE: 这里的 -10 和 -65 是经验值，具体问题尚未查明
            if self.env.is_position_empty(rx, ry):
                self.env.set_cell(rx, ry)
                self.log(f"Cell added at ({rx}, {ry})")
            else:
                self.env.remove_cell(rx, ry)
                self.log(f"Cell removed at ({rx}, {ry})")
            self.draw_grid()
        except Exception as e:
            self.log(f"Err handling grid click: {e}", "ERROR")

    def apply_rules(self):
        """应用新的游戏规则"""
        try:
            # 获取新的规则值
            live_min = dpg.get_value("rule_live_min")
            live_max = dpg.get_value("rule_live_max")
            breed_min = dpg.get_value("rule_breed_min")
            breed_max = dpg.get_value("rule_breed_max")
            vision = dpg.get_value("rule_vision")
            death_rate = dpg.get_value("rule_death_rate")
            energy_consumption = dpg.get_value("rule_energy_consumption")
            restore_prob = dpg.get_value("rule_restore_prob")
            restore_value = dpg.get_value("rule_restore_value")
            env_width = dpg.get_value("config_width")
            env_height = dpg.get_value("config_height")

            if (live_min < 0 or live_max < live_min or live_max > 8):
                self.log("Fail applying rules", "ERROR")
                return
            if (breed_max < breed_min or breed_min < 0 or breed_max > 8):
                return
            if (vision < 1 or vision > 8):
                return
            if (death_rate < 0.0 or death_rate >= 1.0):
                return
            if (energy_consumption < 0.0):
                self.log("Fail applying rules", "ERROR")
                return
            if (restore_prob < 0.0 or restore_prob > 1.0 or restore_value < 0.0):
                self.log("Fail applying rules", "ERROR")
                return
            if (env_height <= 0 or env_width <= 0 or env_height > 200 or env_width > 200):
                self.log("Fail applying rules", "ERROR")
                return
            # 更新配置文件
            config_content = f"""# Smart Game of Life Configuration
# Minimum number of neighbors for a cell to survive
LIVE_MIN = {live_min}

# Maximum number of neighbors for a cell to survive  
LIVE_MAX = {live_max}

# Minimum number of neighbors for a cell to be born
BREED_MIN = {breed_min}

# Maximum number of neighbors for a cell to be born
BREED_MAX = {breed_max}

# Cell vision distance
VISION = {vision}

# Cell death probability
DEATH_RATE = {death_rate:.1f}

# Cell energy consume
ENERGY_CONSUMPTION = {energy_consumption:.1f}

# Cell energy restore rate
RESTORE_PROB = {restore_prob:.1f}
RESTORE_VALUE = {restore_value:.1f}

# Grid size
ENV_WIDTH = {env_width}
ENV_HEIGHT = {env_height}
"""
            
            with open(self.configs['CONFIG_FILE'], 'w') as f:
                f.write(config_content)
            
            self.log("Rules updated and saved to config.txt")
            
            # 重新加载环境以应用新规则
            self.initialize_environment()
            self.draw_grid()
            self.update_statistics()
            
            self.log("Environment reloaded with new rules")
            
        except Exception as e:
            self.log(f"Error applying rules: {e}", "ERROR")

    def reload_rules(self):
        """
        重新加载规则配置文件
        """
        try:
            self.initialize_environment()
            self.draw_grid()
            self.update_statistics()
            dpg.set_value("rule_live_min", self.configs["LIVE_MIN"])
            dpg.set_value("rule_live_max", self.configs["LIVE_MAX"])
            dpg.set_value("rule_breed_min", self.configs["BREED_MIN"])
            dpg.set_value("rule_breed_max", self.configs["BREED_MAX"])
            dpg.set_value("rule_vision", self.configs["VISION"])
            dpg.set_value("rule_death_rate", self.configs["DEATH_RATE"])
            dpg.set_value("rule_energy_consumption", self.configs["ENERGY_CONSUMPTION"])
            dpg.set_value("rule_restore_prob", self.configs["RESTORE_PROB"])
            dpg.set_value("rule_restore_value", self.configs["RESTORE_VALUE"])

            self.log("Rules reloaded from config.txt")
        except Exception as e:
            self.log(f"Error reloading rules: {e}", "ERROR")

    def load_model(self):
        pass

    def start_training(self):
        pass

    def stop_training(self):
        pass

    def change_update_interval(self, sender, app_data):
        """
        Update Interval 滑动条的回调函数
        """
        self.update_interval = app_data
        self.log(f"Update interval changed to {app_data:.2f} seconds")

    def start_simulation(self):
        self.is_running = True
        self.log("Simulation started")

    def export_state(self):
        """将当前网格状态导出到 JSON 文件"""
        try:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"simulation_state_{timestamp}.json"
            
            state_data = {
                "timestamp": timestamp,
                "environment": {
                    "width": Config.ENV_WIDTH,
                    "height": Config.ENV_HEIGHT,
                    "population": self.stats['population'],
                    "density": self.stats['density']
                },
                "simulation": {
                    "step_count": self.step_count,
                    "episode": self.stats['episode'],
                    "total_reward": self.stats['reward'],
                    "average_reward": self.stats['average_reward']
                },
                "cell_positions": list(self.cell_positions),
                "settings": {
                    "update_interval": self.update_interval,
                    "cell_size": self.cell_size,
                    "is_running": self.is_running,
                    "is_training": self.is_training,
                    "debug_mode": self.debug_mode,
                    "edit_mode": self.edit_mode,
                    "show_grid_lines": self.show_grid_lines
                }
            }
            
            with open(filename, 'w') as f:
                json.dump(state_data, f, indent=2)
                
            self.log_message(f"Simulation state exported to {filename}")
            
        except Exception as e:
            self.log_message(f"Error exporting state: {e}", "ERROR")

    def pause_simulation(self):
        self.is_running = False
        self.log("Simulation paused")

    def step_simulation(self):
        """单步更新"""
        if self.env is not None:
            try:
                state = self.env._get_observation()
                if state.size > 0:
                    epsilon = dpg.get_value("epsilon_slider") if self.is_training else 0.0
                    actions = self.agent.act(state, epsilon=epsilon)
                else:
                    actions = None

                next_state, reward, done, info = self.env.step(actions)
                self.stats['reward'] += reward
                self.step_count += 1

                if done:
                    self.stats['episode'] += 1

                self.draw_grid()
                self.update_statistics()
                self.log(f"Simulation stepped. Total steps: {self.step_count}")
            except Exception as e:
                self.log(f"Error stepping simulation: {e}", "ERROR")

    def reset_simulation(self):
        """Reset the simulation"""
        if self.env is not None:
            try:
                self.env.reset(0)
                self.step_count = 0
                self.stats['reward'] = 0.0
                self.stats['average_reward'] = 0.0
                self.stats['episode'] = 0
                self.draw_grid()
                self.update_statistics()
                self.log("Simulation reset")
            except Exception as e:
                self.log(f"Error resetting simulation: {e}", "ERROR")

    def toggle_grid_line(self, sender, app_data):
        """
        Toggle Grid Line Checkbox 的回调函数
        """
        self.show_grid_line = app_data
        self.draw_grid()
        self.log(f"Grid lines {'showed if app_data else hidden'}")

    def force_garbage_collection(self):
        """
        强制垃圾回收，释放内存
        """
        import gc
        gc.collect()
        self.log("Garbage collection forced")

    def clear_log(self):
        """
        清理调试日志
        """
        try:
            dpg.set_value("Logs", "")
            self.log("Logs cleared")
        except Exception as e:
            self.log(f"Error clearing logs: {e}", "ERROR")

    def clear_grid(self):
        """清除网格中的所有细胞"""
        if self.env is not None:
            try:
                self.env.reset(0)
                self.step_count = 0
                self.draw_grid()
                self.update_statistics()
                self.log("Grid cleared")
            except Exception as e:
                self.log(f"Error clearing grid: {e}", "ERROR")
    
    def update_statistics(self):
        """更新统计信息显示"""
        try:
            current_time = time.time()
            elapsed = current_time - self.last_update_time
            self.frame_count += 1

            # Update FPS every second
            if elapsed >= 1.0:
                self.stats['step_per_sec'] = self.frame_count / elapsed
                self.frame_count = 0
                self.last_update_time = current_time
            
            self.stats['population'] = self.env.get_population()
            self.stats['density'] = self.stats['population'] / (self.configs['ENV_HEIGHT'] * self.configs['ENV_WIDTH'])

            dpg.set_value("stat_population", f"Population: {self.stats['population']}")
            dpg.set_value("stat_density", f"Density: {self.stats['density']:.3f}")
            dpg.set_value("stat_episode", f"Episode: {self.stats['episode']}")
            dpg.set_value("stat_reward", f"Reward: {self.stats['reward']:.3f}")
            dpg.set_value("stat_steps", f"Steps: {self.stats['total_steps']}")
            dpg.set_value("stat_steps_per_sec", f"Steps/Sec: {self.stats['steps_per_sec']:.1f}")
            dpg.set_value("stat_avg_reward", f"Avg Reward: {self.stats['average_reward']:.3f}")
            dpg.set_value("stat_loss", f"Training Loss: {self.stats['training_loss']:.3f}")
        except Exception as e:
            self.log(f"Error updating statistics: {e}", "ERROR")

    def draw_grid(self):
        """
        细胞与网格线绘制函数
        """
        # self.log("Draw function called", "DEBUG")
        if not dpg.does_alias_exist("grid_drawlist"):
            return
        
        # 清空背景
        dpg.delete_item("grid_drawlist", children_only=True)

        # 画背景
        dpg.draw_rectangle(
            (0, 0),
            (self.configs["ENV_WIDTH"] * self.cell_size, self.configs["ENV_HEIGHT"] * self.cell_size),
            color=(0, 0, 0, 255),
            fill=(0, 0, 0, 255),
            parent="grid_drawlist",
            tag="grid_background"
        )

        # 画细胞
        for cell in self.env.get_cells():
            dpg.draw_rectangle(
                [cell["x"] * self.cell_size, cell["y"] * self.cell_size],
                [(cell['x'] + 1) * self.cell_size, (cell['y'] + 1) * self.cell_size],
                color=(0, 0, 255, 255),
                fill=(0, 0, 255, 255),
                parent="grid_drawlist"
            )

        # 画网格线
        if self.show_grid_line:
            grid_color = (255, 255, 255, 50)

            # Verticals
            for x in range(self.configs["ENV_WIDTH"] + 1):
                x_pos = x * self.cell_size
                dpg.draw_line(
                    (x_pos, 0),
                    (x_pos, self.configs['ENV_HEIGHT'] * self.cell_size),
                    color=grid_color,
                    thickness=1,
                    parent="grid_drawlist"
                )
            
            # Horizentals
            for y in range(self.configs['ENV_HEIGHT'] + 1):
                y_pos = y * self.cell_size
                dpg.draw_line(
                    (0, y_pos),
                    (self.configs['ENV_WIDTH'] * self.cell_size, y_pos),
                    color=grid_color,
                    thickness=1,
                    parent='grid_drawlist'
                )

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
        """
        模拟主循环
        """
        while True:
            if self.is_running and self.env is not None:
                try:
                    state = self.env._get_observation()
                    if state.size > 0:
                        epsilon = dpg.get_value("epsilon_slider") if self.is_training else 0.0
                        actions = self.agent.act(state, epsilon=epsilon)
                        self.log_message(f"Agent selected {len(actions)} actions with epsilon={epsilon:.2f}", "DEBUG")
                    else:
                        actions = None
                        self.log_message("No cells alive, no action takes", "DEBUG")
                    next_state, reward, done, info = self.env.step(actions)
                    self.stats['reward'] += reward
                    self.step_count += 1

                    self.stats['average_reward'] = self.stats['average_reward'] * 0.95 + reward * 0.05

                    if done:
                        self.stats['episode'] += 1
                        self.log_message(f"Episode {self.stats['episode']} finished after {self.step_count} steps with total reward {self.stats['reward']:.2f}")
                        self.draw_grid()
                        self.update_statistics()
                except Exception as e:
                    self.log(f"Error while simulating: {e}", "ERROR")
                    self.is_running = False

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

