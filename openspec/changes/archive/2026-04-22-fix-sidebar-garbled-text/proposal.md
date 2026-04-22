## Why

当前 DEB 包安装后，边栏/Dock 显示的标题乱码。这是因为桌面环境无法将运行中的 GLFW 窗口与 `.desktop` 文件正确关联。系统回退显示窗口标题，但由于编码上下文缺失导致乱码。我们需要通过设置窗口类名（WM_CLASS）来建立关联，使系统正确读取 `.desktop` 文件中的中文名称。

## What Changes

- **代码**：在 `Application::createWindow()` 中设置 GLFW 窗口类名 Hint (`GLFW_X11_CLASS_NAME`)，确保窗口元数据包含应用标识。
- **打包**：在 `build_ubuntu_deb.sh` 生成的 `.desktop` 文件中添加 `StartupWMClass` 字段，明确指定窗口类名匹配。

## Capabilities

### New Capabilities
- `window-class-matching`: 确保应用窗口具有正确的窗口类名，并与桌面文件匹配，解决标题显示乱码和图标不匹配问题。

### Modified Capabilities
<!-- None -->

## Impact

- `src/Application.cpp`: 修改 `createWindow()` 增加 X11/Wayland 窗口属性设置。
- `scripts/build_ubuntu_deb.sh`: 修改桌面文件生成逻辑，追加 `StartupWMClass` 行。
- 安装后需重新安装 DEB 包或清理缓存以生效。
