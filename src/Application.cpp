#include "Application.h"
#include "FontManager.h"
#include "BackgroundRenderer.h"
#include "ImageLoader.h"
#include "wubi/data_manager.h"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdarg>
#include <thread>
#include <chrono>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

static void CenteredText(const char* text)
{
    float availX = ImGui::GetContentRegionAvail().x;
    ImVec2 textSize = ImGui::CalcTextSize(text);
    float padX = (availX - textSize.x) / 2.0f;
    if (padX > 0) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padX);
    ImGui::TextUnformatted(text);
}

static void CenteredTextColored(const ImVec4& color, const char* format, ...)
{
    char buf[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    float availX = ImGui::GetContentRegionAvail().x;
    ImVec2 textSize = ImGui::CalcTextSize(buf);
    float padX = (availX - textSize.x) / 2.0f;
    if (padX > 0) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padX);
    ImGui::TextColored(color, "%s", buf);
}

static void CenteredTextVert(const ImVec4& color, const char* text, float rowHeight)
{
    float availX = ImGui::GetContentRegionAvail().x;
    ImVec2 textSize = ImGui::CalcTextSize(text);
    float padX = (availX - textSize.x) / 2.0f;
    float padY = (rowHeight - textSize.y) / 2.0f;
    if (padX > 0) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padX);
    if (padY > 0) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + padY);
    ImGui::TextColored(color, "%s", text);
}

Application::Application()
    : m_initialized(false)
    , m_running(true)
    , m_applicationName("五笔编码查询系统")
    , m_windowWidth(1200)
    , m_windowHeight(800)
    , m_window(nullptr)
    , m_fontManager(nullptr)
    , m_backgroundRenderer(nullptr)
    , m_imageLoader(nullptr)
    , m_currentPlatform(BackgroundRenderer::Platform::LINUX)
    , m_dataManager(nullptr)
    , m_dataInitialized(false)
{
    // 初始化搜索输入
    memset(m_searchInput, 0, sizeof(m_searchInput));
}

Application::~Application()
{
    cleanup();
}

bool Application::initialize()
{
    if (m_initialized) {
        return true;
    }

    std::cout << "正在初始化应用程序..." << std::endl;

    try {
        // 初始化GLFW
        if (!initializeGLFW()) {
            std::cerr << "GLFW初始化失败" << std::endl;
            return false;
        }

        // 创建窗口
        if (!createWindow()) {
            std::cerr << "窗口创建失败" << std::endl;
            return false;
        }

        // 初始化ImGui
        if (!initializeImGui()) {
            std::cerr << "ImGui初始化失败" << std::endl;
            return false;
        }

        // 初始化字体管理器
        m_fontManager = std::make_unique<FontManager>();
        if (!m_fontManager->initialize()) {
            std::cerr << "字体管理器初始化失败" << std::endl;
            return false;
        }

        // 初始化背景渲染器
        m_backgroundRenderer = std::make_unique<BackgroundRenderer>();
        if (!m_backgroundRenderer->initialize()) {
            std::cerr << "背景渲染器初始化失败" << std::endl;
            return false;
        }

        // 初始化图片加载器
        m_imageLoader = std::make_unique<ImageLoader>();

        // 设置ImGui样式
        setupImGuiStyle();

        // 检测当前平台
        m_currentPlatform = m_backgroundRenderer->getCurrentPlatform();
        m_backgroundRenderer->setCurrentPlatform(m_currentPlatform);

        // 初始化五笔数据管理器
        m_dataManager = std::make_unique<wubi::DataManager>("assets");
        if (!m_dataManager->initialize()) {
            std::cerr << "五笔数据管理器初始化失败" << std::endl;
            return false;
        }
        m_dataInitialized = true;
        std::cout << "五笔数据加载完成: " << m_dataManager->getCharCount()
                  << " 个汉字, " << m_dataManager->getImageCount() << " 张图片" << std::endl;

        m_initialized = true;
        m_running = true;

        std::cout << "应用程序初始化成功" << std::endl;
        std::cout << "当前平台: " << getPlatformDisplayName(m_backgroundRenderer->getPlatformName(m_currentPlatform)) << std::endl;
        std::cout << "窗口大小: " << m_windowWidth << "x" << m_windowHeight << std::endl;

        return true;

    } catch (const std::exception& e) {
        std::cerr << "初始化过程中发生异常: " << e.what() << std::endl;
        return false;
    }
}

