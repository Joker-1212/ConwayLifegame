#ifndef CELL_H
#define CELL_H

#include "types.h"

/**
 * @file cell.h
 * @brief 细胞类接口声明
 *
 * 定义细胞数据结构和基本接口，用于在 C++ 和 Python 间传递细胞信息
 */

/**
 * @class Cell
 * @brief 细胞数据容器类
 *
 * 存储细胞的基本状态信息，通过 getCells 方法暴露给 Python 端
 */
class Cell
{
private:
    int id_;            ///< 细胞唯一标识符
    Position position_; ///< 细胞位置坐标
    bool alive_;        ///< 细胞存活状态
    int age_;           ///< 细胞年龄
    double energy_;     ///< 细胞能量

public:
    /**
     * @brief 构造函数
     * @param id 细胞唯一ID
     * @param pos 初始位置
     */
    Cell(int id, Position pos);

    // Getter方法 - 提供对细胞属性的只读访问

    /**
     * @brief 获取细胞ID
     * @return 细胞唯一标识符
     */
    int getId() const { return id_; }

    /**
     * @brief 获取细胞位置
     * @return 细胞当前位置
     */
    Position getPosition() const { return position_; }

    /**
     * @brief 检查细胞是否存活
     * @return 存活状态（true=存活，false=死亡）
     */
    bool isAlive() const { return alive_; }

    /**
     * @brief 获取细胞年龄
     * @return 细胞年龄
     */
    int getAge() const { return age_; }

    /**
     * @brief 获取细胞能量
     * @return 细胞能量值
     */
    double getEnergy() const { return energy_; }

    // Setter方法 - 允许修改细胞状态

    /**
     * @brief 设置细胞位置
     * @param pos 新的位置坐标
     */
    void setPosition(const Position &pos) { position_ = pos; }

    /**
     * @brief 设置细胞存活状态
     * @param alive 新的存活状态
     */
    void setAlive(bool alive);

    /**
     * @brief 设置细胞能量
     * @param energy 新的能量值
     */
    void setEnergy(double energy);

    /**
     * @brief 增加细胞年龄
     */
    void increaseAge() { age_++; }

    /**
     * @brief 消耗细胞能量
     * @param amount 消耗的能量值
     */
    void consumeEnergy(double amount) { energy_ -= amount; }
};

#endif // CELL_H
