#include "../include/game_environment.h"
#include "../include/cell.h"
#include <iostream>
#include <random>
/**
 * @file game_environment.cpp
 * @brief 游戏环境实现文件
 *
 * 实现游戏环境接口，仅保留 Python 绑定中使用的方法
 */
GameEnvironment::GameEnvironment(int width, int height, const std::string &config_file)
    : width_(width), height_(height), config_(config_file), grid_(height, std::vector<bool>(width, false))
{
    // 加载配置
    config_.loadConfig();
    Live_max = config_.getInt("LIVE_MAX", 3);
    Live_min = config_.getInt("LIVE_MIN", 2);
    Breed_max = config_.getInt("BREED_MAX", 3);
    Breed_min = config_.getInt("BREED_MIN", 3);
    Vision = config_.getInt("VISION", 5);
    Death_Rate = config_.getDouble("DEATH_RATE", 0.1);
    Energy_consumption = config_.getDouble("ENERGY_CONSUMPTION", 0.1);
    Restore_prob = config_.getDouble("RESTORE_PROB", 0.1);
    Restore_value = config_.getDouble("RESTORE_VALUE", 0.2);
    // TODO:判断用户输入是否合理
}
const std::vector<std::shared_ptr<Cell>> &GameEnvironment::getCells() const
{
    // 返回活细胞列表
    return cells_;
}

