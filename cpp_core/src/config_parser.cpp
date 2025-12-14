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
    i_config_map = {
        {"LIVE_MIN", 0}, {"LIVE_MAX", 1}, {"BREED_MIN", 2}, {"BREED_MAX", 3}, {"VISION", 4}, {"ENV_WIDTH", 5}, {"ENV_HEIGHT", 6}};
    f_config_map = {
        {"DEATH_RATE", 0}, {"ENERGY_CONSUMPTION", 1}, {"RESTORE_PROB", 2}, {"RESTORE_VALUE", 3}};
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
            // 去除前后空格
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            // 存储配置
            if (i_config_map.find(key) != i_config_map.end())
            {
                i_config[i_config_map[key]] = std::stoi(value);
            }
            else if (f_config_map.find(key) != f_config_map.end())
            {
                f_config[f_config_map[key]] = std::stod(value);
            }
        }
    }
    return true;
}
bool ConfigParser::saveConfig()
{
    std::ifstream inFile(config_file_path_);
    if (!inFile.is_open())
    {
        return false;
    }

    // 读取原文件所有内容到内存
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(inFile, line))
    {
        lines.push_back(line);
    }
    inFile.close();

    // 重新打开文件进行写入（覆盖模式）
    std::ofstream outFile(config_file_path_);
    if (!outFile.is_open())
    {
        return false;
    }

    // 处理每一行
    for (const auto &currentLine : lines)
    {
        std::string trimmedLine = currentLine;

        // 去除前后空白
        if (!trimmedLine.empty())
        {
            size_t start = trimmedLine.find_first_not_of(" \t");
            size_t end = trimmedLine.find_last_not_of(" \t");
            if (start != std::string::npos && end != std::string::npos)
            {
                trimmedLine = trimmedLine.substr(start, end - start + 1);
            }
        }

        // 跳过注释和空行（原样写入）
        if (trimmedLine.empty() || trimmedLine[0] == '#')
        {
            outFile << currentLine << std::endl;
            continue;
        }

        // 检查键值对
        size_t delimPos = trimmedLine.find('=');
        if (delimPos != std::string::npos)
        {
            std::string key = trimmedLine.substr(0, delimPos);

            // 去除键的前后空白
            size_t keyStart = key.find_first_not_of(" \t");
            size_t keyEnd = key.find_last_not_of(" \t");
            if (keyStart != std::string::npos && keyEnd != std::string::npos)
            {
                key = key.substr(keyStart, keyEnd - keyStart + 1);
            }

            // 检查并更新配置值
            if (i_config_map.find(key) != i_config_map.end())
            {
                // 找到原始行中的等号位置
                size_t eqPos = currentLine.find('=');
                if (eqPos != std::string::npos)
                {
                    // 保留等号前的所有内容，只修改等号后的值
                    outFile << currentLine.substr(0, eqPos + 1) << " "
                            << i_config[i_config_map[key]] << std::endl;
                    continue;
                }
            }
            else if (f_config_map.find(key) != f_config_map.end())
            {
                size_t eqPos = currentLine.find('=');
                if (eqPos != std::string::npos)
                {
                    outFile << currentLine.substr(0, eqPos + 1) << " "
                            << f_config[f_config_map[key]] << std::endl;
                    continue;
                }
            }
        }

        // 其他行原样写入
        outFile << currentLine << std::endl;
    }

    outFile.close();
    return true;
}
int ConfigParser::getInt(const std::string &key, int default_value) const
{
    // 获取类型为整型的配置值
    if (i_config_map.find(key) != i_config_map.end())
    {
        return i_config[i_config_map.at(key)];
    }
    else
    {
        return default_value;
    }
    return default_value;
}

double ConfigParser::getDouble(const std::string &key, double default_value) const
{
    // 获取类型为浮点型的配置值
    if (f_config_map.find(key) != f_config_map.end())
    {
        return f_config[f_config_map.at(key)];
    }
    else
    {
        return default_value;
    }
}

void ConfigParser::setInt(const std::string &key, int value)
{
    // 设置类型为整型的配置值
    if (i_config_map.find(key) != i_config_map.end())
    {
        i_config[i_config_map.at(key)] = value;
    }
}

void ConfigParser::setDouble(const std::string &key, double value)
{
    // 设置类型为浮点型的配置值
    if (f_config_map.find(key) != f_config_map.end())
    {
        f_config[f_config_map.at(key)] = value;
    }
}

bool ConfigParser::hasKey(const std::string &key) const
{
    // 检查配置键值对是否存在
    return (i_config_map.find(key) != i_config_map.end() || f_config_map.find(key) != f_config_map.end());
}

void ConfigParser::printAll() const
{
    // 输出所有配置值
    std::cout << "Integer Configurations:" << std::endl;
    for (const auto &pair : i_config_map)
    {
        std::cout << pair.first << " = " << i_config[pair.second] << std::endl;
    }
    std::cout << "Double Configurations:" << std::endl;
    for (const auto &pair : f_config_map)
    {
        std::cout << pair.first << " = " << f_config[pair.second] << std::endl;
    }
}