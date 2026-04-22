#pragma once

#include "char_data.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace wubi {

/**
 * @brief 五笔数据解析器
 *
 * 负责解析TSV格式的五笔编码数据文件和汉字基本信息数据文件
 */
class WubiDataParser {
public:
    /**
     * @brief 构造函数
     */
    WubiDataParser() = default;

    /**
     * @brief 析构函数
     */
    ~WubiDataParser() = default;

    /**
     * @brief 禁用拷贝构造和赋值
     */
    WubiDataParser(const WubiDataParser&) = delete;
    WubiDataParser& operator=(const WubiDataParser&) = delete;

    /**
     * @brief 加载五笔编码数据
     * @param filepath TSV文件路径
     * @return 是否加载成功
     */
    bool loadWubiData(const std::string& filepath);

    /**
     * @brief 加载汉字基本信息数据
     * @param filepath TSV文件路径
     * @return 是否加载成功
     */
    bool loadCharData(const std::string& filepath);

    /**
     * @brief 根据汉字查询数据
     * @param char_str 汉字字符
     * @return 汉字数据指针，未找到返回nullptr
     */
    CharData* queryByCharacter(const std::string& char_str);

    /**
     * @brief 根据输入字符串批量查询
     * @param input 输入字符串（可能包含多个汉字）
     * @return 查询结果列表
     */
    std::vector<CharData*> queryByString(const std::string& input);

    /**
     * @brief 获取所有已加载的汉字数据
     * @return 汉字数据映射
     */
    const std::unordered_map<std::string, std::unique_ptr<CharData>>& getAllCharData() const {
        return char_map_;
    }

    /**
     * @brief 获取已加载的汉字数量
     * @return 汉字数量
     */
    size_t getCharCount() const {
        return char_map_.size();
    }

    /**
     * @brief 过滤数据，只保留有图片的汉字
     * @param available_chars 有图片的汉字集合
     * @return 过滤后保留的汉字数量
     */
    size_t filterDataWithImages(const std::unordered_set<std::string>& available_chars);

private:
    /**
     * @brief 从输入字符串中提取汉字
     * @param input 输入字符串
     * @return 汉字列表（去重，保持顺序）
     */
    std::vector<std::string> extractChineseChars(const std::string& input);

    /**
     * @brief 解析五笔编码数据行
     * @param line TSV行数据
     * @return 解析的汉字数据
     */
    CharData parseWubiLine(const std::string& line);

    /**
     * @brief 解析汉字基本信息数据行
     * @param line TSV行数据
     * @return 解析的汉字基本信息
     */
    CharData parseCharLine(const std::string& line);

    /**
     * @brief UTF-8编码
     * @param codepoint Unicode码点
     * @return UTF-8编码的字符串
     */
    std::string encodeUTF8(uint32_t codepoint);

    /**
     * @brief UTF-8解码
     * @param input 输入字符串
     * @param index 字符索引位置
     * @param codepoint 解码后的Unicode码点
     * @return 字符的字节长度
     */
    int decodeUTF8(const std::string& input, size_t index, uint32_t& codepoint);

    /**
     * @brief 检查字符是否为汉字
     * @param codepoint Unicode码点
     * @return 是否为汉字
     */
    bool isChineseChar(uint32_t codepoint) const;

private:
    std::unordered_map<std::string, std::unique_ptr<CharData>> char_map_;  // 汉字数据映射
};

} // namespace wubi