#include "../include/cell.h"

/**
 * @file cell.cpp
 * @brief 细胞类实现
 */
Cell::Cell(int id, Position pos)
    : id_(id), position_(pos), alive_(true), age_(0), energy_(1.0)
{
    // 细胞初始化为存活状态，年龄为0，能量为1.0
}
void Cell::setAlive(bool alive)
{
    alive_ = alive;
    if (!alive_)
    {
        energy_ = 0.0;                // 死亡细胞能量归零
        position_ = Position(-1, -1); // 死亡细胞位置无效化
        id_ = -1;                     // 死亡细胞ID无效化
    }
}
void Cell::setEnergy(double energy)
{
    energy_ = energy;
    if (energy_ <= 0.0)
    {
        energy_ = 0.0;
        alive_ = false; // 能量耗尽细胞死亡
    }
}
