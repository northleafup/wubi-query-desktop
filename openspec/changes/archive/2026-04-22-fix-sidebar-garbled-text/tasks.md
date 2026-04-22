## 1. 代码实现

- [x] 1.1 在 `src/Application.cpp` 的 `createWindow()` 函数中，于 `glfwCreateWindow` 之前添加 `glfwWindowHintString` 调用
- [x] 1.2 设置 `GLFW_X11_CLASS_NAME` 和 `GLFW_X11_INSTANCE_NAME` 为 "wubi-query-desktop"
- [x] 1.3 设置 `GLFW_WAYLAND_APP_ID` 为 "wubi-query-desktop" (使用宏保护)

## 2. 打包脚本更新

- [x] 2.1 在 `scripts/build_ubuntu_deb.sh` 的桌面文件生成部分添加 `StartupWMClass` 字段
- [x] 2.2 验证生成的 `.desktop` 文件格式正确

## 3. 验证

- [x] 3.1 构建项目，确认无编译错误
- [x] 3.2 运行程序，通过 `xprop WM_CLASS` 验证 X11 类名是否正确设置
- [x] 3.3 构建 DEB 包并安装，验证边栏显示正确的中文名称而非乱码
