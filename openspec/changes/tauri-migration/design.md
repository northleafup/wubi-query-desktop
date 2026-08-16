## Context

现有项目是 C++17 + CMake + ImGui + GLFW + OpenGL3 架构，约 3000 行代码。核心功能极简：解析两个 TSV 文件（~8000 + ~11000 行），构建 HashMap 索引，支持按汉字查询，展示结果表格和字根 GIF 图片。当前 6764 个汉字各有对应的 `assets/images/char/<汉字>.gif` 字根拆解图。

用户具备 C++ 和简单 HTML 阅读能力，不掌握 Rust 和现代前端框架。

## Goals / Non-Goals

**Goals:**
- 三平台（macOS / Windows / Linux）无乱码的中文渲染
- 现代化美观 UI，用户可看懂/微调 HTML/CSS
- 包体 ≤ 5MB，原生安装包（.app / .msi / .deb）
- 复用全部现有 TSV 数据和 GIF 资源，零数据转换
- 前端代码 ≤ 300 行纯 HTML/CSS/JS，无 React/Vue 等框架

**Non-Goals:**
- 不做网络功能（纯离线查询工具）
- 不做用户登录/数据同步
- 不迁移 `BackgroundRenderer`（原 ImGui 平台背景切换功能，不再需要）
- 不实现 GIF 动画控制（浏览器原生播放即可）

## Decisions

### D1: Tauri v2 而非 Electron / Flutter / Qt6

**选择**: Tauri v2（Rust 后端 + 系统 WebView 前端）

**理由**:
- 包体 ~3-5MB（Electron ~80MB+，Flutter ~30MB）
- macOS 用 WKWebView、Windows 用 WebView2（Win10+ 自带）、Linux 用 WebKitGTK — 中文渲染由系统 WebView 原生处理，彻底消除乱码
- GIF 图片用 `<img src="...">` 浏览器原生渲染，零额外代码（现有 C++ 方案需 stb_image + OpenGL 纹理，176 行代码）
- 前端只需 HTML/CSS/JS，用户能看懂

**否决方案**:
- Electron: 包体太大（~80MB），对简单查询工具不划算
- Flutter: 需要学 Dart，Material 风格固定，GIF 需第三方库
- Qt6 C++: 杀鸡用牛刀，LGPL 协议限制，UI 代码量大

### D2: 前端用纯 Vanilla JS，不用 React/Vue

**选择**: 纯 HTML + CSS + Vanilla JS

**理由**: 用户只看得懂简单 HTML，引入 React/Vue 会大幅增加维护门槛。查询逻辑极简（一个输入框 + 一个表格），不需要组件化框架。

### D3: Rust 后端数据层设计

**选择**: 启动时一次性加载两个 TSV 到 `HashMap<String, CharInfo>`，通过 Tauri Command 暴露 `query(input: String) -> Vec<CharInfo>` 接口。

**理由**: 与现有 C++ `WubiDataParser` 逻辑完全对等，迁移成本最低。数据量 ~6764 条，HashMap 查询 O(1)，无需数据库。

**替代方案**: SQLite — 数据量太小，引入数据库反而增加复杂度。

### D4: 字根图片通过 Tauri Asset 协议访问

**选择**: 配置 `tauri.conf.json` 的 `assetProtocol` scope，前端用 `asset://localhost/path/to/五.gif` 或 Tauri v2 的 `convertFileSrc()` 访问 GIF 文件。

**理由**: 无需将图片嵌入二进制或启动 HTTP 服务器，WebView 原生处理 GIF 解码和动画。

### D5: 项目结构

```
src-tauri/          # Rust 后端
  src/
    main.rs         # Tauri 入口
    data.rs         # TSV 解析 + 查询（~200 行）
  Cargo.toml
  tauri.conf.json   # 窗口配置、asset 协议、打包配置
src/                # 前端（纯 HTML/CSS/JS）
  index.html        # 主页面（~150 行）
  style.css         # 样式（~100 行）
  app.js            # 查询逻辑（~50 行）
assets/             # 复用现有资源，不改动
  data/*.tsv
  images/char/*.gif
```

## Risks / Trade-offs

- **[WebView2 在旧版 Windows 缺失]** → Windows 7/8 无 WebView2 运行时。Tauri 安装包可配置自动安装 WebView2 bootstrapper。Windows 10 1803+ 已自带。
- **[Linux WebKitGTK 依赖]** → Ubuntu 需安装 `libwebkit2gtk-4.1-dev`。通过 `.deb` 包依赖声明自动解决。
- **[6764 个 GIF 文件打包体积]** → GIF 文件总计约 50-100MB，打入安装包后体积较大。→ 可后续优化为压缩格式，当前先保持现状以完成迁移。
- **[旧 C++ 代码保留还是删除]** → 迁移期间保留旧代码作为参考，验证新 Tauri 版本功能一致后再决定是否删除。→ 建议迁移完成后删除 `src/`、`third_party/imgui`、`third_party/glfw`、`CMakeLists.txt` 等旧代码。
