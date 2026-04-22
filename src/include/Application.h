#pragma once

#include <memory>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include "BackgroundRenderer.h"

class FontManager;
class BackgroundRenderer;
class ImageLoader;
namespace wubi {
    class DataManager;
    struct CharData;
}

/**
 * 主应用程序类
 * 管理整个应用的生命周期和渲染循环
 */
class Application {
public:
    Application();
    ~Application();

    /**
     * 初始化应用程序
     * @return 初始化是否成功
     */
    bool initialize();

    /**
     * 运行应用程序主循环
     * @return 应用程序退出码
     */
    int run();

    /**
     * 清理应用程序资源
     */
    void cleanup();

    /**
     * 获取应用程序名称
     * @return 应用程序名称
     */
    std::string getApplicationName() const;

    /**
     * 设置应用程序名称
     * @param name 新的应用程序名称
     */
    void setApplicationName(const std::string& name);

    /**
     * 获取窗口宽度
     * @return 窗口宽度
     */
    int getWindowWidth() const;

    /**
     * 获取窗口高度
     * @return 窗口高度
     */
    int getWindowHeight() const;

    /**
     * 设置窗口大小
     * @param width 窗口宽度
     * @param height 窗口高度
     */
    void setWindowSize(int width, int height);

    /**
     * 检查应用程序是否已初始化
     * @return 是否已初始化
     */
    bool isInitialized() const;

    /**
     * 检查是否应该退出
     * @return 是否应该退出
     */
    bool shouldExit() const;

    /**
     * 设置退出标志
     * @param shouldExit 是否应该退出
     */
    void setShouldExit(bool shouldExit);

private:
    bool m_initialized;
    bool m_running;
    std::string m_applicationName;
    int m_windowWidth;
    int m_windowHeight;

    GLFWwindow* m_window;
    std::unique_ptr<FontManager> m_fontManager;
    std::unique_ptr<BackgroundRenderer> m_backgroundRenderer;
    std::unique_ptr<ImageLoader> m_imageLoader;
    BackgroundRenderer::Platform m_currentPlatform;

    // 五笔查询系统组件
    std::unique_ptr<wubi::DataManager> m_dataManager;
    std::vector<wubi::CharData*> m_searchResults;
    char m_searchInput[256];
    bool m_dataInitialized;

    /**
     * 初始化GLFW
     * @return 初始化是否成功
     */
    bool initializeGLFW();

    /**
     * 创建窗口
     * @return 窗口创建是否成功
     */
    bool createWindow();

    /**
     * 初始化ImGui
     * @return ImGui初始化是否成功
     */
    bool initializeImGui();

    /**
     * 设置ImGui样式
     */
    void setupImGuiStyle();

    /**
     * 处理输入事件
     */
    void handleInput();

    /**
     * 渲染界面
     */
    void render();

    /**
     * 渲染主界面
     */
    void renderMainInterface();

    /**
     * 渲染控制面板
     */
    void renderControlPanel();

    /**
     * 渲染信息面板
     */
    void renderInfoPanel();

    /**
     * 渲染五笔查询界面
     */
    void renderWubiSearchPanel();

    /**
     * 渲染搜索结果表格
     */
    void renderSearchResults();

    /**
     * 执行五笔查询
     * @param input 查询输入
     */
    void performSearch(const std::string& input);

    /**
     * 渲染字根图片
     * @param charData 汉字数据
     * @return 是否成功渲染
     */
    bool renderCharacterImage(const wubi::CharData* charData);

    /**
     * 更新应用程序状态
     */
    void update();

    /**
     * 渲染帧
     */
    void renderFrame();

    /**
     * 处理窗口大小变化
     * @param width 新的宽度
     * @param height 新的高度
     */
    void handleWindowSizeChange(int width, int height);

    /**
     * 错误回调函数
     * @param error 错误代码
     * @param description 错误描述
     */
    static void errorCallback(int error, const char* description);

    /**
     * 窗口大小变化回调函数
     * @param window 窗口指针
     * @param width 新的宽度
     * @param height 新的高度
     */
    static void windowSizeCallback(GLFWwindow* window, int width, int height);

    /**
     * 键盘回调函数
     * @param window 窗口指针
     * @param key 按键
     * @param scancode 扫描码
     * @param action 动作
     * @param mods 修饰键
     */
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    /**
     * 获取平台名称的友好显示
     * @param platform 平台枚举
     * @return 友好的平台名称
     */
    std::string getPlatformDisplayName(const std::string& platform) const;
};