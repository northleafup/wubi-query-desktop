# 构建指南

## 环境要求

### macOS 构建环境
- macOS 10.15+
- Xcode Command Line Tools: `xcode-select --install`
- Rust: https://rustup.rs
- Node.js 18+: https://nodejs.org

### Windows 构建环境
- Windows 10/11
- Visual Studio 2022 (安装"使用 C++ 的桌面开发"工作负载)
- Rust: https://rustup.rs
- Node.js 18+: https://nodejs.org
- WebView2 Runtime (Win10 自带，Win7 需手动安装)

## 构建步骤

### 1. 安装依赖

```bash
# 安装 Rust (两个平台通用)
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh  # macOS/Linux
# 或下载 Windows 安装包: https://rustup.rs

# 安装 Node.js
# macOS: brew install node 或从官网下载
# Windows: 从官网下载安装包

# 验证安装
rustc --version
node --version
npm --version
```

### 2. 获取源码

```bash
git clone <your-repo-url>
cd wubi-query-desktop
```

### 3. 安装项目依赖

```bash
npm install
```

### 4. 开发模式运行

```bash
npm run tauri dev
```

这会启动一个开发窗口，代码修改会自动热重载。

### 5. 构建生产版本

```bash
npm run tauri build
```

构建产物位置：
- macOS: `src-tauri/target/release/bundle/`
  - `.app` 文件 (应用程序)
  - `.dmg` 文件 (安装包)
- Windows: `src-tauri/target/release/bundle/`
  - `.exe` 文件 (应用程序)
  - `.msi` 文件 (安装包)

## Windows 特殊说明

### Win10 兼容性
Tauri v2 使用 WebView2 (Edge Chromium 内核)，Win10 1803+ 版本已内置 WebView2 Runtime。

### Win7 支持 (不推荐)
如果需要支持 Win7：
1. 用户需手动安装 WebView2 Runtime: https://developer.microsoft.com/en-us/microsoft-edge/webview2/
2. 或在安装程序中捆绑 WebView2 安装程序

### Windows 构建问题排查

**问题: "cannot find Visual Studio installation"**
```powershell
# 安装 Visual Studio Build Tools
# 下载: https://visualstudio.microsoft.com/visual-cpp-build-tools/
# 安装时勾选"使用 C++ 的桌面开发"
```

**问题: "WebView2 runtime not found"**
```powershell
# Win10: 通常已内置，尝试更新 Edge 浏览器
# 手动安装: https://developer.microsoft.com/en-us/microsoft-edge/webview2/
```

## 资源文件说明

运行时需要的资源文件（不在 git 中）：
```
resources/
├── data/
│   ├── data-wubi-v86.tsv    # 五笔编码数据
│   └── data-chars.tsv       # 汉字信息数据
└── images/
    └── char/                # 6764 个字根 GIF 图片
        ├── 一.gif
        ├── 乙.gif
        └── ...
```

这些文件需要从原始数据包解压到 `resources/` 目录。

## 常用命令

```bash
# 开发模式
npm run tauri dev

# 构建生产版本
npm run tauri build

# 仅编译 Rust (不打包)
cd src-tauri && cargo build --release

# 清理构建缓存
cd src-tauri && cargo clean
```

## 目录结构

```
wubi-query-desktop/
├── src-tauri/              # Rust 后端
│   ├── src/
│   │   ├── main.rs        # 入口
│   │   ├── lib.rs         # Tauri 命令
│   │   └── data.rs        # 数据加载
│   ├── Cargo.toml
│   └── tauri.conf.json    # 配置
├── ui/                    # 前端 (HTML/CSS/JS)
│   ├── index.html
│   ├── app.js
│   └── style.css
├── resources/             # 运行时资源
│   ├── data/              # TSV 数据
│   └── images/            # 字根图片
└── docs/                  # 文档
```
