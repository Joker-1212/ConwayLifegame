#ifndef GAME_ENVIRONMENT_H
#define GAME_ENVIRONMENT_H

#include "types.h"
#include "config_parser.h"
#include "cell.h"
#include <vector>
#include <memory>

/**
 * @file game_environment.h
 * @brief 游戏环境接口声明
 *
 * 仅保留在Python绑定中实际使用的游戏环境接口
 */

/**
 * @class GameEnvironment
 * @brief 游戏环境核心类
 *
 * 管理生命游戏的网格状态和细胞群体，仅暴露Python端需要的方法
 */
class GameEnvironment
{
private:
    int width_, height_;  ///< 网格尺寸
    ConfigParser config_; ///< 配置管理器

public:
    /**
     * @brief 构造函数
     * @param width 环境宽度
     * @param height 环境高度
     * @param config_file 配置文件路径
     */
    GameEnvironment(int width, int height, const std::string &config_file = "config.txt");

    // 以下方法在 PyBind11 绑定中被直接调用

    /**
     * @brief 随机初始化环境
     * @param num_cells 初始细胞数量
     */
    void initializeRandom(int num_cells);

    /**
     * @brief 更新游戏状态
     */
    void update();

    /**
     * @brief 带移动的更新
     * @param moves 细胞移动指令列表
     */
    void updateWithMoves(const std::vector<int> &moves);

    /**
     * @brief 获取细胞状态观察
     * @return 细胞状态向量
     */
    std::vector<std::vector<float>> getCellStates() const;

    /**
     * @brief 获取网格状态
     * @return 网格状态矩阵
     */
    std::vector<std::vector<bool>> getGridState() const;

    /**
     * @brief 获取空邻居位置
     * @param pos 中心位置
     * @param d 搜索距离
     * @return 空位置列表
     */
    std::vector<Position> getEmptyNeighbors(const Position &pos, int d = -1) const;

    /**
     * @brief 检查位置有效性
     * @param pos 要检查的位置
     * @return 如果位置有效返回 true，否则 false
     */
    bool isValidPosition(const Position &pos) const;

    /**
     * @brief 检查位置是否为空
     * @param pos 要检查的位置
     * @return 如果位置为空返回 true，否则 false
     */
    bool isPositionEmpty(const Position &pos) const;

    // Getter 方法 - 在 Python 绑定中使用

    /**
     * @brief 获取环境宽度
     * @return 环境宽度值
     */
    int getWidth() const { return width_; }

    /**
     * @brief 获取环境高度
     * @return 环境高度值
     */
    int getHeight() const { return height_; }

    /**
     * @brief 获取细胞数量
     * @return 当前细胞数量
     */
    int getPopulation() const;

    /**
     * @brief 获取细胞密度
     * @return 细胞密度值
     */
    double getDensity() const;

    // 配置管理方法 - 在 Python 绑定中使用

    /**
     * @brief 重新加载配置
     */
    void reloadConfig();

    /**
     * @brief 打印当前配置
     */
    void printConfig() const;

    /**
     * @brief 获取环境中所有细胞的列表
     * @return 包含所有细胞智能指针的常量引用
     *
     * 这个方法在PyBind11绑定中被get_cell_positions方法调用
     * 用于获取细胞的详细信息并返回给Python端
     */
    const std::vector<std::shared_ptr<Cell>> &getCells() const;
};

#endif // GAME_ENVIRONMENT_H
