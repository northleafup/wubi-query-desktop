#include "wubi_data_parser.h"
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <iostream>

namespace wubi {

bool WubiDataParser::loadWubiData(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open wubi data file: " << filepath << std::endl;
        return false;
    }

    std::string line;
    if (!std::getline(file, line)) {
        std::cerr << "Failed to read header from wubi data file: " << filepath << std::endl;
        return false;
    }

    int line_count = 0;
    int empty_char_count = 0;
    while (std::getline(file, line)) {
        CharData char_data = parseWubiLine(line);
        if (char_data.character.empty()) {
            empty_char_count++;
            continue;  // 跳过空字符的行
        }

        // 保存字符键，避免在移动后丢失
        std::string char_key = char_data.character;

        // 如果字符已存在，更新现有数据
        auto it = char_map_.find(char_key);
        if (it != char_map_.end()) {
            // 更新五笔编码相关信息
            it->second->wubi_code = char_data.wubi_code;
            it->second->simple_codes = char_data.simple_codes;
            it->second->segments = char_data.segments;
        } else {
            char_map_[char_key] = std::make_unique<CharData>(std::move(char_data));
        }
        line_count++;
    }

    if (empty_char_count > 0) {
        std::cout << "Warning: Skipped " << empty_char_count << " lines with empty characters" << std::endl;
    }

    std::cout << "Loaded " << line_count << " wubi characters from: " << filepath << std::endl;
    return true;
}

bool WubiDataParser::loadCharData(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open char data file: " << filepath << std::endl;
        return false;
    }

    std::string line;
    if (!std::getline(file, line)) {
        std::cerr << "Failed to read header from char data file: " << filepath << std::endl;
        return false;
    }

    int line_count = 0;
    while (std::getline(file, line)) {
        CharData char_data = parseCharLine(line);
        if (!char_data.character.empty()) {
            // 保存字符键，避免在移动后丢失
            std::string char_key = char_data.character;

            // 如果字符已存在，更新现有数据
            auto it = char_map_.find(char_key);
            if (it != char_map_.end()) {
                // 只更新基本信息，保留已有的五笔编码数据
                it->second->pinyin = char_data.pinyin;
                it->second->strokes = char_data.strokes;
            } else {
                // 只有当字符不存在时才创建新记录
                char_map_[char_key] = std::make_unique<CharData>(std::move(char_data));
            }
            line_count++;
        }
    }

    std::cout << "Loaded " << line_count << " character basic info from: " << filepath << std::endl;
    return true;
}

CharData* WubiDataParser::queryByCharacter(const std::string& char_str) {
    auto it = char_map_.find(char_str);
    return (it != char_map_.end()) ? it->second.get() : nullptr;
}

std::vector<CharData*> WubiDataParser::queryByString(const std::string& input) {
    std::vector<CharData*> results;
    std::vector<std::string> chars = extractChineseChars(input);

    for (const auto& char_str : chars) {
        CharData* char_data = queryByCharacter(char_str);
        if (char_data) {
            results.push_back(char_data);
        }
    }

    return results;
}

std::vector<std::string> WubiDataParser::extractChineseChars(const std::string& input) {
    std::vector<std::string> chars;
    std::unordered_set<std::string> seen;

    for (size_t i = 0; i < input.size(); ) {
        uint32_t codepoint = 0;
        int bytes = decodeUTF8(input, i, codepoint);

        if (bytes <= 0) {
            i++; // 跳过无效字节
            continue;
        }

        if (isChineseChar(codepoint)) {
            std::string char_str = encodeUTF8(codepoint);
            if (seen.find(char_str) == seen.end()) {
                seen.insert(char_str);
                chars.push_back(char_str);
            }
        }

        i += bytes;
    }

    return chars;
}

CharData WubiDataParser::parseWubiLine(const std::string& line) {
    CharData data;
    std::stringstream ss(line);
    std::string cell;
    std::vector<std::string> cells;

    // 按制表符分割
    while (std::getline(ss, cell, '\t')) {
        cells.push_back(cell);
    }

    if (cells.size() >= 2 && !cells[0].empty()) {
        data.character = cells[0];                    // 汉字
        data.wubi_code = cells.size() > 1 ? cells[1] : "";                    // 五笔全码

                if (cells.size() > 2 && !cells[2].empty()) {
            data.simple_codes.push_back(cells[2]);    // 简码
        }
        if (cells.size() > 3 && !cells[3].empty()) {
            data.simple_codes.push_back(cells[3]);    // 容错码
        }
        // segments字段在最后一列，可能是第9列或第10列
        if (cells.size() > 9) {
            data.segments = cells[9];                 // 字根拆解（第10列）
        } else if (cells.size() > 8) {
            data.segments = cells[8];                 // 字根拆解（第9列）
        }
    }

    return data;
}

