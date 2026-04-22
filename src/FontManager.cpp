#include "FontManager.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <filesystem>

// 中文字符范围定义
static const ImWchar chinese_ranges[] = {
    0x0020, 0x00FF,    // 基本拉丁字母
    0x3000, 0x30FF,    // CJK符号和假名
    0x3100, 0x312F,    // 注音字母
    0x3200, 0x32FF,    // 中文符号
    0x4E00, 0x9FFF,    // CJK统一汉字 (最重要)
    0xF900, 0xFAFF,    // CJK兼容汉字
    0xFE30, 0xFE4F,    // CJK兼容形式
    0xFF00, 0xFFEF,    // 全角字符
    0x2000, 0x206F,    // 常用标点符号
    0
};

FontManager::FontManager()
    : m_initialized(false)
    , m_defaultFont(nullptr)
    , m_largeFont(nullptr)
{
}

FontManager::~FontManager()
{
    // ImGui会自动管理字体内存，不需要手动释放
}

bool FontManager::initialize()
{
    if (m_initialized) {
        return true;
    }

    std::cout << "正在初始化字体系统..." << std::endl;

    if (!loadChineseFont()) {
        std::cerr << "中文字体加载失败，尝试使用备用字体..." << std::endl;
        if (!loadFallbackFont()) {
            std::cerr << "字体系统初始化失败！" << std::endl;
            return false;
        }
    }

    m_initialized = true;
    std::cout << "字体系统初始化成功" << std::endl;
    return true;
}

ImFont* FontManager::getDefaultFont() const
{
    return m_defaultFont ? m_defaultFont : ImGui::GetIO().Fonts->Fonts[0];
}

ImFont* FontManager::getLargeFont() const
{
    return m_largeFont ? m_largeFont : getDefaultFont();
}

bool FontManager::isChineseCharacter(char c)
{
    // 单字节字符不可能是中文
    return (unsigned char)c > 127;
}

bool FontManager::containsChinese(const std::string& str)
{
    return std::any_of(str.begin(), str.end(), [](char c) {
        return isChineseCharacter(c);
    });
}

bool FontManager::isInitialized() const
{
    return m_initialized;
}

bool FontManager::reloadFonts()
{
    if (!m_initialized) {
        return initialize();
    }

    // 重建字体图集
    ImGui::GetIO().Fonts->Clear();
    return loadChineseFont();
}

bool FontManager::loadChineseFont()
{
    auto& io = ImGui::GetIO();
    ImFontAtlas* fonts = io.Fonts;

    // 获取中文字体路径
    std::string fontPath = getChineseFontPath();

    // 检查字体文件是否存在
    if (!std::ifstream(fontPath).good()) {
        std::cerr << "字体文件不存在: " << fontPath << std::endl;
        return false;
    }

    std::cout << "加载中文字体: " << fontPath << std::endl;

    // 配置字体合并处理器
    static ImFontConfig fontConfig;
    fontConfig.MergeMode = false;
    fontConfig.PixelSnapH = true;
    fontConfig.OversampleH = 1;
    fontConfig.OversampleV = 1;

    // 加载默认字体 (16px)
    m_defaultFont = fonts->AddFontFromFileTTF(
        fontPath.c_str(),
        16.0f,                    // 字体大小
        &fontConfig,
        chinese_ranges
    );

    if (!m_defaultFont) {
        std::cerr << "默认字体加载失败" << std::endl;
        return false;
    }

    // 加载大号字体 (24px)
    fontConfig.MergeMode = false;  // 不使用合并模式，独立字体
    fontConfig.PixelSnapH = true;
    fontConfig.OversampleH = 1;
    fontConfig.OversampleV = 1;
    m_largeFont = fonts->AddFontFromFileTTF(
        fontPath.c_str(),
        24.0f,                    // 大号字体大小
        &fontConfig,
        chinese_ranges
    );

    if (!m_largeFont) {
        std::cerr << "大号字体加载失败，将使用默认字体" << std::endl;
        m_largeFont = m_defaultFont;
    }

    // 构建字体图集
    bool atlasResult = fonts->Build();
    if (!atlasResult) {
        std::cerr << "字体图集构建失败" << std::endl;
        return false;
    }

    std::cout << "中文字体加载成功" << std::endl;
    return true;
}

ImVector<ImWchar> FontManager::setupChineseRanges()
{
    ImVector<ImWchar> ranges;
    for (int i = 0; chinese_ranges[i] != 0; i += 2) {
        ranges.push_back(chinese_ranges[i]);
        ranges.push_back(chinese_ranges[i + 1]);
    }
    return ranges;
}

std::string FontManager::getChineseFontPath() const
{
    // 获取当前工作目录，然后构建字体文件路径
    char current_path[FILENAME_MAX];
    std::string base_dir = "";

    if (getcwd(current_path, sizeof(current_path))) {
        std::string current_dir(current_path);

        // 检查环境变量中是否指定了资源路径
        const char* env_assets = std::getenv("WUBI_ASSETS_PATH");
        if (env_assets && std::filesystem::exists(std::string(env_assets) + "/fonts")) {
            base_dir = std::string(env_assets) + "/fonts/";
        }
        // 检查是否为DEB安装环境 - 检查系统字体目录是否存在
        else if (std::filesystem::exists("/usr/share/wubi-query-desktop/assets/fonts")) {
            base_dir = "/usr/share/wubi-query-desktop/assets/fonts/";
        }
        // 如果在build/bin目录中，需要回到项目根目录
        else if (current_dir.find("/build/bin") != std::string::npos) {
            base_dir = current_dir.substr(0, current_dir.find("/build/bin")) + "/assets/fonts/";
        }
        // 如果在build目录中
        else if (current_dir.find("/build") != std::string::npos) {
            base_dir = current_dir.substr(0, current_dir.find("/build")) + "/assets/fonts/";
        }
        // 如果在项目根目录中
        else {
            base_dir = "assets/fonts/";
        }
    } else {
        base_dir = "assets/fonts/";
    }

    // 字体文件搜索路径
    std::vector<std::string> fontPaths = {
        base_dir + "wqy-zenhei.ttc",            // 文泉驿正黑（首选）
        base_dir + "wqy-microhei.ttc",          // 文泉驿微米黑
        base_dir + "DejaVuSans.ttf",            // DejaVu备用字体
        "./wqy-zenhei.ttc",                    // 当前目录（构建时复制）
        "./wqy-microhei.ttc",
        "./DejaVuSans.ttf"
    };

    for (const auto& path : fontPaths) {
        if (std::ifstream(path).good()) {
            std::cout << "找到字体文件: " << path << std::endl;
            return path;
        }
    }

    std::cerr << "未找到合适的字体文件" << std::endl;
    return "";
}

bool FontManager::loadFallbackFont()
{
    auto& io = ImGui::GetIO();
    ImFontAtlas* fonts = io.Fonts;

    std::cout << "使用系统默认字体作为备用字体" << std::endl;

    // 使用ImGui默认字体，但添加基本的中文字符范围
    static ImFontConfig fallbackConfig;
    fallbackConfig.SizePixels = 16.0f;

    // 尝试加载系统字体
    m_defaultFont = fonts->AddFontDefault(&fallbackConfig);

    if (!m_defaultFont) {
        std::cerr << "默认字体加载失败" << std::endl;
        return false;
    }

    m_largeFont = m_defaultFont;

    // 构建字体图集
    bool atlasResult = fonts->Build();
    if (!atlasResult) {
        std::cerr << "备用字体图集构建失败" << std::endl;
        return false;
    }

    std::cout << "备用字体加载成功（可能无法正确显示中文）" << std::endl;
    return true;
}