## Context

桌面环境（如 GNOME）通过 `WM_CLASS` (X11) 或 `App ID` (Wayland) 属性来识别窗口，并将其与 `.desktop` 文件中的 `StartupWMClass` 字段进行匹配。
当前应用未设置这些属性，导致桌面环境无法关联窗口，从而错误地将窗口标题（可能被误解码）作为侧边栏显示名称。

## Goals / Non-Goals

**Goals:**
- 在 C++ 代码中设置 GLFW 窗口提示，使窗口在 X11 和 Wayland 下具有明确的类名。
- 在打包脚本生成的 `.desktop` 文件中包含 `StartupWMClass`，引导桌面环境正确匹配。

**Non-Goals:**
- 不修改应用内部显示的标题逻辑。
- 不涉及 macOS (AppKit) 的 Bundle 标识符修改（由 Info.plist 控制）。

## Decisions

1. **使用 `glfwWindowHintString` 设置类名**：
   - 使用 `GLFW_X11_CLASS_NAME` 和 `GLFW_X11_INSTANCE_NAME` 显式指定 X11 下的类名。
   - 使用 `GLFW_WAYLAND_APP_ID` 显式指定 Wayland 下的 App ID。
   - 类名统一设定为 `wubi-query-desktop`（kebab-case），与项目名称一致。

2. **在 `.desktop` 文件中声明 `StartupWMClass`**：
   - 添加 `StartupWMClass=wubi-query-desktop` 字段。
   - 该字段告诉桌面环境：“当看到一个 `WM_CLASS` 为 `wubi-query-desktop` 的窗口时，请使用此文件定义的图标和名称”。

3. **顺序要求**：
   - `glfwWindowHint` 必须在 `glfwCreateWindow` 之前调用。

## Risks / Trade-offs

- [窗口类名与旧版本不兼容] → 无旧版本遗留问题，直接设定新类名。
- [GLFW 版本支持] → `glfwWindowHintString` 需要 GLFW 3.2+，项目已使用 GLFW 3.x，支持良好。