int Application::run()
{
    if (!m_initialized) {
        std::cerr << "应用程序未初始化，无法运行" << std::endl;
        return -1;
    }

    std::cout << "开始运行应用程序主循环..." << std::endl;
    std::cout << "提示: 按 ESC 键或关闭窗口退出" << std::endl;
    std::cout << "=========================" << std::endl;

    // 主循环
    while (m_running) {
        handleInput();
        update();
        renderFrame();
    }

    std::cout << "应用程序主循环结束" << std::endl;
    return 0;
}

void Application::cleanup()
{
    if (m_initialized) {
        std::cout << "正在清理应用程序资源..." << std::endl;

        // 清理ImGui
        if (m_initialized) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        // 清理GLFW窗口
        if (m_window) {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }

        // 清理GLFW
        glfwTerminate();

        // 智能指针会自动清理
        m_fontManager.reset();
        m_backgroundRenderer.reset();

        m_initialized = false;
        std::cout << "应用程序资源清理完成" << std::endl;
    }
}

std::string Application::getApplicationName() const
{
    return m_applicationName;
}

void Application::setApplicationName(const std::string& name)
{
    m_applicationName = name;
    if (m_window) {
        glfwSetWindowTitle(m_window, m_applicationName.c_str());
    }
}

int Application::getWindowWidth() const
{
    return m_windowWidth;
}

int Application::getWindowHeight() const
{
    return m_windowHeight;
}

void Application::setWindowSize(int width, int height)
{
    m_windowWidth = width;
    m_windowHeight = height;
    if (m_window) {
        glfwSetWindowSize(m_window, width, height);
    }
}

bool Application::isInitialized() const
{
    return m_initialized;
}

bool Application::shouldExit() const
{
    return !m_running;
}

void Application::setShouldExit(bool shouldExit)
{
    m_running = !shouldExit;
}

bool Application::initializeGLFW()
{
    // 设置错误回调
    glfwSetErrorCallback(errorCallback);

    // 初始化GLFW
    if (!glfwInit()) {
        return false;
    }

    // 设置OpenGL版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    return true;
}

bool Application::createWindow()
{
    // 创建窗口
    m_window = glfwCreateWindow(m_windowWidth, m_windowHeight,
                               m_applicationName.c_str(), nullptr, nullptr);
    if (!m_window) {
        const char* error_desc;
        glfwGetError(&error_desc);
        if (error_desc) {
            std::cerr << "GLFW错误: " << error_desc << std::endl;
        }
        return false;
    }

    // 设置窗口为当前上下文
    glfwMakeContextCurrent(m_window);

    // 设置VSync
    glfwSwapInterval(1);

    // 设置窗口大小变化回调
    glfwSetWindowUserPointer(m_window, this);
    glfwSetWindowSizeCallback(m_window, windowSizeCallback);
    glfwSetKeyCallback(m_window, keyCallback);

    // 显示窗口
    glfwShowWindow(m_window);

    return true;
}

bool Application::initializeImGui()
{
    // 创建ImGui上下文
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // 设置配置文件路径到 config 目录
    io.IniFilename = "config/imgui.ini";

    // 启用光标闪烁，确保光标可见
    io.ConfigInputTextCursorBlink = true;

    // 移除不支持的DockingEnable配置

    // 设置平台/渲染器后端
    if (!ImGui_ImplGlfw_InitForOpenGL(m_window, true)) {
        std::cerr << "ImGui GLFW后端初始化失败" << std::endl;
        return false;
    }

    // 使用nullptr让ImGui自动选择合适的GLSL版本
    const char* glsl_version = nullptr;
    if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
        std::cerr << "ImGui OpenGL3后端初始化失败" << std::endl;
        return false;
    }

    return true;
}