CharData WubiDataParser::parseCharLine(const std::string& line) {
    CharData data;
    std::stringstream ss(line);
    std::string cell;
    std::vector<std::string> cells;

    // 按制表符分割
    while (std::getline(ss, cell, '\t')) {
        cells.push_back(cell);
    }

    if (cells.size() >= 2 && cells.size() > 1 && !cells[1].empty()) {
        data.character = cells[1];                    // 汉字在第2列
        data.strokes = cells.size() > 5 ? cells[5] : "";                      // 笔画数在第6列
        data.pinyin = cells.size() > 6 ? cells[6] : "";                       // 拼音在第7列
    }

    return data;
}

std::string WubiDataParser::encodeUTF8(uint32_t codepoint) {
    std::string result;

    if (codepoint <= 0x7F) {
        result.push_back(static_cast<char>(codepoint));
    } else if (codepoint <= 0x7FF) {
        result.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0xFFFF) {
        result.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0x10FFFF) {
        result.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    }

    return result;
}

int WubiDataParser::decodeUTF8(const std::string& input, size_t index, uint32_t& codepoint) {
    if (index >= input.size()) {
        return 0;
    }

    uint8_t first_byte = static_cast<uint8_t>(input[index]);

    if ((first_byte & 0x80) == 0x00) {
        // 1字节字符
        codepoint = first_byte;
        return 1;
    } else if ((first_byte & 0xE0) == 0xC0) {
        // 2字节字符
        if (index + 1 >= input.size()) return 0;
        uint8_t second_byte = static_cast<uint8_t>(input[index + 1]);
        if ((second_byte & 0xC0) != 0x80) return 0;

        codepoint = ((first_byte & 0x1F) << 6) | (second_byte & 0x3F);
        return 2;
    } else if ((first_byte & 0xF0) == 0xE0) {
        // 3字节字符
        if (index + 2 >= input.size()) return 0;
        uint8_t second_byte = static_cast<uint8_t>(input[index + 1]);
        uint8_t third_byte = static_cast<uint8_t>(input[index + 2]);
        if ((second_byte & 0xC0) != 0x80 || (third_byte & 0xC0) != 0x80) return 0;

        codepoint = ((first_byte & 0x0F) << 12) | ((second_byte & 0x3F) << 6) | (third_byte & 0x3F);
        return 3;
    } else if ((first_byte & 0xF8) == 0xF0) {
        // 4字节字符
        if (index + 3 >= input.size()) return 0;
        uint8_t second_byte = static_cast<uint8_t>(input[index + 1]);
        uint8_t third_byte = static_cast<uint8_t>(input[index + 2]);
        uint8_t fourth_byte = static_cast<uint8_t>(input[index + 3]);
        if ((second_byte & 0xC0) != 0x80 || (third_byte & 0xC0) != 0x80 || (fourth_byte & 0xC0) != 0x80) return 0;

        codepoint = ((first_byte & 0x07) << 18) | ((second_byte & 0x3F) << 12) |
                   ((third_byte & 0x3F) << 6) | (fourth_byte & 0x3F);
        return 4;
    }

    return 0; // 无效的UTF-8
}

bool WubiDataParser::isChineseChar(uint32_t codepoint) const {
    // CJK统一汉字
    if (codepoint >= 0x4E00 && codepoint <= 0x9FFF) {
        return true;
    }
    // CJK扩展A
    if (codepoint >= 0x3400 && codepoint <= 0x4DBF) {
        return true;
    }
    // CJK兼容汉字
    if (codepoint >= 0xF900 && codepoint <= 0xFAFF) {
        return true;
    }

    return false;
}

size_t WubiDataParser::filterDataWithImages(const std::unordered_set<std::string>& available_chars) {
    size_t original_count = char_map_.size();
    size_t removed_count = 0;

    // 使用迭代器安全地删除元素
    auto it = char_map_.begin();
    while (it != char_map_.end()) {
        if (available_chars.find(it->first) == available_chars.end()) {
            // 这个汉字没有对应的图片，删除它
            it = char_map_.erase(it);
            removed_count++;
        } else {
            ++it;
        }
    }

    std::cout << "Filtered " << removed_count << " characters without images" << std::endl;
    std::cout << "Retained " << char_map_.size() << " characters with images (from " << original_count << " total)" << std::endl;

    return char_map_.size();
}

} // namespace wubi