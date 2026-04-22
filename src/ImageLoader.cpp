#include "ImageLoader.h"
#include <iostream>
#include <fstream>
#include <vector>
#ifdef PLATFORM_MACOS
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <imgui.h>

// 使用stb_image库进行真正的图片加载
#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"

ImageTexture::~ImageTexture() {
    if (loaded && textureId != 0) {
        glDeleteTextures(1, &textureId);
        textureId = 0;
    }
}

ImageTexture::ImageTexture(ImageTexture&& other) noexcept
    : textureId(other.textureId)
    , width(other.width)
    , height(other.height)
    , loaded(other.loaded)
{
    other.textureId = 0;
    other.loaded = false;
}

ImageTexture& ImageTexture::operator=(ImageTexture&& other) noexcept {
    if (this != &other) {
        if (loaded && textureId != 0) {
            glDeleteTextures(1, &textureId);
        }

        textureId = other.textureId;
        width = other.width;
        height = other.height;
        loaded = other.loaded;

        other.textureId = 0;
        other.loaded = false;
    }
    return *this;
}

ImageTexture* ImageLoader::loadImage(const std::string& filepath) {
    // 检查缓存
    auto it = textureCache_.find(filepath);
    if (it != textureCache_.end()) {
        return it->second.get();
    }

    // 创建新的纹理对象
    auto texture = std::make_unique<ImageTexture>();

    int width = 0, height = 0, channels = 0;
    unsigned char* data = loadImageFromFile(filepath, width, height, channels);

    if (!data) {
        std::cerr << "Failed to load image: " << filepath << std::endl;
        return nullptr;
    }

    texture->textureId = createTexture(data, width, height, channels);
    texture->width = width;
    texture->height = height;
    texture->loaded = (texture->textureId != 0);

    // 释放图片数据 - 使用stb_image的释放函数
    stbi_image_free(data);

    if (!texture->loaded) {
        std::cerr << "Failed to create texture for: " << filepath << std::endl;
        return nullptr;
    }

    // 缓存纹理
    ImageTexture* result = texture.get();
    textureCache_[filepath] = std::move(texture);

    std::cout << "Successfully loaded image: " << filepath
              << " (" << width << "x" << height << ")" << std::endl;

    return result;
}

ImageTexture* ImageLoader::getImage(const std::string& filepath) {
    auto it = textureCache_.find(filepath);
    return (it != textureCache_.end()) ? it->second.get() : nullptr;
}

bool ImageLoader::renderImage(ImageTexture* texture, float width, float height) {
    if (!texture || !texture->loaded || texture->textureId == 0) {
        return false;
    }

    // 在ImGui中渲染纹理
    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture->textureId)),
                 ImVec2(width, height));
    return true;
}

void ImageLoader::clearCache() {
    textureCache_.clear();
}

unsigned char* ImageLoader::loadImageFromFile(const std::string& filepath, int& width, int& height, int& channels) {
    // 使用stb_image加载图片
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0); // 0表示自动检测通道数

    if (!data) {
        std::cerr << "Failed to load image with stb_image: " << filepath << std::endl;
        std::cerr << "stb_image error: " << stbi_failure_reason() << std::endl;
        return nullptr;
    }

    // stb_image默认加载为RGB，但OpenGL可能需要RGBA
    // 如果channels为3(RGB)，我们转换为4(RGBA)
    if (channels == 3) {
        unsigned char* rgba_data = new unsigned char[width * height * 4];
        for (int i = 0; i < width * height; i++) {
            rgba_data[i * 4 + 0] = data[i * 3 + 0]; // R
            rgba_data[i * 4 + 1] = data[i * 3 + 1]; // G
            rgba_data[i * 4 + 2] = data[i * 3 + 2]; // B
            rgba_data[i * 4 + 3] = 255;              // A
        }
        stbi_image_free(data); // 释放原始数据
        data = rgba_data;
        channels = 4;
    }

    std::cout << "Successfully loaded real image: " << filepath
              << " (" << width << "x" << height << ", " << channels << " channels)" << std::endl;

    return data;
}

unsigned int ImageLoader::createTexture(unsigned char* data, int width, int height, int channels) {
    GLuint textureId;
    glGenTextures(1, &textureId);

    if (textureId == 0) {
        std::cerr << "Failed to generate OpenGL texture" << std::endl;
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, textureId);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 根据通道数确定OpenGL格式
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    // 上传纹理数据
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    // 检查OpenGL错误
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error during texture creation: " << error << std::endl;
        glDeleteTextures(1, &textureId);
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureId;
}