void Application::setupImGuiStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.TabRounding = 6.0f;
    style.PopupRounding = 6.0f;

    style.WindowPadding = ImVec2(16.0f, 12.0f);
    style.FramePadding = ImVec2(10.0f, 8.0f);
    style.ItemSpacing = ImVec2(12.0f, 8.0f);
    style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
    style.TouchExtraPadding = ImVec2(4.0f, 4.0f);
    style.IndentSpacing = 20.0f;
    style.ScrollbarSize = 10.0f;
    style.GrabMinSize = 12.0f;

    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.DisplayWindowPadding = ImVec2(20.0f, 20.0f);
    style.DisplaySafeAreaPadding = ImVec2(4.0f, 4.0f);

    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text] = ImVec4(0.15f, 0.16f, 0.18f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.51f, 0.55f, 1.00f);

    colors[ImGuiCol_WindowBg] = ImVec4(0.88f, 0.90f, 0.93f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);

    colors[ImGuiCol_Border] = ImVec4(0.78f, 0.80f, 0.84f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    colors[ImGuiCol_FrameBg] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.92f, 0.95f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.85f, 0.87f, 0.91f, 1.00f);

    colors[ImGuiCol_TitleBg] = ImVec4(0.88f, 0.90f, 0.93f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.85f, 0.87f, 0.91f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.88f, 0.90f, 0.93f, 0.75f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.88f, 0.90f, 0.93f, 1.00f);

    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.92f, 0.94f, 0.96f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.70f, 0.72f, 0.76f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.58f, 0.60f, 0.65f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.46f, 0.48f, 0.53f, 1.00f);

    colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 0.88f, 1.00f);

    colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 0.88f, 0.78f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.28f, 0.56f, 0.88f, 1.00f);

    colors[ImGuiCol_Button] = ImVec4(0.28f, 0.56f, 0.88f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.22f, 0.48f, 0.80f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.18f, 0.42f, 0.72f, 1.00f);

    colors[ImGuiCol_Header] = ImVec4(0.28f, 0.56f, 0.88f, 0.15f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.56f, 0.88f, 0.25f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.28f, 0.56f, 0.88f, 0.35f);

    colors[ImGuiCol_Separator] = ImVec4(0.78f, 0.80f, 0.84f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.66f, 0.68f, 0.73f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.28f, 0.56f, 0.88f, 0.25f);

    colors[ImGuiCol_ResizeGrip] = ImVec4(0.74f, 0.76f, 0.80f, 0.50f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.66f, 0.68f, 0.73f, 0.75f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.28f, 0.56f, 0.88f, 0.40f);

    colors[ImGuiCol_Tab] = ImVec4(0.90f, 0.92f, 0.95f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.56f, 0.88f, 0.20f);
    colors[ImGuiCol_TabActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.88f, 0.90f, 0.93f, 0.95f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.92f, 0.94f, 0.96f, 1.00f);

    colors[ImGuiCol_PlotLines] = ImVec4(0.42f, 0.44f, 0.48f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.82f, 0.30f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.60f, 0.10f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.80f, 0.50f, 0.00f, 1.00f);

    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.91f, 0.93f, 0.96f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.80f, 0.82f, 0.86f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.86f, 0.88f, 0.91f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);

    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.28f, 0.56f, 0.88f, 0.20f);

    colors[ImGuiCol_DragDropTarget] = ImVec4(0.28f, 0.56f, 0.88f, 0.90f);

    colors[ImGuiCol_NavHighlight] = ImVec4(0.28f, 0.56f, 0.88f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.50f, 0.50f, 0.50f, 0.58f);

    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
}

void Application::handleInput()
{
    if (!m_window) {
        std::cout << "窗口句柄为空" << std::endl;
        m_running = false;
        return;
    }

    glfwPollEvents();

    // 检查ESC键退出
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        std::cout << "检测到ESC键" << std::endl;
        m_running = false;
        return;
    }

    // 检查窗口关闭
    if (glfwWindowShouldClose(m_window)) {
        std::cout << "glfwWindowShouldClose 返回 true" << std::endl;
        const char* error_desc;
        glfwGetError(&error_desc);
        if (error_desc) {
            std::cout << "GLFW错误: " << error_desc << std::endl;
        }
        m_running = false;
        return;
    }
}

