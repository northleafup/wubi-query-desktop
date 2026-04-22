#!/bin/bash

# 桌面应用程序运行脚本

# set -e  # 注释掉，避免多次调用build.sh导致退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

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

PROJECT_NAME="wubi-query-desktop"
BUILD_DIR="build"
EXECUTABLE="$BUILD_DIR/bin/$PROJECT_NAME"

# 检查构建目录和可执行文件
if [ ! -d "$BUILD_DIR" ] || [ ! -f "$EXECUTABLE" ] || [ ! -d "$BUILD_DIR/fonts" ]; then
    print_warning "需要构建项目，自动构建..."
    ./scripts/build.sh

    # 再次检查是否构建成功
    if [ ! -f "$EXECUTABLE" ]; then
        print_error "构建失败，无法找到可执行文件"
        exit 1
    fi
fi

# 运行应用程序
print_info "启动桌面应用程序..."
print_info "按 ESC 键或关闭窗口退出"
echo "================================="

cd "$BUILD_DIR"
set -e  # 重新启用错误处理
./bin/$PROJECT_NAME