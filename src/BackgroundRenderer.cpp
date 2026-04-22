#include "BackgroundRenderer.h"
#include <iostream>
#include <cmath>

BackgroundRenderer::BackgroundRenderer()
    : m_initialized(false)
    , m_currentPlatform(detectCurrentPlatform())
{
}

BackgroundRenderer::~BackgroundRenderer()
{
}

bool BackgroundRenderer::initialize()
{
    if (m_initialized) {
        return true;
    }

    std::cout << "正在初始化背景渲染器..." << std::endl;
    m_initialized = true;
    return true;
}

void BackgroundRenderer::render(Platform platform)
{
    if (!m_initialized) {
        std::cerr << "背景渲染器未初始化" << std::endl;
        return;
    }

    // 获取窗口尺寸
    ImGuiIO& io = ImGui::GetIO();
    float windowWidth = io.DisplaySize.x;
    float windowHeight = io.DisplaySize.y;

    // 设置ImGui窗口覆盖整个屏幕
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    // 开始渲染背景窗口
    ImGui::Begin("Background", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoScrollWithMouse |
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoBackground |
                 ImGuiWindowFlags_NoBringToFrontOnFocus);

    // 根据平台渲染不同的背景
    switch (platform) {
        case Platform::LINUX:
            renderLinuxBackground();
            break;
        case Platform::MACOS:
            renderMacOSBackground();
            break;
        case Platform::WINDOWS:
            renderWindowsBackground();
            break;
        default:
            renderLinuxBackground();  // 默认使用Linux背景
            break;
    }

    ImGui::End();
    ImGui::PopStyleVar(2);
}

void BackgroundRenderer::setCurrentPlatform(Platform platform)
{
    m_currentPlatform = platform;
}

BackgroundRenderer::Platform BackgroundRenderer::getCurrentPlatform() const
{
    return m_currentPlatform;
}

std::string BackgroundRenderer::getPlatformName(Platform platform) const
{
    switch (platform) {
        case Platform::LINUX:
            return "Linux";
        case Platform::MACOS:
            return "macOS";
        case Platform::WINDOWS:
            return "Windows";
        default:
            return "Unknown";
    }
}

bool BackgroundRenderer::isInitialized() const
{
    return m_initialized;
}

void BackgroundRenderer::renderLinuxBackground()
{
    ImGuiIO& io = ImGui::GetIO();
    float windowWidth = io.DisplaySize.x;
    float windowHeight = io.DisplaySize.y;

    ImVec4 startColor = ImVec4(0.90f, 0.92f, 0.95f, 1.0f);
    ImVec4 endColor = ImVec4(0.86f, 0.89f, 0.93f, 1.0f);

    drawGradientBackground(startColor, endColor, windowWidth, windowHeight);
}

void BackgroundRenderer::renderMacOSBackground()
{
    ImGuiIO& io = ImGui::GetIO();
    float windowWidth = io.DisplaySize.x;
    float windowHeight = io.DisplaySize.y;

    ImVec4 startColor = ImVec4(0.90f, 0.92f, 0.95f, 1.0f);
    ImVec4 endColor = ImVec4(0.86f, 0.89f, 0.93f, 1.0f);

    drawGradientBackground(startColor, endColor, windowWidth, windowHeight);
}

void BackgroundRenderer::renderWindowsBackground()
{
    ImGuiIO& io = ImGui::GetIO();
    float windowWidth = io.DisplaySize.x;
    float windowHeight = io.DisplaySize.y;

    ImVec4 startColor = ImVec4(0.90f, 0.92f, 0.95f, 1.0f);
    ImVec4 endColor = ImVec4(0.86f, 0.89f, 0.93f, 1.0f);

    drawGradientBackground(startColor, endColor, windowWidth, windowHeight);
}

void BackgroundRenderer::drawGradientBackground(ImVec4 startColor, ImVec4 endColor,
                                               float width, float height)
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();

    // 创建渐变效果
    int gradientSteps = 50;
    for (int i = 0; i < gradientSteps; ++i) {
        float t = (float)i / (gradientSteps - 1);
        ImVec4 currentColor = ImVec4(
            startColor.x * (1.0f - t) + endColor.x * t,
            startColor.y * (1.0f - t) + endColor.y * t,
            startColor.z * (1.0f - t) + endColor.z * t,
            startColor.w * (1.0f - t) + endColor.w * t
        );

        float rectHeight = height / gradientSteps;
        float y = windowPos.y + i * rectHeight;

        drawList->AddRectFilled(
            ImVec2(windowPos.x, y),
            ImVec2(windowPos.x + width, y + rectHeight),
            ImGui::ColorConvertFloat4ToU32(currentColor)
        );
    }
}

void BackgroundRenderer::drawPatternBackground(ImVec4 color, float patternSize,
                                              float width, float height)
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImU32 patternColor = ImGui::ColorConvertFloat4ToU32(color);

    // 绘制网格图案
    for (float x = 0; x < width; x += patternSize) {
        for (float y = 0; y < height; y += patternSize) {
            if ((int)(x / patternSize + y / patternSize) % 2 == 0) {
                drawList->AddRectFilled(
                    ImVec2(windowPos.x + x, windowPos.y + y),
                    ImVec2(windowPos.x + x + patternSize, windowPos.y + y + patternSize),
                    patternColor
                );
            }
        }
    }
}

void BackgroundRenderer::drawPlatformLogo(const std::string& text, ImVec2 position,
                                         ImVec4 color, float size)
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 textPos = ImVec2(windowPos.x + position.x, windowPos.y + position.y);

    // 获取默认字体
    ImFont* font = ImGui::GetFont();
    if (!font) {
        return;
    }

    // 绘制文本阴影
    ImVec4 shadowColor = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
    ImVec2 shadowOffset = ImVec2(2.0f, 2.0f);
    drawList->AddText(font, size,
                     ImVec2(textPos.x + shadowOffset.x, textPos.y + shadowOffset.y),
                     ImGui::ColorConvertFloat4ToU32(shadowColor),
                     text.c_str());

    // 绘制主文本
    drawList->AddText(font, size, textPos,
                     ImGui::ColorConvertFloat4ToU32(color),
                     text.c_str());
}

BackgroundRenderer::Platform BackgroundRenderer::detectCurrentPlatform()
{
#ifdef PLATFORM_WINDOWS
    return Platform::WINDOWS;
#elif defined(PLATFORM_MACOS)
    return Platform::MACOS;
#else
    return Platform::LINUX;
#endif
}