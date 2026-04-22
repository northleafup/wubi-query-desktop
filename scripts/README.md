# 构建和部署脚本

本目录包含用于构建、运行和部署五笔编码查询应用程序的脚本。

## 📁 脚本说明

### 🔧 构建脚本

#### `build.sh`
**用途**: 一键构建应用程序

**功能**:
- 创建并清理构建目录
- 初始化 Git 子模块
- 配置 CMake (Release 模式)
- 多核编译项目
- 验证可执行文件生成

**使用方法**:
```bash
./scripts/build.sh
```

#### `run.sh`
**用途**: 运行应用程序

**功能**:
- 检查可执行文件是否存在
- 验证字体文件
- 启动应用程序
- 提供退出说明

**使用方法**:
```bash
./scripts/run.sh
```

### 📦 部署脚本

#### `create_package.sh`
**用途**: 创建 macOS 安装包

**功能**:
- 支持多种安装包类型选择:
  1. DMG 拖拽安装包 (推荐)
  2. PKG 安装包
  3. APP 应用程序包
  4. 全部创建
  5. 仅构建应用
- 自动化构建和打包流程
- 创建应用程序包结构
- 包含卸载功能

**使用方法**:
```bash
./scripts/create_package.sh
```

#### `build_macos_pkg.sh`
**用途**: 专业的 macOS PKG 安装包构建

**功能**:
- 使用 macOS 原生工具
- 创建带卸载功能的 PKG 包
- 支持 UI 安装界面
- 包含许可证和欢迎信息
- **自动清理临时文件** - 无论成功或失败都会清理 `pkg_temp` 目录

**使用方法**:
```bash
./scripts/build_macos_pkg.sh
```

**注意**: 脚本使用 `trap` 机制确保临时目录在任何情况下都会被自动清理

### 📄 配置文件

#### `../config/component.plist`
**用途**: macOS PKG 组件配置

**功能**:
- 定义应用程序安装位置
- 设置升级策略
- 配置包标识符

---

## 🚀 快速开始

### 1. 开发环境构建
```bash
# 构建项目
./scripts/build.sh

# 运行应用
./scripts/run.sh
```

### 2. 创建安装包
```bash
# 创建 DMG (推荐)
./scripts/create_package.sh

# 选择选项 1 (DMG)
```

### 3. 专业 PKG 打包
```bash
# 创建专业 PKG 安装包
./scripts/build_macos_pkg.sh
```

## 📋 系统要求

- **macOS**: 10.15+ (Catalina)
- **Linux**: Ubuntu 20.04+ 或等效系统
- **Windows**: Windows 10+ (WSL 环境)

## 🔧 依赖项

### 构建依赖
- CMake 3.16+
- C++17 兼容编译器 (GCC 7+, Clang 8+)
- 开发工具包 (build-essential, Xcode Command Line Tools)

### macOS 特定
- Xcode Command Line Tools
- macOS SDK

### Linux 特定
- libglfw3-dev
- libgl1-mesa-dev
- libglu1-mesa-dev

## ⚠️ 注意事项

1. **字体文件**: 应用程序依赖 `assets/fonts/` 目录下的字体文件
2. **数据文件**: 确保所有 TSV 数据文件在 `assets/data/` 目录中
3. **图片资源**: 字根图片文件在 `assets/images/char/` 目录中
4. **权限**: 脚本需要执行权限 (`chmod +x scripts/*.sh`)
5. **空间**: 确保有足够的磁盘空间进行构建和打包

## 🐛 故障排除

### 构建失败
```bash
# 清理并重新构建
rm -rf build/
./scripts/build.sh
```

### 字体问题
```bash
# 检查字体文件
ls -la assets/fonts/
# 应该包含: wqy-zenhei.ttc, wqy-microhei.ttc, DejaVuSans.ttf
```

### PKG 打包失败
```bash
# 检查 macOS 开发者工具
xcode-select --install

# 验证配置文件
cat config/component.plist
```