#pragma once

#include "imgui.h"
#include <string>

/**
 * 背景渲染器类
 * 负责渲染各平台的经典背景页
 */
class BackgroundRenderer {
public:
    enum class Platform {
        LINUX,
        MACOS,
        WINDOWS
    };

    BackgroundRenderer();
    ~BackgroundRenderer();

    /**
     * 初始化背景渲染器
     * @return 初始化是否成功
     */
    bool initialize();

    /**
     * 渲染背景
     * @param platform 要渲染的平台背景
     */
    void render(Platform platform);

    /**
     * 设置当前平台
     * @param platform 当前平台
     */
    void setCurrentPlatform(Platform platform);

    /**
     * 获取当前平台
     * @return 当前平台
     */
    Platform getCurrentPlatform() const;

    /**
     * 获取平台名称
     * @param platform 平台枚举
     * @return 平台名称
     */
    std::string getPlatformName(Platform platform) const;

    /**
     * 检查是否已初始化
     * @return 是否已初始化
     */
    bool isInitialized() const;

private:
    bool m_initialized;
    Platform m_currentPlatform;

    /**
     * 渲染Linux经典背景
     */
    void renderLinuxBackground();

    /**
     * 渲染macOS经典背景
     */
    void renderMacOSBackground();

    /**
     * 渲染Windows经典背景
     */
    void renderWindowsBackground();

    /**
     * 绘制渐变背景
     * @param startColor 起始颜色
     * @param endColor 结束颜色
     * @param width 宽度
     * @param height 高度
     */
    void drawGradientBackground(ImVec4 startColor, ImVec4 endColor, float width, float height);

    /**
     * 绘制图案背景
     * @param color 主色
     * @param patternSize 图案大小
     * @param width 宽度
     * @param height 高度
     */
    void drawPatternBackground(ImVec4 color, float patternSize, float width, float height);

    /**
     * 绘制平台标识
     * @param text 标识文本
     * @param position 位置
     * @param color 颜色
     * @param size 字体大小
     */
    void drawPlatformLogo(const std::string& text, ImVec2 position, ImVec4 color, float size);

    /**
     * 检测当前平台
     * @return 检测到的平台
     */
    static Platform detectCurrentPlatform();
};