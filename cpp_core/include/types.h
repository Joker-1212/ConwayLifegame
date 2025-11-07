#ifndef TYPES_H
#define TYPES_H

/**
 * @file types.h
 * @brief 基础类型定义文件
 *
 * 定义项目中使用的基本数据结构，为 C++/Python 接口提供统一的数据类型
 * Position 结构体在 PyBind11 绑定中被直接使用
 */

/**
 * @struct Position
 * @brief 二维位置坐标结构体
 *
 * 表示网格环境中的位置坐标，通过 PyBind11 暴露给 Python，
 * 在 Python 端用于细胞位置定位和移动计算
 */
struct Position
{
    int x; ///< X 坐标（水平方向）
    int y; ///< Y 坐标（垂直方向）

    /**
     * @brief 构造函数
     * @param x X 坐标，默认为 0
     * @param y Y 坐标，默认为 0
     */
    Position(int x_ = 0, int y_ = 0) : x(x_), y(y_) {}

    /**
     * @brief 位置相等比较运算符重载
     * @param other 要比较的另一个位置
     * @return 如果坐标相同返回 true，否则 false
     */
    bool operator==(const Position &other) const
    {
        return x == other.x && y == other.y;
    }
};

#endif // TYPES_H