void GameEnvironment::initializeRandom(int num_cells)
{
    if (num_cells <= 0)
        return;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distX(0, width_ - 1);
    std::uniform_int_distribution<int> distY(0, height_ - 1);

    int cells_placed = 0;
    int attempts = 0;
    const int max_attempts = num_cells * 10; // 每个细胞最多尝试10次

    while (cells_placed < num_cells && attempts < max_attempts)
    {
        attempts++;

        int x = distX(gen);
        int y = distY(gen);
        Position pos(x, y);

        // 双重检查：先验证位置有效性，再检查是否为空
        if (!isValidPosition(pos))
        {
            continue; // 位置无效，重试
        }

        if (!isPositionEmpty(pos))
        {
            continue; // 位置已被占用，重试
        }

        // 位置有效且为空，放置细胞
        setCell(pos);
        cells_placed++;
    }
}
void GameEnvironment::update()
{
    // 若所有细胞都静止，更新游戏状态，随机增加能量，增加年龄
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (int i = 0; i < cells_.size(); i++)
    {
        if (cells_[i]->isAlive())
        {
            // 随机增加能量

            double prob = dist(gen);
            if (prob < Restore_prob)
            {
                double new_energy = cells_[i]->getEnergy() + Restore_value;
                cells_[i]->setEnergy(new_energy);
            }
            // 增加年龄
            cells_[i]->increaseAge();
        }
    }
    // 对于有细胞的位置，根据邻居细胞数量决定存活与否
    for (int i = 0; i < cells_.size(); i++)
    {

        if (cells_[i]->getPosition().x == 0 && cells_[i]->getPosition().y == 0)
        {
            int s1 = grid_[1][0] + grid_[0][1] + grid_[1][1];
            if (s1 < Live_min || s1 > Live_max)
            {
                removeCell(cells_[i]->getPosition());
            }
        }
        else if (cells_[i]->getPosition().x == width_ - 1 && cells_[i]->getPosition().y == 0)
        {
            int s2 = grid_[0][width_ - 2] + grid_[1][width_ - 2] + grid_[1][width_ - 1];
            if (s2 < Live_min || s2 > Live_max)
            {
                removeCell(cells_[i]->getPosition());
            }
        }
        else if (cells_[i]->getPosition().x == 0 && cells_[i]->getPosition().y == height_ - 1)
        {
            int s3 = grid_[height_ - 2][0] + grid_[height_ - 2][1] + grid_[height_ - 1][1];
            if (s3 < Live_min || s3 > Live_max)
            {
                removeCell(cells_[i]->getPosition());
            }
        }
        else if (cells_[i]->getPosition().x == width_ - 1 && cells_[i]->getPosition().y == height_ - 1)
        {
            int s4 = grid_[height_ - 1][width_ - 1] + grid_[height_ - 2][width_ - 1] + grid_[height_ - 1][width_ - 2];
            if (s4 < Live_min || s4 > Live_max)
            {
                removeCell(cells_[i]->getPosition());
            }
        }
        else if (cells_[i]->getPosition().y == 0 && cells_[i]->getPosition().x > 0 && cells_[i]->getPosition().x < width_ - 1)
        {
            int s5 = grid_[0][cells_[i]->getPosition().x - 1] + grid_[0][cells_[i]->getPosition().x + 1] + grid_[1][cells_[i]->getPosition().x - 1] + grid_[1][cells_[i]->getPosition().x] + grid_[1][cells_[i]->getPosition().x + 1];
            if (s5 < Live_min || s5 > Live_max)
            {
                removeCell(cells_[i]->getPosition());
            }
        }
        else if (cells_[i]->getPosition().y == height_ - 1 && cells_[i]->getPosition().x > 0 && cells_[i]->getPosition().y < width_ - 1)
        {
            int s6 = grid_[height_ - 1][cells_[i]->getPosition().x - 1] + grid_[height_ - 1][cells_[i]->getPosition().x + 1] + grid_[height_ - 2][cells_[i]->getPosition().x - 1] + grid_[height_ - 2][cells_[i]->getPosition().x] + grid_[height_ - 2][cells_[i]->getPosition().x + 1];
            if (s6 < Live_min || s6 > Live_max)
            {
                removeCell(cells_[i]->getPosition());
            }
        }
        else if (cells_[i]->getPosition().x == 0 && cells_[i]->getPosition().y > 0 && cells_[i]->getPosition().y < height_ - 1)
        {
            int s7 = grid_[cells_[i]->getPosition().y - 1][0] + grid_[cells_[i]->getPosition().y - 1][1] + grid_[cells_[i]->getPosition().y][1] + grid_[cells_[i]->getPosition().y + 1][1] + grid_[cells_[i]->getPosition().y + 1][0];
            if (s7 < Live_min || s7 > Live_max)
            {
                removeCell(cells_[i]->getPosition());
            }
        }
        else if (cells_[i]->getPosition().x == width_ - 1 && cells_[i]->getPosition().y > 0 && cells_[i]->getPosition().y < height_ - 1)
        {
            int s8 = grid_[cells_[i]->getPosition().y - 1][width_ - 1] + grid_[cells_[i]->getPosition().y - 1][width_ - 2] + grid_[cells_[i]->getPosition().y][width_ - 1] + grid_[cells_[i]->getPosition().y + 1][width_ - 2] + grid_[cells_[i]->getPosition().y + 1][width_ - 1];
            if (s8 < Live_min || s8 > Live_max)
            {
                removeCell(cells_[i]->getPosition());
            }
        }
        else if (cells_[i]->getPosition().x > 0 && cells_[i]->getPosition().x < width_ - 1 && cells_[i]->getPosition().y > 0 && cells_[i]->getPosition().y < height_ - 1)
        {
            int s9 = grid_[cells_[i]->getPosition().y - 1][cells_[i]->getPosition().x - 1] + grid_[cells_[i]->getPosition().y - 1][cells_[i]->getPosition().x] + grid_[cells_[i]->getPosition().y - 1][cells_[i]->getPosition().x + 1] +
                     grid_[cells_[i]->getPosition().y][cells_[i]->getPosition().x - 1] + grid_[cells_[i]->getPosition().y][cells_[i]->getPosition().x + 1] +
                     grid_[cells_[i]->getPosition().y + 1][cells_[i]->getPosition().x - 1] + grid_[cells_[i]->getPosition().y + 1][cells_[i]->getPosition().x] + grid_[cells_[i]->getPosition().y + 1][cells_[i]->getPosition().x + 1];
            if (s9 < Live_min || s9 > Live_max)
            {
                removeCell(cells_[i]->getPosition());
            }
        }
    }
    // 对于空位，根据邻居数量决定是否繁殖新细胞
    for (int i = 0; i < height_; i++)
    {
        for (int j = 0; j < width_; j++)
        {
            if (!grid_[i][j])
            {
                int s = 0;
                for (int m = -1; m <= 1; m++)
                {
                    for (int n = -1; n <= 1; n++)
                    {
                        if (m == 0 && n == 0)
                        {
                            continue;
                        }
                        if (i + m >= 0 && i + m < height_ - 1 && j + n >= 0 && j + n < width_ - 1)
                        {
                            if (grid_[i + m][j + n])
                            {
                                s++;
                            }
                        }
                    }
                }
                if (s >= Breed_min && s <= Breed_max)
                {
                    setCell(Position{j, i});
                }
            }
        }
    }
}