void Application::render()
{
    // 清空屏幕
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 开始新帧
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 渲染背景
    if (m_backgroundRenderer) {
        m_backgroundRenderer->render(m_currentPlatform);
    }

    // 渲染主界面
    renderMainInterface();

    // 渲染ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // 交换缓冲区
    glfwSwapBuffers(m_window);
}

void Application::renderMainInterface()
{
    if (!m_fontManager) {
        return;
    }

    // 设置字体
    ImFont* font = m_fontManager->getDefaultFont();
    if (font) {
        ImGui::PushFont(font);
    }

    // 渲染五笔查询界面
    renderWubiSearchPanel();

    // 如果需要，可以保留原有的控制面板
    // renderControlPanel();

    // 如果需要，可以保留原有的信息面板
    // renderInfoPanel();

    // 恢复字体
    if (font) {
        ImGui::PopFont();
    }
}

void Application::renderControlPanel()
{
    // 控制面板窗口
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

    ImGui::Begin("控制面板", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize |
                 ImGuiWindowFlags_NoCollapse);

    ImGui::Text("平台背景选择器");
    ImGui::Separator();

    // 平台选择按钮
    const char* platformNames[] = {"Linux", "macOS", "Windows"};
    for (int i = 0; i < 3; ++i) {
        auto platform = static_cast<BackgroundRenderer::Platform>(i);
        bool isSelected = (m_currentPlatform == platform);

        if (ImGui::RadioButton(platformNames[i], isSelected)) {
            m_currentPlatform = platform;
            if (m_backgroundRenderer) {
                m_backgroundRenderer->setCurrentPlatform(m_currentPlatform);
            }
        }

        if (i < 2) {
            ImGui::SameLine();
        }
    }

    ImGui::Separator();
    ImGui::Text("当前平台: %s", getPlatformDisplayName(m_backgroundRenderer->getPlatformName(m_currentPlatform)).c_str());

    // 字体测试区域
    ImGui::Separator();
    ImGui::Text("中文字体测试:");
    ImGui::Text("你好，世界！");
    ImGui::Text("这是五笔编码查询工具");
    ImGui::Text("中文显示测试：成功！");

    ImGui::End();
}

void Application::renderInfoPanel()
{
    // 信息面板窗口
    ImGui::SetNextWindowPos(ImVec2(20, 240), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_FirstUseEver);

    ImGui::Begin("信息面板", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize |
                 ImGuiWindowFlags_NoCollapse);

    ImGui::Text("应用程序信息");
    ImGui::Separator();

    ImGui::Text("名称: %s", m_applicationName.c_str());
    ImGui::Text("窗口大小: %d x %d", m_windowWidth, m_windowHeight);
    ImGui::Text("字体状态: %s",
                m_fontManager && m_fontManager->isInitialized() ? "已加载" : "未加载");

    // FPS显示
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

    ImGui::End();
}

void Application::update()
{
    // 更新应用程序状态
    if (m_window) {
        // 获取窗口大小
        glfwGetWindowSize(m_window, &m_windowWidth, &m_windowHeight);
    }
}

void Application::renderFrame()
{
    render();
}

void Application::handleWindowSizeChange(int width, int height)
{
    m_windowWidth = width;
    m_windowHeight = height;
}

void Application::errorCallback(int error, const char* description)
{
    std::cerr << "GLFW错误 [" << error << "]: " << description << std::endl;
}

void Application::windowSizeCallback(GLFWwindow* window, int width, int height)
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->handleWindowSizeChange(width, height);
    }

    // 更新OpenGL视口
    glViewport(0, 0, width, height);
}

void Application::keyCallback(GLFWwindow* window, int key, int /* scancode */, int action, int /* mods */)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app) {
            app->setShouldExit(true);
        }
    }
}

