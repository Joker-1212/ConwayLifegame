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
