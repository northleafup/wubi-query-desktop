## Why

当前项目使用 C++ + ImGui + GLFW + OpenGL3 技术栈，存在三个核心问题：
1. **UI 不美观**：ImGui 是即时模式 GPU 渲染框架，天生呈现"开发者工具"风格，无法达到现代桌面应用的视觉品质
2. **macOS 中文乱码**：字体加载依赖打包的 `.ttc` 文件（被 .gitignore 忽略），且未适配 macOS 系统字体路径
3. **字根图片渲染复杂**：使用 stb_image + OpenGL 纹理手动加载 GIF，代码量大且不支持 GIF 动画

用户只需简单的 HTML 阅读能力即可参与前端维护，降低长期维护门槛。

## What Changes

- **BREAKING** — 整体技术栈从 C++/ImGui 迁移到 Tauri v2 (Rust 后端 + 纯 HTML/CSS/JS 前端)
- 复用现有 TSV 数据文件和字根 GIF 图片资源，不做数据格式变更
- Rust 后端实现 TSV 解析、汉字查询、图片路径解析（替代 `wubi/` 模块全部逻辑）
- 纯 HTML/CSS/JS 前端实现搜索框、结果表格、字根 GIF 展示（前端代码不超过 300 行）
- GIF 图片使用浏览器原生 `<img>` 标签渲染，零额外代码
- 中文渲染由系统 WebView 原生支持，彻底消除乱码问题
- 三平台通用：macOS (WKWebView)、Windows (WebView2)、Linux (WebKitGTK)
- 打包产物为原生 `.app` / `.msi` / `.deb`，包体约 3-5MB

## Capabilities

### New Capabilities
- `data-loading`: Rust 后端启动时解析 TSV 数据文件并构建内存索引，通过 Tauri IPC 向前端暴露查询接口
- `char-query`: 前端发送汉字输入，后端返回匹配的五笔编码、拼音、笔画、字根拆解、图片路径等结构化数据
- `image-serving`: 后端通过 Tauri asset 协议提供字根 GIF 文件访问，前端用 `<img>` 标签直接渲染
- `search-ui`: 前端搜索界面，包含输入框、查询按钮、结果表格，纯 HTML/CSS/JS 实现

### Modified Capabilities
<!-- 无已有 spec 需要修改 -->

## Impact

- **新增依赖**: Tauri CLI (`@tauri-apps/cli`)、Tauri API (`@tauri-apps/api`)、Rust `tauri` crate、`serde`/`serde_json`
- **删除依赖**: ImGui、GLFW、OpenGL3、stb_image、CMake 构建体系
- **资源复用**: `assets/data/*.tsv`、`assets/images/char/*.gif` 完全复用，不做任何修改
- **构建流程**: 从 `cmake + make` 变为 `npm run tauri build`
- **打包方式**: 从自定义脚本 (`build_macos_pkg.sh` / `build_ubuntu_deb.sh`) 变为 Tauri 内置打包 (`.app` / `.msi` / `.deb`)
- **代码量**: 从 ~3000 行 C++ 变为约 400 行 Rust + 300 行 HTML/CSS/JS
- **分发**: 产物体积从 ~5MB (无 installer) 变为 ~3-5MB (含 native installer)
