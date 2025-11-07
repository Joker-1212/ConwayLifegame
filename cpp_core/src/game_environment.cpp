#include "../include/game_environment.h"
#include "../include/cell.h"
#include <iostream>
/**
 * @file game_environment.cpp
 * @brief 游戏环境实现文件
 *
 * 实现游戏环境接口，仅保留 Python 绑定中使用的方法
 */
GameEnvironment::GameEnvironment(int width, int height, const std::string &config_file)
    : width_(width), height_(height), config_(config_file), grid_(height, std::vector<bool>(width, false))
{
}

const std::vector<std::shared_ptr<Cell>> &GameEnvironment::getCells() const
{
    // TODO:返回活细胞列表
    return std::vector<std::shared_ptr<Cell>>();
}

void GameEnvironment::initializeRandom(int num_cells)
{
    // TODO:在随机位置放置细胞
}

void GameEnvironment::update()
{
    // TODO:更新游戏状态
}

void GameEnvironment::updateWithMoves(const std::vector<int> &moves)
{
    // TODO:根据移动列表更新状态（移动列表和细胞列表中的细胞一一对应）
    // 0~3 上下左右
    // 4~7 左上右上左下右下
    // 8 不动
}

std::vector<std::vector<float>> GameEnvironment::getCellStates() const
{
    // TODO:将视野范围内的细胞状态打包成二维数组返回
    return std::vector<std::vector<float>>();
}

std::vector<std::vector<bool>> GameEnvironment::getGridState() const
{
    // TODO:获取网格状态

    return grid_;
}

std::vector<Position> GameEnvironment::getEmptyNeighbors(const Position &pos, int d) const
{
    // TODO:返回空邻居位置
    return std::vector<Position>();
}

bool GameEnvironment::isValidPosition(const Position &pos) const
{
    // TODO:检查位置有效性
    return false;
}

bool GameEnvironment::isPositionEmpty(const Position &pos) const
{
    // TODO:检查位置是否为空
    if (grid_[pos.y][pos.x] == false)
        return true;
    else
        return false;
}

int GameEnvironment::getPopulation() const
{
    // TODO:获取细胞数量
    int sum = 0;
    for (int i = 0; i < height_; i++)
    {
        for (int j = 0; j < width_; j++)
        {
            if (grid_[i][j])
            {
                sum++;
            }
        }
    }
    return sum;
}

float GameEnvironment::getDensity() const
{
    // TODO:计算细胞密度
    int population = getPopulation();
    int area = width_ * height_;
    float density = static_cast<float>(population) / area;
    return density;
}

void GameEnvironment::reloadConfig()
{
    // TODO:重新加载配置
}

void GameEnvironment::printConfig() const
{
    // TODO:打印配置信息
}
