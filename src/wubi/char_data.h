#pragma once

#include <string>
#include <vector>

namespace wubi {

/**
 * @brief 汉字数据结构
 *
 * 存储单个汉字的五笔编码和相关信息
 */
struct CharData {
    std::string character;              // 汉字字符
    std::string wubi_code;              // 五笔全码
    std::vector<std::string> simple_codes; // 简码/容错码列表
    std::string pinyin;                 // 拼音
    std::string strokes;                // 笔画数
    std::string segments;               // 字根拆解描述
    bool has_image;                     // 是否有字根图片
    std::string image_path;             // 图片文件路径

    /**
     * @brief 默认构造函数
     */
    CharData() : has_image(false) {}

    /**
     * @brief 构造函数
     * @param char_str 汉字字符
     */
    explicit CharData(const std::string& char_str)
        : character(char_str), has_image(false) {}

    /**
     * @brief 检查数据是否有效
     * @return 数据是否包含有效信息
     */
    bool isValid() const {
        return !character.empty() && !wubi_code.empty();
    }

    /**
     * @brief 获取所有编码（全码+简码）
     * @return 所有编码的列表
     */
    std::vector<std::string> getAllCodes() const {
        std::vector<std::string> codes;
        if (!wubi_code.empty()) {
            codes.push_back(wubi_code);
        }
        codes.insert(codes.end(), simple_codes.begin(), simple_codes.end());
        return codes;
    }

    /**
     * @brief 获取简码/容错码的字符串表示
     * @return 用逗号分隔的简码字符串
     */
    std::string getSimpleCodesString() const {
        if (simple_codes.empty()) {
            return "";
        }

        std::string result;
        for (size_t i = 0; i < simple_codes.size(); ++i) {
            if (i > 0) result += ", ";
            result += simple_codes[i];
        }
        return result;
    }
};

} // namespace wubi