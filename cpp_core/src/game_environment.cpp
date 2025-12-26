#include "../include/game_environment.h"
#include "../include/cell.h"
#include <iostream>
#include <random>
#include <queue>
#include <utility>
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
    // 先清空所有细胞
    cells_.clear();
    for (int i = 0; i < height_; i++)
    {
        for (int j = 0; j < width_; j++)
        {
            grid_[i][j] = false;
        }
    }
    // 随机放置细胞
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distX(0, width_ - 1);
    std::uniform_int_distribution<int> distY(0, height_ - 1);

    int cells_placed = 0;
    int attempts = 0;
    const int max_attempts = num_cells * 10000; // 每个细胞最多尝试10000次

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
    // 创建下一代状态记录
    std::vector<std::vector<bool>> nextState(height_, std::vector<bool>(width_, false));

    // 基于当前状态计算下一代（不立即修改）
    for (int i = 0; i < cells_.size(); i++)
    {
        if (cells_[i]->isAlive())
        {
            int x = cells_[i]->getPosition().x;
            int y = cells_[i]->getPosition().y;
            int liveNeighbors = 0;

            // 计算活邻居数量
            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    if (dx == 0 && dy == 0)
                        continue;

                    int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && nx < width_ && ny >= 0 && ny < height_)
                    {
                        if (grid_[ny][nx])
                            liveNeighbors++;
                    }
                }
            }

            // 应用康威规则决定下一代状态
            if (liveNeighbors >= Live_min && liveNeighbors <= Live_max)
            {
                nextState[y][x] = true; // 存活
            }
            else
            {
                nextState[y][x] = false; // 死亡
            }
        }
    }

    // 处理死细胞的繁殖
    for (int y = 0; y < height_; y++)
    {
        for (int x = 0; x < width_; x++)
        {
            if (!grid_[y][x])
            { // 当前是死细胞
                int liveNeighbors = 0;
                for (int dy = -1; dy <= 1; dy++)
                {
                    for (int dx = -1; dx <= 1; dx++)
                    {
                        if (dx == 0 && dy == 0)
                            continue;

                        int nx = x + dx, ny = y + dy;
                        if (nx >= 0 && nx < width_ && ny >= 0 && ny < height_)
                        {
                            if (grid_[ny][nx])
                                liveNeighbors++;
                        }
                    }
                }

                // 繁殖规则
                if (liveNeighbors >= Breed_min && liveNeighbors <= Breed_max)
                {
                    nextState[y][x] = true;
                }
            }
        }
    }

    // 同步更新所有细胞状态
    for (int y = 0; y < height_; y++)
    {
        for (int x = 0; x < width_; x++)
        {
            if (nextState[y][x] != grid_[y][x])
            {
                if (nextState[y][x])
                {
                    setCell(Position{x, y});
                }
                else
                {
                    removeCell(Position{x, y});
                }
            }
        }
    }

    // 能量和年龄更新逻辑
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (int i = 0; i < cells_.size(); i++)
    {
        if (cells_[i]->isAlive())
        {
            double prob = dist(gen);
            if (prob < Restore_prob)
            {
                double new_energy = cells_[i]->getEnergy() + Restore_value;
                cells_[i]->setEnergy(new_energy);
            }
            cells_[i]->increaseAge();
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

float GameEnvironment::newDensity() const
{
    int width = width_;
    int height = height_;

    // 如果没有活细胞，直接返回0
    if (getPopulation() == 0)
    {
        return 0.0f;
    }

    // 创建访问标记矩阵，记录哪些细胞已被分组
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    // 方向数组，定义8个邻居方向（包括对角线）
    int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    std::vector<float> groupDensities; // 存储每个细胞组的密度值

    // 遍历整个网格
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            // 如果当前细胞是活的且未被访问过，则开始一个新的组
            if (grid_[y][x] && !visited[y][x])
            {
                std::queue<std::pair<int, int>> q; // BFS队列
                q.push({x, y});
                visited[y][x] = true;

                // 初始化当前组的边界框
                int min_x = x, max_x = x, min_y = y, max_y = y;
                int cellCount = 0; // 当前组内的活细胞数量

                // BFS遍历当前组
                while (!q.empty())
                {
                    auto cell = q.front();
                    q.pop();
                    int cx = cell.first;
                    int cy = cell.second;
                    cellCount++;

                    // 更新边界框坐标
                    if (cx < min_x)
                        min_x = cx;
                    if (cx > max_x)
                        max_x = cx;
                    if (cy < min_y)
                        min_y = cy;
                    if (cy > max_y)
                        max_y = cy;

                    // 检查8个方向的邻居
                    for (int i = 0; i < 8; ++i)
                    {
                        int nx = cx + dx[i];
                        int ny = cy + dy[i];
                        // 确保邻居在网格范围内
                        if (nx >= 0 && nx < width && ny >= 0 && ny < height)
                        {
                            if (grid_[ny][nx] && !visited[ny][nx])
                            {
                                visited[ny][nx] = true;
                                q.push({nx, ny});
                            }
                        }
                    }
                }

                // 计算当前组的边界框面积
                int area = (max_x - min_x + 1) * (max_y - min_y + 1);
                if (area > 0)
                {
                    float density = static_cast<float>(cellCount) / area;
                    groupDensities.push_back(density);
                }
            }
        }
    }

    // 计算所有组密度的平均值
    if (groupDensities.empty())
    {
        return 0.0f;
    }

    float sum = 0.0f;
    for (float d : groupDensities)
    {
        sum += d;
    }
    return sum / groupDensities.size();
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
