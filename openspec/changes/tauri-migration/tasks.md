## 1. 环境搭建

- [ ] 1.1 安装 Rust 工具链 (`rustup`)、Node.js、Tauri CLI (`npm install -D @tauri-apps/cli`)
- [ ] 1.2 在项目根目录初始化 Tauri 项目 (`npm init` + `npx tauri init`)，生成 `src-tauri/` 骨架
- [ ] 1.3 配置 `src-tauri/tauri.conf.json`：窗口标题"五笔编码查询"、默认尺寸 1200x800、asset 协议 scope 覆盖 `assets/images/char/**`
- [ ] 1.4 确认 `npm run tauri dev` 能启动空白窗口（验证 Tauri 开发环境正常）

## 2. Rust 后端 — 数据层

- [ ] 2.1 在 `src-tauri/src/data.rs` 定义 `CharInfo` 结构体（serde Serialize），字段：character, wubi_code, simple_codes, pinyin, strokes, segments, has_image, image_path
- [ ] 2.2 实现 `load_wubi_data(path) -> HashMap<String, CharInfo>`：解析 `data-wubi-v86.tsv`，提取 char/code/code_short/code_more/segments 字段
- [ ] 2.3 实现 `load_char_data(path, map) -> HashMap<String, CharInfo>`：解析 `data-chars.tsv`，合并 pinyin/strokes 字段到已有 map
- [ ] 2.4 实现 `scan_images(dir) -> HashSet<String>`：扫描 `assets/images/char/*.gif`，返回可用汉字集合
- [ ] 2.5 实现 `filter_and_enrich(map, images)`：过滤无图片的汉字，为保留的汉字设置 `has_image=true` 和 `image_path`
- [ ] 2.6 实现 `query(map, input) -> Vec<CharInfo>`：从输入中提取汉字（UTF-8 解码 + CJK 范围判断），去重后逐一查询，返回结果数组

## 3. Rust 后端 — Tauri Command 暴露

- [ ] 3.1 在 `src-tauri/src/main.rs` 创建 Tauri managed state，存放 `HashMap<String, CharInfo>`
- [ ] 3.2 实现 `setup` 钩子：在应用启动时调用数据加载函数，构建索引并存入 state
- [ ] 3.3 注册 `#[tauri::command] fn query_chars(input: String, state: State<DataStore>) -> Vec<CharInfo>`
- [ ] 3.4 配置 `tauri.conf.json` 的 `app.assetProtocol.scope`，允许前端访问 `assets/images/char/` 目录

## 4. 前端 — HTML/CSS/JS

- [ ] 4.1 编写 `src/index.html`：标题栏（应用名 + 汉字计数 + ESC 提示）、搜索卡片（输入框 + 查询按钮）、结果表格骨架（6列：序号/汉字/基本信息/五笔全码/简码/字根拆解）
- [ ] 4.2 编写 `src/style.css`：现代卡片式布局，搜索框圆角边框，表格斑马纹背景，汉字列大号字体，与现有 `setupImGuiStyle()` 配色方案一致（蓝色主题 #4890E0）
- [ ] 4.3 编写 `src/app.js`：监听回车/按钮点击 → 调用 `invoke('query_chars', { input })` → 动态填充表格 → 用 `convertFileSrc()` 生成 GIF 图片 URL 并设置 `<img>` 标签

## 5. 资源集成

- [ ] 5.1 在 `src-tauri/tauri.conf.json` 的 `build` 配置中设置 `beforeDevCommand` 和 `beforeBuildCommand`
- [ ] 5.2 配置 `tauri.conf.json` 的 bundle `resources` 字段，将 `assets/data/*.tsv` 和 `assets/images/char/**` 打入安装包
- [ ] 5.3 处理资源路径：开发模式从项目根目录 `assets/` 读取，打包后从 Tauri `resolveResource()` 路径读取

## 6. 跨平台验证

- [ ] 6.1 macOS：`npm run tauri dev` 验证中文无乱码、GIF 图片正常显示、查询结果正确
- [ ] 6.2 Windows：`npm run tauri dev` 验证同样功能（需 WebView2 运行时）
- [ ] 6.3 Linux (Ubuntu)：安装 `libwebkit2gtk-4.1-dev` 等依赖后 `npm run tauri dev` 验证

## 7. 打包发布

- [ ] 7.1 `npm run tauri build` 生成 macOS `.app`/`.dmg`
- [ ] 7.2 `npm run tauri build` 生成 Windows `.msi`/`.exe`
- [ ] 7.3 `npm run tauri build` 生成 Linux `.deb`
- [ ] 7.4 验证安装包安装后运行正常，资源文件路径正确

## 8. 清理旧代码和资源（迁移验证完成后）

- [ ] 8.1 删除旧 C++ 源码：`src/`、`src/include/`、`src/wubi/`
- [ ] 8.2 删除旧构建系统：`CMakeLists.txt`、`tests/`
- [ ] 8.3 删除旧脚本：`scripts/build.sh`、`scripts/run.sh`、`scripts/build_macos_pkg.sh`、`scripts/deploy.sh`
- [ ] 8.4 删除旧子模块和第三方库：`third_party/imgui/`、`third_party/glfw/`、`third_party/stb_image.h`
- [ ] 8.5 删除 `.gitmodules`（imgui 和 glfw 子模块配置不再需要）
- [ ] 8.6 删除 `assets/fonts/` 目录（WebView 原生渲染中文，不再需要打包 wqy-zenhei.ttc 等字体文件）
- [ ] 8.7 删除 `config/` 目录（`imgui.ini` 是 ImGui 运行时配置，`component.plist` 是旧 macOS PKG 打包配置，均不再需要）
- [ ] 8.8 清理 `.gitignore`：移除 CMake/build/ImGui/字体 相关条目，添加 Tauri 相关条目（`src-tauri/target/`、`node_modules/`）