void GameEnvironment::updateWithMoves(const std::vector<int> &moves)
{
    // 根据移动列表更新状态（移动列表和细胞列表中的细胞一一对应）
    //  0~3 上下左右
    //  4~7 左上右上左下右下
    //  8 不动
    //  随move减少能量
    //  同时移入能量高的存活
    for (int i = 0; i < moves.size(); i++)
    {
        Position pos = cells_[i]->getPosition();
        switch (moves[i])
        {
        case 0:
            // 上
            if (isValidPosition(Position{pos.x, pos.y - 1}) && isPositionEmpty(Position{pos.x, pos.y - 1}))
            {
                cells_[i]->setPosition(Position{pos.x, pos.y - 1});
                grid_[pos.y][pos.x] = false;
                grid_[pos.y - 1][pos.x] = true;
            }
            break;
        case 1:
            // 下
            if (isValidPosition(Position{pos.x, pos.y + 1}) && isPositionEmpty(Position{pos.x, pos.y + 1}))
            {
                cells_[i]->setPosition(Position{pos.x, pos.y + 1});
                grid_[pos.y][pos.x] = false;
                grid_[pos.y + 1][pos.x] = true;
            }
            break;
        case 2:
            // 左
            if (isValidPosition(Position{pos.x - 1, pos.y}) && isPositionEmpty(Position{pos.x - 1, pos.y}))
            {
                cells_[i]->setPosition(Position{pos.x - 1, pos.y});
                grid_[pos.y][pos.x] = false;
                grid_[pos.y][pos.x - 1] = true;
            }
            break;
        case 3:
            // 右
            if (isValidPosition(Position{pos.x + 1, pos.y}) && isPositionEmpty(Position{pos.x + 1, pos.y}))
            {
                cells_[i]->setPosition(Position{pos.x + 1, pos.y});
                grid_[pos.y][pos.x] = false;
                grid_[pos.y][pos.x + 1] = true;
            }
            break;
        case 4:
            // 左上
            if (isValidPosition(Position{pos.x - 1, pos.y - 1}) && isPositionEmpty(Position{pos.x - 1, pos.y - 1}))
            {
                cells_[i]->setPosition(Position{pos.x - 1, pos.y - 1});
                grid_[pos.y][pos.x] = false;
                grid_[pos.y - 1][pos.x - 1] = true;
            }
            break;
        case 5:
            // 右上
            if (isValidPosition(Position{pos.x + 1, pos.y - 1}) && isPositionEmpty(Position{pos.x + 1, pos.y - 1}))
            {
                cells_[i]->setPosition(Position{pos.x + 1, pos.y - 1});
                grid_[pos.y][pos.x] = false;
                grid_[pos.y - 1][pos.x + 1] = true;
            }
            break;
        case 6:
            // 左下
            if (isValidPosition(Position{pos.x - 1, pos.y + 1}) && isPositionEmpty(Position{pos.x - 1, pos.y + 1}))
            {
                cells_[i]->setPosition(Position{pos.x - 1, pos.y + 1});
                grid_[pos.y][pos.x] = false;
                grid_[pos.y + 1][pos.x - 1] = true;
            }
            break;
        case 7:
            // 右下
            if (isValidPosition(Position{pos.x + 1, pos.y + 1}) && isPositionEmpty(Position{pos.x + 1, pos.y + 1}))
            {
                cells_[i]->setPosition(Position{pos.x + 1, pos.y + 1});
                grid_[pos.y][pos.x] = false;
                grid_[pos.y + 1][pos.x + 1] = true;
            }
            break;
        case 8:
            // 不动
            break;
        default:
            break;
        }
        // 减少能量
        double new_energy = cells_[i]->getEnergy() - Energy_consumption;
        cells_[i]->setEnergy(new_energy);
    }
    // 处理同时移入
    std::vector<Position> occupied_positions;
    for (int i = 0; i < cells_.size(); i++)
    {
        Position pos = cells_[i]->getPosition();
        bool occupied = false;
        for (int j = 0; j < occupied_positions.size(); j++)
        {
            if (pos.x == occupied_positions[j].x && pos.y == occupied_positions[j].y)
            {
                occupied = true;
                break;
            }
        }
        if (!occupied)
        {
            occupied_positions.push_back(pos);
        }
        else
        {
            // 找到能量最高的细胞
            double max_energy = cells_[i]->getEnergy();
            int max_index = i;
            for (int k = 0; k < cells_.size(); k++)
            {
                if (k != i && cells_[k]->getPosition().x == pos.x && cells_[k]->getPosition().y == pos.y)
                {
                    if (cells_[k]->getEnergy() > max_energy)
                    {
                        max_energy = cells_[k]->getEnergy();
                        max_index = k;
                    }
                }
            }
            // 删除其他细胞
            for (int k = 0; k < cells_.size(); k++)
            {
                if (k != max_index && cells_[k]->getPosition().x == pos.x && cells_[k]->getPosition().y == pos.y)
                {
                    cells_.erase(cells_.begin() + k);
                    k--;
                }
            }
        }
    }
    // 更新游戏状态
    update();
}

