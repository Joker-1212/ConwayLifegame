import os
import sys
import numpy as np
from Module.Configs.config import Config

try:
    import smart_life_core
    print("Successfully imported smart_life_core")
except ImportError as e:
    print(f"Failed to import smart_life_core: {e}")
    sys.exit(1)

def test_basic_functionality():
    """测试基本功能"""
    print("\nTesting basic functionality...")
    
    try:
        # 创建环境
        env = smart_life_core.GameEnvironment(20, 20)
        env.initialize_random(30)
        
        print(f"Initial population: {env.get_population()}")
        print(f"Grid size: {env.get_width()}x{env.get_height()}")
        
        # 更新几步
        for i in range(5):
            env.update()
            pop = env.get_population()
            density = env.get_density()
            print(f"Step {i+1}: Population={pop}, Density={density:.3f}")
        
        # 测试配置
        config = Config()
        config.load_from_file()
        vision = config["VISION"]
        print(f"Vision from config: {vision}")
        
        print("Basic functionality test passed")
        return True
        
    except Exception as e:
        print(f"Basic functionality test failed: {e}")
        return False

def test_numpy_integration():
    """测试 NumPy 集成"""
    print("\nTesting NumPy integration...")
    
    try:
        env = smart_life_core.GameEnvironment(10, 10)
        env.initialize_random(10)
        
        # 获取细胞状态
        states = env.get_cell_states()
        print(f"Cell states shape: {states.shape}")
        print(f"Cell states dtype: {states.dtype}")
        
        # 获取网格状态
        grid = env.get_grid_state()
        print(f"Grid shape: {grid.shape}")
        print(f"Grid dtype: {grid.dtype}")
        
        print("NumPy integration test passed")
        return True
        
    except Exception as e:
        print(f"NumPy integration test failed: {e}")
        return False

if __name__ == "__main__":
    print("Windows Build Test")
    print("=" * 50)
    
    success = True
    success &= test_basic_functionality()
    success &= test_numpy_integration()
    
    print("\n" + "=" * 50)
    if success:
        print("All tests passed! Build is successful.")
    else:
        print("Some tests failed. Please check the build.")
        sys.exit(1)
