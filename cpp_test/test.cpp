#include <iostream>
#include "../cpp_core/include/cell.h"
#include "../cpp_core/include/game_environment.h"
#include "../cpp_core/include/config_parser.h"
int main()
{
  // 创建游戏环境
  GameEnvironment env(10, 10, "../config.txt");
  // 打印配置
  env.printConfig();
  // 初始化随机细胞
  env.initializeRandom(35);
  /*// 打印初始细胞状态
  const auto &cells = env.getCells();
  for (const auto &cell : cells)
  {
    std::cout << "Cell ID: " << cell->getId() << ", Position: ("
              << cell->getPosition().x << ", " << cell->getPosition().y
              << "), Alive: " << (cell->isAlive() ? "Yes" : "No")
              << ", Age: " << cell->getAge()
              << ", Energy: " << cell->getEnergy() << std::endl;
  }*/
  // 打印二维数组
  auto grid = env.getGridState();
  std::cout << "Grid State:" << std::endl;
  for (const auto &row : grid)
  {
    for (bool cell : row)
    {
      std::cout << (cell ? "." : " ");
    }
    std::cout << std::endl;
  }
  // 更新游戏状态
  env.update();
  /*// 打印更新后的细胞状态
  std::cout << "After update:" << std::endl;
  for (const auto &cell : env.getCells())
  {
    std::cout << "Cell ID: " << cell->getId() << ", Position: ("
              << cell->getPosition().x << ", " << cell->getPosition().y
              << "), Alive: " << (cell->isAlive() ? "Yes" : "No")
              << ", Age: " << cell->getAge()
              << ", Energy: " << cell->getEnergy() << std::endl;
  }*/
  // 打印二维数组
  grid = env.getGridState();
  std::cout << "Grid State after update:" << std::endl;
  for (const auto &row : grid)
  {
    for (bool cell : row)
    {
      std::cout << (cell ? "." : " ");
    }
    std::cout << std::endl;
  }

  return 0;
}