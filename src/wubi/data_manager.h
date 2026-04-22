#pragma once

#include "wubi_data_parser.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

namespace wubi {

/**
 * @brief 数据管理器
 *
 * 统一管理五笔数据的加载、查询和图片资源管理
 */
class DataManager {
public:
    /**
     * @brief 构造函数
     */
    explicit DataManager(const std::string& assets_path = "assets");

    /**
     * @brief 析构函数
     */
    ~DataManager() = default;

    /**
     * @brief 禁用拷贝构造和赋值
     */
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

    /**
     * @brief 初始化数据管理器
     * @return 是否初始化成功
     */
    bool initialize();

    /**
     * @brief 根据输入查询汉字数据
     * @param input 输入字符串
     * @return 查询结果列表
     */
    std::vector<CharData*> query(const std::string& input);

    /**
     * @brief 根据单个汉字查询数据
     * @param character 汉字字符
     * @return 汉字数据指针，未找到返回nullptr
     */
    CharData* queryByCharacter(const std::string& character);

    /**
     * @brief 检查是否有对应的字根图片
     * @param character 汉字字符
     * @return 是否有图片
     */
    bool hasImageData(const std::string& character) const;

    /**
     * @brief 获取字根图片文件路径
     * @param character 汉字字符
     * @return 图片文件路径，不存在返回空字符串
     */
    std::string getImagePath(const std::string& character) const;

    /**
     * @brief 获取已加载的汉字总数
     * @return 汉字总数
     */
    size_t getCharCount() const;

    /**
     * @brief 获取有图片的汉字数量
     * @return 有图片的汉字数量
     */
    size_t getImageCount() const;

    /**
     * @brief 获取资源路径
     * @return 资源路径
     */
    const std::string& getAssetsPath() const {
        return assets_path_;
    }

    /**
     * @brief 设置资源路径
     * @param path 资源路径
     */
    void setAssetsPath(const std::string& path) {
        assets_path_ = path;
    }

    /**
     * @brief 检查数据是否已加载
     * @return 数据是否已加载
     */
    bool isDataLoaded() const {
        return data_loaded_;
    }

private:
    /**
     * @brief 扫描可用的字根图片文件
     * @return 是否扫描成功
     */
    bool scanAvailableImages();

    /**
     * @brief 检查文件是否存在
     * @param filepath 文件路径
     * @return 文件是否存在
     */
    bool fileExists(const std::string& filepath) const;

private:
    std::unique_ptr<WubiDataParser> parser_;              // 数据解析器
    std::string assets_path_;                             // 原始资源路径
    std::string resolved_assets_path_;                    // 解析后的绝对资源路径
    std::unordered_set<std::string> available_images_;    // 可用的图片文件集合
    bool data_loaded_;                                    // 数据是否已加载
};

} // namespace wubi