std::vector<std::vector<float>> GameEnvironment::getCellStates() const
{
    // 将视野范围内的细胞存活状况打包成二维数组返回
    std::vector<std::vector<float>> states;
    for (int i = 0; i < cells_.size(); i++)
    {
        Position pos = cells_[i]->getPosition();
        std::vector<float> state;
        for (int dy = -Vision; dy <= Vision; dy++)
        {
            for (int dx = -Vision; dx <= Vision; dx++)
            {
                int nx = pos.x + dx;
                int ny = pos.y + dy;
                if (nx >= 0 && nx < width_ && ny >= 0 && ny < height_)
                {
                    state.push_back(grid_[ny][nx] ? 1.0f : 0.0f);
                }
                else
                {
                    state.push_back(0.0f); // 边界外视为无细胞
                }
            }
        }
        states.push_back(state);
    }
    return states;
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
    if (pos.x >= 0 && pos.x < height_ && pos.y >= 0 && pos.y < width_)
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
    // 打印配置信息
    std::cout << "Live_min: " << Live_min << std::endl;
    std::cout << "Live_max: " << Live_max << std::endl;
    std::cout << "Breed_min: " << Breed_min << std::endl;
    std::cout << "Breed_max: " << Breed_max << std::endl;
    std::cout << "Vision: " << Vision << std::endl;
    std::cout << "Death_Rate: " << Death_Rate << std::endl;
    std::cout << "Energy_consumption: " << Energy_consumption << std::endl;
    std::cout << "Restore_prob: " << Restore_prob << std::endl;
    std::cout << "Restore_value: " << Restore_value << std::endl;
}
void GameEnvironment::setCell(Position pos)
{
    // 在指定位置放置细胞

    if (pos.x >= 0 && pos.x < height_ && pos.y >= 0 && pos.y < width_ && !grid_[pos.y][pos.x])
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
    // 移除指定位置的细胞
    for (int i = 0; i < cells_.size(); i++)
    {
        if (cells_[i]->getPosition().x == pos.x && cells_[i]->getPosition().y == pos.y)
        {
            cells_.erase(cells_.begin() + i);
            grid_[pos.y][pos.x] = false;
            break;
        }
    }
}
