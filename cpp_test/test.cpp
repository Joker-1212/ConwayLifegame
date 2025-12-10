#include <iostream>
#include "../cpp_core/include/cell.h"
#include "../cpp_core/include/game_environment.h"
#include "../cpp_core/include/config_parser.h"
int main()
{
  // 创建游戏环境
  // GameEnvironment env(10, 10, "../config.txt");
  // 打印配置
  // env.printConfig();
  // 设置配置值
  ConfigParser config("../config.txt");
  config.loadConfig();
  config.setInt("LIVE_MIN", 5);
  config.saveConfig();
  GameEnvironment env(10, 10, "../config.txt");
  env.printConfig();
  /*  // 初始化随机细胞
    env.initializeRandom(5);
    // 打印初始细胞状态
    const auto &cells = env.getCells();
    for (const auto &cell : cells)
    {
        std::cout << "Cell ID: " << cell->getId() << ", Position: ("
                  << cell->getPosition().x << ", " << cell->getPosition().y
                  << "), Alive: " << (cell->isAlive() ? "Yes" : "No")
                  << ", Age: " << cell->getAge()
                  << ", Energy: " << cell->getEnergy() << std::endl;
    }
    // 更新游戏状态
    env.update();
    // 打印更新后的细胞状态
    std::cout << "After update:" << std::endl;
    for (const auto &cell : env.getCells())
    {
        std::cout << "Cell ID: " << cell->getId() << ", Position: ("
                  << cell->getPosition().x << ", " << cell->getPosition().y
                  << "), Alive: " << (cell->isAlive() ? "Yes" : "No")
                  << ", Age: " << cell->getAge()
                  << ", Energy: " << cell->getEnergy() << std::endl;
    }*/

  return 0;
}