#pragma once

#include <string>
#include <memory>
#include <vector>
#include "imgui.h"

/**
 * 字体管理器类
 * 负责管理中英文字体，确保中文正确显示
 */
class FontManager {
public:
    FontManager();
    ~FontManager();

    /**
     * 初始化字体系统
     * @return 初始化是否成功
     */
    bool initialize();

    /**
     * 获取默认字体
     * @return 默认中英文字体指针
     */
    ImFont* getDefaultFont() const;

    /**
     * 获取大号字体
     * @return 大号中英文字体指针
     */
    ImFont* getLargeFont() const;

    /**
     * 检查字符是否为中文字符
     * @param c 要检测的字符
     * @return 是否为中文字符
     */
    static bool isChineseCharacter(char c);

    /**
     * 检查字符串是否包含中文
     * @param str 要检测的字符串
     * @return 是否包含中文字符
     */
    static bool containsChinese(const std::string& str);

    /**
     * 检查字体系统是否已初始化
     * @return 是否已初始化
     */
    bool isInitialized() const;

    /**
     * 重新加载字体（当字体文件更新时）
     * @return 重新加载是否成功
     */
    bool reloadFonts();

private:
    bool m_initialized;
    ImFont* m_defaultFont;
    ImFont* m_largeFont;

    /**
     * 加载中文字体
     * @return 加载是否成功
     */
    bool loadChineseFont();

    /**
     * 设置中文字符范围
     * @return 字符范围构建器
     */
    ImVector<ImWchar> setupChineseRanges();

    /**
     * 获取中文字体文件路径
     * @return 字体文件路径
     */
    std::string getChineseFontPath() const;

    /**
     * 备用字体加载（当找不到中文字体时）
     * @return 备用字体加载是否成功
     */
    bool loadFallbackFont();
};