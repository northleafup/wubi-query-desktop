# 构建和部署脚本

本目录包含用于构建、运行和部署五笔编码查询应用程序的脚本。

## 📁 脚本说明

### 🔧 常用脚本

#### `run.sh`
**用途**: 本地开发调试
**功能**:
- 检查应用是否已构建，若未构建则自动调用构建流程
- 启动应用程序
- 提示退出快捷键

**使用方法**:
```bash
./scripts/run.sh
```

#### `deploy.sh`
**用途**: 一键部署与更新 (推荐)
**功能**:
- 自动识别操作系统 (macOS / Ubuntu Linux)
- 清理旧构建缓存，执行全新构建
- 自动生成安装包 (macOS 生成 PKG, Ubuntu 生成 DEB)
- 自动查杀正在运行的旧进程
- 自动卸载旧版本并安装新生成的包

**使用方法**:
```bash
./scripts/deploy.sh
```
*注意：安装步骤需要 sudo 权限，运行过程中会提示输入密码。*

---

## 🚀 快速开始

### 1. 日常开发
```bash
# 启动应用进行调试
./scripts/run.sh
```

### 2. 发布与更新
```bash
# 一键完成构建、打包、卸载旧版本并安装新版本
./scripts/deploy.sh
```

## 📋 系统要求

- **macOS**: 10.15+ (Catalina)
- **Linux**: Ubuntu 20.04+ (Debian-based)

## 🔧 依赖项

### 构建依赖
- CMake 3.16+
- C++17 兼容编译器 (GCC 9+, Clang 10+)
- `pkg-config`, `git`

### macOS 特定
- Xcode Command Line Tools

### Linux 特定
- build-essential
- libglfw3-dev, libgl1-mesa-dev

## ⚠️ 注意事项

1. **权限**: `deploy.sh` 包含安装步骤，执行时需要 sudo 权限。
2. **进程占用**: `deploy.sh` 会自动关闭正在运行的 `wubi-query-desktop`，请确保数据已保存。
3. **网络**: 首次构建可能需要下载子模块，请保持网络畅通。

## 🐛 故障排除

### 构建失败
```bash
# 手动清理构建目录并重试
rm -rf build/
./scripts/deploy.sh
```

### 图标显示异常
如果是 Linux 用户，安装完成后若图标未刷新，可尝试：
```bash
sudo update-desktop-database
gtk-update-icon-cache
```
