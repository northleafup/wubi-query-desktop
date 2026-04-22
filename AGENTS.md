# AGENTS.md — wubi-query-desktop

## 关键约束
- 本目录下所有操作都允许；禁止使用绝对路径，必须使用相对路径
- 项目基于 C++17 + CMake 3.16+，使用 ImGui + GLFW + OpenGL3 技术栈

## 开发者命令
```bash
# 初始化子模块（首次克隆或 submodule 变动后必须执行）
git submodule update --init --recursive

# 构建（二选一）
./scripts/build.sh              # 一键构建，会清理旧 build/ 并重新 cmake + make
mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j$(nproc)

# 运行（二选一）
./scripts/run.sh                # 自动检查构建状态，未构建则先构建
./build/bin/wubi-query-desktop  # 直接运行，需从项目根目录执行
```

## 构建须知
- `git submodule` 管理 imgui 和 glfw（见 `.gitmodules`），构建前必须初始化
- CMake 输出目录为 `build/bin/`，可执行文件名为 `wubi-query-desktop`
- CMake POST_BUILD 命令会自动将 `assets/fonts/*.ttf|.ttc`、`assets/data/*.tsv` 和 `assets/images/char/` 复制到 `build/bin/assets/`
- `assets/images/char/*.gif` 和 `assets/data/*.tsv` 被 .gitignore 忽略（文件大），但源码仓库中应有对应文件
- 字体文件（`.ttf/.ttc`）同样被 .gitignore 忽略，通过脚本下载

## Linux 依赖（Ubuntu）
```bash
sudo apt-get install build-essential cmake git pkg-config \
    libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev \
    libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

## 项目结构
```
src/                        # C++ 源代码
  main.cpp                  # 入口：创建 Application 对象，调用 initialize() -> run()
  Application.cpp           # 主应用类（窗口、ImGui 渲染循环、UI）
  FontManager.cpp           # 中文字体管理
  BackgroundRenderer.cpp    # 背景渲染
  ImageLoader.cpp           # GIF/图片加载（stb_image）
  wubi/                     # 五笔数据模块
    data_manager.cpp        # 数据加载与管理
    wubi_data_parser.cpp    # TSV 数据解析
    char_data.h             # 字符数据结构
third_party/                # 子模块 + 单头文件库
  imgui/                    # git submodule
  glfw/                     # git submodule
  stb_image.h               # 图片加载
assets/
  fonts/                    # 字体（.ttf/.ttc，被 gitignore）
  data/                     # TSV 数据文件（被 gitignore）
  images/char/              # 6764 张字根 GIF（被 gitignore）
config/
  imgui.ini                 # ImGui 运行时配置（被 gitignore）
  component.plist           # macOS PKG 组件配置
scripts/
  build.sh / run.sh         # 开发构建和运行
  build_macos_pkg.sh        # macOS PKG 打包
  build_ubuntu_deb.sh       # Ubuntu DEB 打包
```

## 测试
- 项目没有单元测试框架或 CI 配置
- 验证方式：构建成功后运行应用，手动测试查询功能

## 打包
- macOS: `./scripts/build_macos_pkg.sh`
- Ubuntu DEB: `./scripts/build_ubuntu_deb.sh`
- 打包产物输出到 `release/` 目录（被 .gitignore）

## 注意事项
- 编译时使用 `-finput-charset=UTF-8 -fexec-charset=UTF-8` 确保中文源码正确处理
- 退出方式：ESC 键或关闭窗口
- 运行时无需额外环境变量，程序从可执行文件相对路径加载 assets
