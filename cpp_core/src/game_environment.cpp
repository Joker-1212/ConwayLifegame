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
    // 返回活细胞列表
    return cells_;
}

void GameEnvironment::initializeRandom(int num_cells)
{
    // TODO:在随机位置放置细胞
}

void GameEnvironment::update()
{
    // TODO:若所有细胞都静止，更新游戏状态,随机增加能量，增加年龄
    
}

void GameEnvironment::updateWithMoves(const std::vector<int> &moves)
{
    // TODO:根据移动列表更新状态（移动列表和细胞列表中的细胞一一对应）
    // 0~3 上下左右
    // 4~7 左上右上左下右下
    // 8 不动
    // 随move减少能量
}

std::vector<std::vector<float>> GameEnvironment::getCellStates() const
{
    // TODO:将视野范围内的细胞状态打包成二维数组返回
    return std::vector<std::vector<float>>();
}

std::vector<std::vector<bool>> GameEnvironment::getGridState() const
{
    // 返回网格状态

    return grid_;
}

std::vector<Position> GameEnvironment::getEmptyNeighbors(const Position &pos, int d) const
{
    // 返回空邻居位置
    std::vector<Position> emp;
    for (int i = -d; i <= d; i++)
    {
        for (int j = -d; j <= d; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            Position nPos{pos.x + i, pos.y + j};
            if (!grid_[pos.y + j][pos.x + i] && pos.x + i <= width_ && pos.x + i >= 0 && pos.y + j <= height_ && pos.y + j >= 0)
            {
                emp.push_back(nPos);
            }
        }
    }
    return emp;
}

bool GameEnvironment::isValidPosition(const Position &pos) const
{
    // 检查位置是否合法
    if (pos.x >= 0 && pos.x <= height_ && pos.y >= 0 && pos.y <= width_)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool GameEnvironment::isPositionEmpty(const Position &pos) const
{
    // 检查位置是否为空
    if (grid_[pos.y][pos.x] == false)
        return true;
    else
        return false;
}

int GameEnvironment::getPopulation() const
{
    // 获取细胞数量
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
    // 计算细胞密度
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
void GameEnvironment::setCell(Position pos)
{
    // 在指定位置放置细胞

    if (pos.x >= 0 && pos.x <= height_ && pos.y >= 0 && pos.y <= width_ && !grid_[pos.y][pos.x])
    {
        if (cells_.size() == 0)
        {
            cells_.emplace_back(std::make_shared<Cell>(0, pos));
        }
        else
        {
            long long maxId = cells_[0]->getId();
            for (int i = 0; i < cells_.size(); i++)
            {
                if (cells_[i]->getId() > maxId)
                {
                    maxId = cells_[i]->getId();
                }
            }
            cells_.emplace_back(std::make_shared<Cell>(maxId + 1, pos));
        }
        grid_[pos.y][pos.x] = true;
    }
}
void GameEnvironment::removeCell(Position pos)
{
    // FIXME: 细胞无法删除
    if (this->isValidPosition(pos) && this->grid_[pos.x][pos.y])
    {
        this->grid_[pos.x][pos.y] = 0;
        for (auto it = cells_.begin(); it != cells_.end(); ++it)
        {
            if ((*it)->getPosition() == pos)
            {
                cells_.erase(it);
                break;
            }
        }
    }
}
