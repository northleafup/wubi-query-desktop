## Why

DEB 包安装后，应用菜单图标已正确显示，但系统边栏/Dock 显示的是 GLFW 默认的齿轮图标。这是因为桌面环境对这两处使用不同的图标来源：应用菜单读取 `.desktop` 文件，而边栏/Dock 读取窗口图标。当前代码未调用 `glfwSetWindowIcon()` 设置窗口图标。

## What Changes

- 在 `Application::createWindow()` 中添加 `glfwSetWindowIcon()` 调用
- 使用项目已有的 `stb_image.h` 加载图标 PNG 文件
- 支持多尺寸图标（256x256 或 32x32 + 256x256）
- 图标文件复用 `assets/icons/` 目录下的现有资源

## Capabilities

### New Capabilities
- `window-icon`: 通过 `glfwSetWindowIcon` 设置窗口图标，使边栏/Dock 显示正确的应用图标

### Modified Capabilities
<!-- None -->

## Impact

- `src/Application.cpp`: 在 `createWindow()` 中添加图标加载和设置逻辑
- `assets/icons/`: 复用已有的 `app_*.png` 文件
- 无新增依赖（复用 `stb_image.h`）
- DEB 包安装后边栏/Dock 图标将与应用菜单图标一致
