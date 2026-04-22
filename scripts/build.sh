#!/bin/bash

# 桌面应用程序构建脚本
# 用于一键构建项目

set -e  # 遇到错误立即退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印带颜色的消息
print_info() {
    echo -e "${BLUE}[信息]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[成功]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[警告]${NC} $1"
}

print_error() {
    echo -e "${RED}[错误]${NC} $1"
}

# 项目配置
PROJECT_NAME="wubi-query-desktop"
BUILD_DIR="build"

print_info "开始构建桌面应用程序..."

# 创建构建目录
if [ -d "$BUILD_DIR" ]; then
    print_warning "构建目录已存在，清理旧文件..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"

# 初始化子模块
print_info "初始化Git子模块..."
git submodule update --init --recursive

# 配置CMake
cd "$BUILD_DIR"
print_info "配置CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译项目
print_info "编译项目..."
# 检测操作系统并设置合适的并行数
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS 使用 sysctl 获取 CPU 核心数
    CORES=$(sysctl -n hw.ncpu)
else
    # Linux 使用 nproc
    CORES=$(nproc)
fi
make -j$CORES

print_success "构建完成！"
print_info "可执行文件: $BUILD_DIR/bin/$PROJECT_NAME"

# 测试可执行文件
if [ -f "bin/$PROJECT_NAME" ]; then
    print_success "可执行文件生成成功"
else
    print_error "可执行文件生成失败"
    exit 1
fi