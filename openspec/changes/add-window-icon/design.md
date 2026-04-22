## Context

当前 `Application::createWindow()` 调用 `glfwCreateWindow()` 后未设置窗口图标。GLFW 在 X11/Wayland 下会使用默认图标（通常为齿轮）。桌面环境的边栏/Dock 读取窗口图标而非 `.desktop` 文件，导致用户看到错误的图标。

项目已有 `stb_image.h` 和 `ImageLoader.cpp` 使用它加载图片，可以直接复用。图标文件 `assets/icons/app_*.png` 已存在多尺寸版本。

## Goals / Non-Goals

**Goals:**
- 在窗口创建后立即调用 `glfwSetWindowIcon()` 设置应用图标
- 复用 `stb_image.h` 加载 PNG，不引入新依赖
- 加载 32x32 和 256x256 两个尺寸（GLFW 推荐做法）

**Non-Goals:**
- 不修改图标文件本身（已在上一个变更中生成）
- 不处理 macOS Bundle 图标（由 .icns 文件控制，非 GLFW 职责）
- 不处理 Windows .ico 图标（当前聚焦 Linux DEB 包）

## Decisions

1. **在 `createWindow()` 末尾设置图标**：图标设置必须在 `glfwCreateWindow()` 之后、窗口显示之前。这是 GLFW 文档推荐的位置。

2. **加载两个尺寸 (32x32 + 256x256)**：GLFW 支持传入多个 `GLFWimage` 结构，系统会选择最合适的尺寸。32x32 用于小图标场景，256x256 用于高分辨率显示。

3. **使用 stb_image 直接加载，不依赖 ImageLoader**：`ImageLoader` 创建 OpenGL 纹理，而窗口图标需要原始 RGBA 像素数据。直接使用 `stbi_load()` 更简洁，无需额外封装。

4. **图标路径使用相对路径 "assets/icons/"**：与项目现有资源加载方式一致，程序从可执行文件相对路径查找。

## Risks / Trade-offs

- [图标文件不存在时程序不应崩溃] → 加载失败时打印警告并继续，不调用 `glfwSetWindowIcon`
- [stb_image 的 `stbi_set_flip_vertically_on_load` 影响] → GLFW 图标数据不需要翻转，确保加载前未设置翻转标志（默认不翻转，安全）
- [内存泄漏风险] → 使用 `stbi_image_free()` 及时释放像素数据