std::string Application::getPlatformDisplayName(const std::string& platform) const
{
    if (platform == "Linux") {
        return "Linux (Ubuntu)";
    } else if (platform == "macOS") {
        return "macOS (Monterey)";
    } else if (platform == "Windows") {
        return "Windows (经典蓝)";
    } else {
        return "未知平台";
    }
}

// ==================== 五笔查询系统实现 ====================

void Application::renderWubiSearchPanel()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("##MainWindow", nullptr,
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoScrollWithMouse |
                 ImGuiWindowFlags_NoTitleBar);

    float windowWidth = ImGui::GetIO().DisplaySize.x;
    float windowHeight = ImGui::GetIO().DisplaySize.y;
    float contentWidth = 1000.0f;
    float contentStartX = (windowWidth - contentWidth) / 2.0f;

    // Top bar
    ImGui::Dummy(ImVec2(0, 28.0f));
    ImGui::SetCursorPosX(contentStartX);

    ImGui::SetWindowFontScale(1.5f);
    ImGui::TextColored(ImVec4(0.22f, 0.48f, 0.78f, 1.0f), "五笔编码查询");
    ImGui::SetWindowFontScale(1.0f);

    float titleWidth = ImGui::GetItemRectSize().x;
    float labelX = contentStartX + titleWidth + 12.0f;

    ImGui::SameLine();
    ImGui::SetCursorPosX(labelX);
    ImGui::TextColored(ImVec4(0.70f, 0.72f, 0.76f, 1.0f), "|");

    float sepWidth = ImGui::GetItemRectSize().x;
    float dataX = labelX + sepWidth + 12.0f;

    ImGui::SameLine();
    ImGui::SetCursorPosX(dataX);
    if (m_dataInitialized) {
        char dataText[128];
        snprintf(dataText, sizeof(dataText), "%zu 个汉字已加载", m_dataManager->getCharCount());
        ImGui::TextColored(ImVec4(0.48f, 0.50f, 0.54f, 1.0f), "%s", dataText);
    } else {
        ImGui::TextColored(ImVec4(0.85f, 0.45f, 0.15f, 1.0f), "数据加载中...");
    }

    ImGui::SameLine();
    float escTextWidth = ImGui::CalcTextSize("ESC 退出").x;
    ImGui::SetCursorPosX(windowWidth - escTextWidth - 24.0f);
    ImGui::TextColored(ImVec4(0.60f, 0.62f, 0.66f, 1.0f), "ESC 退出");

    ImGui::Dummy(ImVec2(0, 12.0f));

    // Search card
    ImGui::SetCursorPosX(contentStartX);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.80f, 0.82f, 0.86f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 16.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

    if (ImGui::BeginChild("##SearchCard", ImVec2(contentWidth, 100.0f), true,
                          ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse)) {

        ImGui::SetWindowFontScale(0.90f);
        ImGui::TextColored(ImVec4(0.45f, 0.47f, 0.52f, 1.0f), "输入汉字查询五笔编码");
        ImGui::SetWindowFontScale(1.0f);

        ImGui::Dummy(ImVec2(0, 6.0f));

        static bool enterPressed = false;
        float searchInputWidth = contentWidth - 140.0f;
        ImGui::PushItemWidth(searchInputWidth);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.00f, 1.00f, 1.00f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.00f, 1.00f, 1.00f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.00f, 1.00f, 1.00f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.55f, 0.58f, 0.63f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.00f, 0.00f, 0.00f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, ImVec4(0.28f, 0.56f, 0.88f, 0.35f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 10.0f));

        if (ImGui::InputText("##SearchInput", m_searchInput, sizeof(m_searchInput),
                             ImGuiInputTextFlags_EnterReturnsTrue)) {
            enterPressed = true;
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(6);
        ImGui::PopItemWidth();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.28f, 0.58f, 0.90f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.22f, 0.50f, 0.82f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.18f, 0.44f, 0.74f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(24.0f, 10.0f));

        if (ImGui::Button(" 查询 ", ImVec2(0, 0)) || enterPressed) {
            performSearch(std::string(m_searchInput));
            enterPressed = false;
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);

        ImGui::Dummy(ImVec2(0, 6.0f));
        ImGui::TextColored(ImVec4(0.55f, 0.57f, 0.62f, 1.0f), "支持输入单个或多个汉字，如 \"五\"、\"北京\" 等");
    }
    ImGui::EndChild();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);

    ImGui::Dummy(ImVec2(0, 12.0f));

    // Data loading state
    if (!m_dataInitialized) {
        ImGui::Dummy(ImVec2(0, 40.0f));
        ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("数据正在加载中...").x) / 2);
        ImGui::TextColored(ImVec4(0.85f, 0.45f, 0.15f, 1.0f), "数据正在加载中...");
        ImGui::End();
        return;
    }

    // Results section
    if (!m_searchResults.empty()) {
        char resultText[256];
        snprintf(resultText, sizeof(resultText), "查询结果: 共 %zu 个汉字", m_searchResults.size());
        ImGui::SetCursorPosX(contentStartX);
        ImGui::TextColored(ImVec4(0.38f, 0.40f, 0.44f, 1.0f), "%s", resultText);

        ImGui::Dummy(ImVec2(0, 8.0f));
        ImGui::SetCursorPosX(contentStartX);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.80f, 0.82f, 0.86f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

        float tableWidth = contentWidth;
        float tableHeight = windowHeight - ImGui::GetCursorPosY() - 32.0f;

        if (ImGui::BeginTable("##ResultsTable", 6,
                              ImGuiTableFlags_BordersInnerV |
                              ImGuiTableFlags_RowBg |
                              ImGuiTableFlags_ScrollY |
                              ImGuiTableFlags_SizingFixedFit |
                              ImGuiTableFlags_NoHostExtendX,
                              ImVec2(tableWidth, tableHeight))) {

            ImGui::TableSetupColumn("序号", ImGuiTableColumnFlags_WidthFixed, 50.0f);
            ImGui::TableSetupColumn("汉字", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableSetupColumn("基本信息", ImGuiTableColumnFlags_WidthStretch, 0.20f);
            ImGui::TableSetupColumn("五笔全码", ImGuiTableColumnFlags_WidthFixed, 90.0f);
            ImGui::TableSetupColumn("简码/容错码", ImGuiTableColumnFlags_WidthStretch, 0.15f);
            ImGui::TableSetupColumn("字根拆解", ImGuiTableColumnFlags_WidthStretch, 0.45f);

            // Custom centered header row
            ImGui::TableNextRow();
            for (int col = 0; col < 6; ++col) {
                ImGui::TableSetColumnIndex(col);
                const char* headerName = ImGui::TableGetColumnName(col);
                ImVec2 textSize = ImGui::CalcTextSize(headerName);
                float colWidth = ImGui::GetContentRegionAvail().x;
                float padX = (colWidth - textSize.x) / 2.0f;
                if (padX > 0) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padX);
                ImGui::TextUnformatted(headerName);
            }

            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 4.0f));

            for (size_t i = 0; i < m_searchResults.size(); ++i) {
                const auto* charData = m_searchResults[i];
                if (!charData) continue;

                ImGui::TableNextRow(0, 80.0f);

                ImGui::TableSetColumnIndex(0);
                CenteredTextVert(ImVec4(0.55f, 0.57f, 0.62f, 1.0f), std::to_string(i + 1).c_str(), 80.0f);

                ImGui::TableSetColumnIndex(1);
                {
                    ImGui::SetWindowFontScale(1.8f);
                    CenteredTextVert(ImVec4(0.16f, 0.17f, 0.20f, 1.0f), charData->character.c_str(), 80.0f);
                    ImGui::SetWindowFontScale(1.0f);
                }

                ImGui::TableSetColumnIndex(2);
                if (!charData->pinyin.empty() || !charData->strokes.empty()) {
                    char infoText[256];
                    snprintf(infoText, sizeof(infoText), "拼音: %s  笔画: %s",
                            charData->pinyin.empty() ? "-" : charData->pinyin.c_str(),
                            charData->strokes.empty() ? "-" : charData->strokes.c_str());
                    CenteredTextVert(ImVec4(0.38f, 0.40f, 0.44f, 1.0f), infoText, 80.0f);
                } else {
                    CenteredTextVert(ImVec4(0.60f, 0.62f, 0.66f, 1.0f), "-", 80.0f);
                }

                ImGui::TableSetColumnIndex(3);
                if (!charData->wubi_code.empty()) {
                    CenteredTextVert(ImVec4(0.22f, 0.48f, 0.78f, 1.0f), charData->wubi_code.c_str(), 80.0f);
                } else {
                    CenteredTextVert(ImVec4(0.60f, 0.62f, 0.66f, 1.0f), "-", 80.0f);
                }

                ImGui::TableSetColumnIndex(4);
                if (!charData->simple_codes.empty()) {
                    std::string simpleCodes;
                    for (size_t j = 0; j < charData->simple_codes.size(); ++j) {
                        if (j > 0) simpleCodes += ", ";
                        simpleCodes += charData->simple_codes[j];
                    }
                    CenteredTextVert(ImVec4(0.38f, 0.40f, 0.44f, 1.0f), simpleCodes.c_str(), 80.0f);
                } else {
                    CenteredTextVert(ImVec4(0.60f, 0.62f, 0.66f, 1.0f), "-", 80.0f);
                }

                ImGui::TableSetColumnIndex(5);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
                if (charData->has_image && m_imageLoader) {
                    ImageTexture* texture = m_imageLoader->loadImage(charData->image_path);
                    if (texture && texture->loaded) {
                        float targetHeight = 60.0f;
                        float aspectRatio = (float)texture->width / (float)texture->height;
                        float calculatedWidth = targetHeight * aspectRatio;
                        float maxWidth = 280.0f;
                        if (calculatedWidth > maxWidth) {
                            calculatedWidth = maxWidth;
                        }
                        m_imageLoader->renderImage(texture, calculatedWidth, targetHeight);
                    } else {
                        ImGui::TextColored(ImVec4(0.85f, 0.30f, 0.30f, 1.0f), "图片加载失败");
                    }
                } else {
                    ImGui::TextColored(ImVec4(0.60f, 0.62f, 0.66f, 1.0f), "无");
                }
            }

            ImGui::PopStyleVar();
            ImGui::EndTable();
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(2);

    } else if (strlen(m_searchInput) > 0) {
        ImGui::Dummy(ImVec2(0, 40.0f));
        ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("未找到匹配的汉字编码").x) / 2);
        ImGui::TextColored(ImVec4(0.55f, 0.57f, 0.62f, 1.0f), "未找到匹配的汉字编码");
    }

    ImGui::End();
}

