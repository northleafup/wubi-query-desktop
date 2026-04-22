## ADDED Requirements

### Requirement: Window icon loading
应用程序 SHALL 在窗口创建后加载图标文件并调用 `glfwSetWindowIcon()` 设置窗口图标。

#### Scenario: Icon loaded successfully
- **WHEN** 应用程序启动并创建窗口
- **THEN** 加载 `assets/icons/app_32x32.png` 和 `assets/icons/app_256x256.png`
- **AND** 调用 `glfwSetWindowIcon(window, 2, images)`

#### Scenario: Icon file missing
- **WHEN** 图标文件不存在
- **THEN** 打印警告信息到 stderr
- **AND** 程序继续运行（不崩溃）
- **AND** 使用系统默认窗口图标

### Requirement: Multi-size icon support
应用程序 SHALL 提供至少两个尺寸的图标数据给 GLFW，以便系统选择最合适的尺寸。

#### Scenario: Two sizes provided
- **WHEN** 调用 `glfwSetWindowIcon`
- **THEN** 传入 32x32 和 256x256 两个 `GLFWimage` 结构
- **AND** 像素数据格式为 RGBA（4 通道）

### Requirement: Resource cleanup
应用程序 SHALL 在图标设置完成后释放像素数据内存。

#### Scenario: Memory freed
- **WHEN** `glfwSetWindowIcon()` 返回后
- **THEN** 调用 `stbi_image_free()` 释放每个图标的像素数据
