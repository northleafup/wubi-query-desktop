#pragma once

#include <string>
#include <unordered_map>
#include <memory>

// 简单的图片纹理结构
struct ImageTexture {
    unsigned int textureId = 0;
    int width = 0;
    int height = 0;
    bool loaded = false;

    ImageTexture() = default;
    ~ImageTexture();

    // 禁用拷贝，允许移动
    ImageTexture(const ImageTexture&) = delete;
    ImageTexture& operator=(const ImageTexture&) = delete;
    ImageTexture(ImageTexture&& other) noexcept;
    ImageTexture& operator=(ImageTexture&& other) noexcept;
};

/**
 * @brief 图片加载器
 * 负责加载和管理GIF图片，转换为OpenGL纹理
 */
class ImageLoader {
public:
    ImageLoader() = default;
    ~ImageLoader() = default;

    // 禁用拷贝
    ImageLoader(const ImageLoader&) = delete;
    ImageLoader& operator=(const ImageLoader&) = delete;

    /**
     * @brief 加载图片文件
     * @param filepath 图片文件路径
     * @return 图片纹理指针，加载失败返回nullptr
     */
    ImageTexture* loadImage(const std::string& filepath);

    /**
     * @brief 获取已加载的图片
     * @param filepath 图片文件路径
     * @return 图片纹理指针，未找到返回nullptr
     */
    ImageTexture* getImage(const std::string& filepath);

    /**
     * @brief 在ImGui中渲染图片
     * @param texture 图片纹理
     * @param width 显示宽度
     * @param height 显示高度
     * @return 是否成功渲染
     */
    bool renderImage(ImageTexture* texture, float width = 160.0f, float height = 160.0f);

    /**
     * @brief 清理所有已加载的图片
     */
    void clearCache();

private:
    std::unordered_map<std::string, std::unique_ptr<ImageTexture>> textureCache_;

    /**
     * @brief 从文件加载图片数据
     * @param filepath 文件路径
     * @param width 输出图片宽度
     * @param height 输出图片高度
     * @param channels 输出图片通道数
     * @return 图片数据指针，需要调用者释放
     */
    unsigned char* loadImageFromFile(const std::string& filepath, int& width, int& height, int& channels);

    /**
     * @brief 创建OpenGL纹理
     * @param data 图片数据
     * @param width 图片宽度
     * @param height 图片高度
     * @param channels 图片通道数
     * @return OpenGL纹理ID
     */
    unsigned int createTexture(unsigned char* data, int width, int height, int channels);
};