void Application::performSearch(const std::string& input)
{
    if (!m_dataManager || !m_dataInitialized) {
        std::cerr << "数据管理器未初始化" << std::endl;
        return;
    }

    if (input.empty()) {
        m_searchResults.clear();
        return;
    }

    // 执行查询
    m_searchResults = m_dataManager->query(input);

    std::cout << "查询 '" << input << "': 找到 " << m_searchResults.size() << " 个结果" << std::endl;
}

bool Application::renderCharacterImage(const wubi::CharData* charData)
{
    if (!charData || !charData->has_image) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "无图片");
        return false;
    }

    if (!m_imageLoader) {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "图片加载器未初始化");
        return false;
    }

    // 尝试加载并渲染图片
    ImageTexture* texture = m_imageLoader->getImage(charData->image_path);
    if (!texture) {
        // 如果图片还未加载，尝试加载
        texture = m_imageLoader->loadImage(charData->image_path);
    }

    if (texture) {
        // 渲染实际图片
        bool success = m_imageLoader->renderImage(texture, 120.0f, 120.0f);
        if (success) {
            // 显示图片路径作为工具提示
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("字根图片: %s\n图片路径: %s",
                                 charData->character.c_str(),
                                 charData->image_path.c_str());
            }
            return true;
        }
    }

    // 如果图片加载失败，显示错误信息
    ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "[图片加载失败]");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("无法加载图片: %s", charData->image_path.c_str());
    }

    return false;
}