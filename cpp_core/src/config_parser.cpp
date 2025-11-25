#include "../include/config_parser.h"
#include <iostream>

/**
 * @file config_parser.cpp
 * @brief 配置解析器实现文件
 *
 * 实现配置管理接口，仅保留 Python 绑定中使用的方法
 * 具体实现代码需要后续补充
 */

ConfigParser::ConfigParser(const std::string &config_file)
    : config_file_path_(config_file)
{
}

bool ConfigParser::loadConfig()
{
    // 从配置文件加载配置
    std::ifstream file(config_file_path_);
    if (!file.is_open())
    {
        return false;
    }
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line))
    {
        // 跳过空行
        if (line.empty())
        {
            continue;
        }
        // 跳过注释行
        if (line[0] == '#')
        {
            continue;
        }
        size_t pos = line.find('=');
        if (pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            if (lineNumber < 5)
            {
                i_config[lineNumber] = std::stoi(value);
            }
            else if (lineNumber < 9)
            {
                f_config[lineNumber - 5] = std::stod(value);
            }
            else if (lineNumber == 9)
            {
                i_config[5] = std::stoi(value);
            }
            else if (lineNumber == 10)
            {
                i_config[6] = std::stoi(value);
            }
            lineNumber++;
        }
    }
    return true;
}
int ConfigParser::getInt(const std::string &key, int default_value) const
{
    // 获取类型为整型的配置值
    if (key == "Live_min")
    {
        return i_config[0];
    }
    else if (key == "Live_max")
    {
        return i_config[1];
    }
    else if (key == "Breed_min")
    {
        return i_config[2];
    }
    else if (key == "Breed_max")
    {
        return i_config[3];
    }
    else if (key == "Vision")
    {
        return i_config[4];
    }
    else if (key == "X")
    {
        return i_config[5];
    }
    else if (key == "Y")
    {
        return i_config[6];
    }
    else
    {
        return default_value;
    }
}

double ConfigParser::getDouble(const std::string &key, double default_value) const
{
    // 获取类型为浮点型的配置值
    if (key == "Death_Rate")
    {
        return f_config[0];
    }
    else if (key == "Energy_comsumption")
    {
        return f_config[1];
    }
    else if (key == "Restore_prob")
    {
        return f_config[2];
    }
    else if (key == "Restore_value")
    {
        return f_config[3];
    }
    else
    {
        return default_value;
    }
}

void ConfigParser::setInt(const std::string &key, int value)
{
    // 保存配置到文件
    if (key == "Live_min")
    {
        i_config[0] = value;
    }
    else if (key == "Live_max")
    {
        i_config[1] = value;
    }
    else if (key == "Breed_min")
    {
        i_config[2] = value;
    }
    else if (key == "Breed_max")
    {
        i_config[3] = value;
    }
    else if (key == "Vision")
    {
        i_config[4] = value;
    }
    else if (key == "X")
    {
        i_config[5] = value;
    }
    else if (key == "Y")
    {
        i_config[6] = value;
    }
}

void ConfigParser::setDouble(const std::string &key, double value)
{
    // 设置类型为浮点型的配置值
    if (key == "Death_Rate")
    {
        f_config[0] = value;
    }
    else if (key == "Energy_comsumption")
    {
        f_config[1] = value;
    }
    else if (key == "Restore_prob")
    {
        f_config[2] = value;
    }
    else if (key == "Restore_value")
    {
        f_config[3] = value;
    }
}

bool ConfigParser::hasKey(const std::string &key) const
{
    // 检查配置键值对是否存在
    if (key == "Live_min" || key == "Live_max" || key == "Breed_min" || key == "Breed_max" || key == "Vision" || key == "X" || key == "Y" ||
        key == "Death_Rate" || key == "Energy_comsumption" || key == "Restore_prob" || key == "Restore_value")
    {
        return true;
    }
    else
    {
        return false;
    }
}

void ConfigParser::printAll() const
{
    // 输出所有配置值
    std::cout << "Minimum number of neighbors for a cell to survive is :" << i_config[0] << std::endl;
    std::cout << "Maximum number of neighbors for a cell to survive is :" << i_config[1] << std::endl;
    std::cout << "Minimum number of neighbors for a cell to be born is :" << i_config[2] << std::endl;
    std::cout << "Maximum number of neighbors for a cell to be born is :" << i_config[3] << std::endl;
    std::cout << "Cell vision distance is :" << i_config[4] << std::endl;
    std::cout << "Cell death probability is :" << f_config[0] << std::endl;
    std::cout << "Cell energy consumption is :" << f_config[1] << std::endl;
    std::cout << "Cell energy restore probability is :" << f_config[2] << std::endl;
    std::cout << "Cell energy restore value is :" << f_config[3] << std::endl;
    std::cout << "width of the gird is :" << i_config[5] << std::endl;
    std::cout << "height of the gird is :" << i_config[6] << std::endl;
}
