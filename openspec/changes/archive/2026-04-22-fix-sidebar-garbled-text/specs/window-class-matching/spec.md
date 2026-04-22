## ADDED Requirements

### Requirement: Window Class Name Setting
应用程序 SHALL 在创建窗口前设置 X11 窗口类名和 Wayland App ID，以便桌面环境识别窗口身份。

#### Scenario: X11 Environment
- **WHEN** 应用程序在 X11 环境下启动
- **THEN** 调用 `glfwWindowHintString(GLFW_X11_CLASS_NAME, "wubi-query-desktop")`
- **AND** 调用 `glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "wubi-query-desktop")`

#### Scenario: Wayland Environment
- **WHEN** 应用程序在 Wayland 环境下启动
- **THEN** 调用 `glfwWindowHintString(GLFW_WAYLAND_APP_ID, "wubi-query-desktop")`

### Requirement: Desktop File Association
构建脚本生成的 `.desktop` 文件 SHALL 包含 `StartupWMClass` 字段，以声明窗口与文件的关联。

#### Scenario: Matching Class Name
- **WHEN** 桌面环境解析 `wubi-query-desktop.desktop`
- **THEN** 文件包含行 `StartupWMClass=wubi-query-desktop`
- **AND** 桌面环境将应用窗口的显示名称设置为此文件的 `Name` 字段值
