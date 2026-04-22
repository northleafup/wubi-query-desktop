#include "data_manager.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>

namespace wubi {

DataManager::DataManager(const std::string& assets_path)
    : assets_path_(assets_path), data_loaded_(false) {
    parser_ = std::make_unique<WubiDataParser>();
}

bool DataManager::initialize() {
    // 获取当前工作目录并构建正确的assets路径
    char current_path[FILENAME_MAX];
    std::string resolved_assets_path = assets_path_;

    if (getcwd(current_path, sizeof(current_path))) {
        std::string current_dir(current_path);

        // 检查环境变量中是否指定了资源路径
        const char* env_assets = std::getenv("WUBI_ASSETS_PATH");
        if (env_assets && std::filesystem::exists(env_assets)) {
            resolved_assets_path = env_assets;
        }
        // 检查是否为DEB安装环境 - 检查系统资源目录是否存在
        else if (std::filesystem::exists("/usr/share/wubi-query-desktop/assets")) {
            resolved_assets_path = "/usr/share/wubi-query-desktop/assets";
        }
        // 如果在build目录中，需要回到项目根目录
        else if (current_dir.find("/build") != std::string::npos) {
            resolved_assets_path = current_dir.substr(0, current_dir.find("/build")) + "/assets";
        }
        // 如果在项目根目录中，保持原路径
        else {
            resolved_assets_path = assets_path_;
        }
    }

    std::cout << "Initializing DataManager with assets path: " << resolved_assets_path << std::endl;

    // 保存解析后的绝对路径
    resolved_assets_path_ = resolved_assets_path;

    // 构建数据文件路径
    std::string wubi_data_path = resolved_assets_path + "/data/data-wubi-v86.tsv";
    std::string char_data_path = resolved_assets_path + "/data/data-chars.tsv";

    std::cout << "Trying to open: " << wubi_data_path << std::endl;

    // 加载五笔编码数据
    if (!parser_->loadWubiData(wubi_data_path)) {
        std::cerr << "Failed to load wubi data from: " << wubi_data_path << std::endl;
        return false;
    }

    // 加载汉字基本信息数据
    if (!parser_->loadCharData(char_data_path)) {
        std::cerr << "Failed to load character data from: " << char_data_path << std::endl;
        return false;
    }

    // 扫描可用的字根图片
    if (!scanAvailableImages()) {
        std::cerr << "Failed to scan available images" << std::endl;
        return false;
    }

    // 过滤数据，只保留有图片的汉字
    (void)parser_->filterDataWithImages(available_images_);

    // 更新保留汉字数据的图片信息
    auto& all_chars = parser_->getAllCharData();
    for (const auto& pair : all_chars) {
        const std::string& character = pair.first;
        CharData* char_data = pair.second.get();

        // 现在所有保留的汉字都应该有图片
        char_data->has_image = true;
        char_data->image_path = getImagePath(character);
    }

    data_loaded_ = true;

    std::cout << "DataManager initialized successfully!" << std::endl;
    std::cout << "五笔数据加载完成: " << getCharCount() << " 个汉字（已过滤为有图片的）" << std::endl;
    std::cout << "字根图片数量: " << getImageCount() << " 张" << std::endl;
    std::cout << "所有汉字都有对应的字根拆解图片！" << std::endl;

    return true;
}

std::vector<CharData*> DataManager::query(const std::string& input) {
    if (!data_loaded_) {
        std::cerr << "DataManager not initialized" << std::endl;
        return {};
    }

    return parser_->queryByString(input);
}

CharData* DataManager::queryByCharacter(const std::string& character) {
    if (!data_loaded_) {
        std::cerr << "DataManager not initialized" << std::endl;
        return nullptr;
    }

    return parser_->queryByCharacter(character);
}

bool DataManager::hasImageData(const std::string& character) const {
    return available_images_.find(character) != available_images_.end();
}

std::string DataManager::getImagePath(const std::string& character) const {
    if (!hasImageData(character)) {
        return "";
    }

    return resolved_assets_path_ + "/images/char/" + character + ".gif";
}

size_t DataManager::getCharCount() const {
    if (!parser_) {
        return 0;
    }
    return parser_->getCharCount();
}

size_t DataManager::getImageCount() const {
    return available_images_.size();
}

bool DataManager::scanAvailableImages() {
    // 使用解析后的assets路径，与initialize()保持一致
    std::string images_dir;

    // 获取当前工作目录并构建正确的图片路径
    char current_path[FILENAME_MAX];
    if (getcwd(current_path, sizeof(current_path))) {
        std::string current_dir(current_path);

        // 检查环境变量中是否指定了资源路径
        const char* env_assets = std::getenv("WUBI_ASSETS_PATH");
        if (env_assets && std::filesystem::exists(std::string(env_assets) + "/images/char")) {
            images_dir = std::string(env_assets) + "/images/char";
        }
        // 检查是否为DEB安装环境 - 检查系统资源目录是否存在
        else if (std::filesystem::exists("/usr/share/wubi-query-desktop/assets/images/char")) {
            images_dir = "/usr/share/wubi-query-desktop/assets/images/char";
        }
        // 如果在build目录中，需要回到项目根目录
        else if (current_dir.find("/build") != std::string::npos) {
            images_dir = current_dir.substr(0, current_dir.find("/build")) + "/assets/images/char";
        }
        // 如果在项目根目录中，保持原路径
        else {
            images_dir = resolved_assets_path_ + "/images/char";
        }
    } else {
        images_dir = resolved_assets_path_ + "/images/char";
    }

    if (!std::filesystem::exists(images_dir)) {
        std::cerr << "Images directory does not exist: " << images_dir << std::endl;
        return false;
    }

    if (!std::filesystem::is_directory(images_dir)) {
        std::cerr << "Images path is not a directory: " << images_dir << std::endl;
        return false;
    }

    try {
        size_t count = 0;
        for (const auto& entry : std::filesystem::directory_iterator(images_dir)) {
            if (entry.is_regular_file()) {
                const std::string filename = entry.path().filename().string();

                // 检查是否为GIF文件
                if (filename.length() >= 5 && filename.substr(filename.length() - 4) == ".gif") {
                    // 提取汉字字符（去掉.gif后缀）
                    std::string character = filename.substr(0, filename.length() - 4);

                    if (!character.empty()) {
                        available_images_.insert(character);
                        count++;
                    }
                }
            }
        }

        std::cout << "Scanned " << count << " image files in: " << images_dir << std::endl;
        return true;

    } catch (const std::filesystem::filesystem_error& ex) {
        std::cerr << "Filesystem error scanning images: " << ex.what() << std::endl;
        return false;
    }
}

bool DataManager::fileExists(const std::string& filepath) const {
    return std::filesystem::exists(filepath) && std::filesystem::is_regular_file(filepath);
}

} // namespace wubi