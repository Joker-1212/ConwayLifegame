#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <string>
#include <fstream>

/**
 * @file config_parser.h
 * @brief 配置解析器接口声明
 */

/**
 * @class ConfigParser
 * @brief 配置管理类
 *
 * 提供配置文件的读写接口，此处仅保留 Python 端直接调用的方法
 */
// TODO:具体实现有待补充
class ConfigParser
{
private:
    std::string config_file_path_; ///< 配置文件路径
    int i_config[7];               ///< 整型配置存储,前5项目依次代表Live_min, Live_max, Breed_min, Breed_max, Vision，后2项目依次代表X, Y
    double f_config[4];            ///< 浮点型配置存储，依次代表Death_Rate, Energy_comsumption, Restore_prob, Restore_value

public:
    /**
     * @brief 构造函数
     * @param config_file 配置文件路径，默认为"config.txt"
     */
    ConfigParser(const std::string &config_file = "config.txt");

    // 以下方法在 PyBind11 绑定中被直接调用

    /**
     * @brief 从文件加载配置
     * @return 操作是否成功
     */
    bool loadConfig();

       /**
     * @brief 获取整型配置值
     * @param key 配置键名
     * @param default_value 默认值
     * @return 配置值或默认值
     */
    int getInt(const std::string &key, int default_value) const;

    /**
     * @brief 获取浮点型配置值
     * @param key 配置键名
     * @param default_value 默认值
     * @return 配置值或默认值
     */
    double getDouble(const std::string &key, double default_value) const;

    /**
     * @brief 设置整型配置值
     * @param key 配置键名
     * @param value 配置值
     */
    void setInt(const std::string &key, int value);

    /**
     * @brief 设置浮点型配置值
     * @param key 配置键名
     * @param value 配置值
     */
    void setDouble(const std::string &key, double value);

    /**
     * @brief 检查配置键是否存在
     * @param key 配置键名
     * @return 如果键存在返回true，否则false
     */
    bool hasKey(const std::string &key) const;

    /**
     * @brief 打印所有配置值
     *
     * 在 Python 绑定中用于调试和配置查看
     */
    void printAll() const;
};

#endif // CONFIG_PARSER_H
