#!/bin/bash

# 统一构建与部署脚本
# 功能：自动清理、构建、打包、卸载旧版本并安装新版本
# 支持平台：macOS, Ubuntu (Linux)

set -e

# ================= 全局变量 =================
PROJECT_NAME="wubi-query-desktop"
APP_NAME="五笔编码查询"
VERSION="1.0.0"
MAINTAINER="Wubi Query Desktop Team <wubi-query@example.com>"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
RELEASE_DIR="$PROJECT_ROOT/release"
PLATFORM=""

# ================= 颜色定义 =================
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# ================= 辅助函数 =================

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

check_tool() {
    if ! command -v "$1" >/dev/null 2>&1; then
        log_error "未找到必要工具: $1"
        log_info "请安装后重试: $2"
        exit 1
    fi
}

# ================= 平台检测 =================

detect_platform() {
    OS_NAME=$(uname -s)
    case "$OS_NAME" in
        Darwin)
            PLATFORM="macos"
            log_info "检测到平台: macOS"
            ;;
        Linux)
            PLATFORM="ubuntu"
            log_info "检测到平台: Ubuntu/Linux"
            ;;
        *)
            log_error "不支持的平台: $OS_NAME"
            exit 1
            ;;
    esac
}

# ================= 清理与构建 =================

clean_and_build() {
    log_info "开始清理旧构建目录..."
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
    mkdir -p "$BUILD_DIR"

    log_info "初始化 Git 子模块..."
    cd "$PROJECT_ROOT"
    git submodule update --init --recursive

    log_info "配置 CMake..."
    cd "$BUILD_DIR"
    cmake .. -DCMAKE_BUILD_TYPE=Release

    log_info "编译项目..."
    make -j$(nproc 2>/dev/null || echo 4)

    if [ ! -f "bin/$PROJECT_NAME" ]; then
        log_error "构建失败，未找到可执行文件"
        exit 1
    fi
    log_success "构建完成"
}

# ================= Ubuntu 打包与部署 =================

ubuntu_package() {
    log_info "开始创建 DEB 包目录结构..."
    local DEB_BUILD_DIR="$PROJECT_ROOT/deb_build"
    local DEB_ROOT="$DEB_BUILD_DIR/$PROJECT_NAME"
    local DEB_BIN_DIR="$DEB_ROOT/usr/bin"
    local DEB_SHARE_DIR="$DEB_ROOT/usr/share/$PROJECT_NAME"
    local DEB_DESKTOP_DIR="$DEB_ROOT/usr/share/applications"
    local DEB_DOC_DIR="$DEB_ROOT/usr/share/doc/$PROJECT_NAME"
    local DEB_CONTROL_DIR="$DEB_ROOT/DEBIAN"

    # 清理并创建目录
    rm -rf "$DEB_BUILD_DIR"
    mkdir -p "$DEB_ROOT" "$DEB_BIN_DIR" "$DEB_SHARE_DIR" "$DEB_DESKTOP_DIR" "$DEB_DOC_DIR" "$DEB_CONTROL_DIR"

    # 复制文件
    cp "$BUILD_DIR/bin/$PROJECT_NAME" "$DEB_BIN_DIR/"
    chmod +x "$DEB_BIN_DIR/$PROJECT_NAME"
    cp -r "$PROJECT_ROOT/assets" "$DEB_SHARE_DIR/"
    
    # 安装多尺寸图标到 hicolor 主题目录
    local ICON_SIZES="16 24 32 48 64 128 256"
    log_info "安装应用图标..."
    for size in $ICON_SIZES; do
        local ICON_DIR="$DEB_ROOT/usr/share/icons/hicolor/${size}x${size}/apps"
        mkdir -p "$ICON_DIR"
        if [ -f "$PROJECT_ROOT/assets/icons/app_${size}x${size}.png" ]; then
            cp "$PROJECT_ROOT/assets/icons/app_${size}x${size}.png" "$ICON_DIR/$PROJECT_NAME.png"
        fi
    done
    
    # 生成桌面文件
    cat > "$DEB_DESKTOP_DIR/$PROJECT_NAME.desktop" << EOF
[Desktop Entry]
Encoding=UTF-8
Name=五笔编码查询
GenericName=Wubi Input Method Query Tool
Comment=五笔输入法编码查询桌面应用程序
Exec=env WUBI_ASSETS_PATH=/usr/share/wubi-query-desktop/assets $PROJECT_NAME
Icon=$PROJECT_NAME
Terminal=false
Type=Application
Categories=Utility;Education;
StartupNotify=true
StartupWMClass=$PROJECT_NAME
EOF

    # 生成控制文件
    cat > "$DEB_CONTROL_DIR/control" << EOF
Package: $PROJECT_NAME
Version: $VERSION
Section: utils
Priority: optional
Architecture: amd64
Depends: libc6, libstdc++6, libglfw3
Maintainer: $MAINTAINER
Description: 五笔输入法编码查询桌面应用
  支持多字符查询和字根图片展示。
EOF

    # 生成安装后脚本 (更新缓存)
    cat > "$DEB_CONTROL_DIR/postinst" << 'EOF'
#!/bin/sh
update-desktop-database -q /usr/share/applications || true
gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor || true
exit 0
EOF
    chmod +x "$DEB_CONTROL_DIR/postinst"

    # 打包
    log_info "正在打包 DEB..."
    cd "$DEB_BUILD_DIR"
    dpkg-deb --build "$PROJECT_NAME"
    
    local DEB_FILE="$RELEASE_DIR/$PROJECT_NAME-$VERSION-amd64.deb"
    mkdir -p "$RELEASE_DIR"
    mv "$PROJECT_NAME.deb" "$DEB_FILE"
    log_success "DEB 包已生成: $DEB_FILE"
}

ubuntu_install() {
    local DEB_FILE="$RELEASE_DIR/$PROJECT_NAME-$VERSION-amd64.deb"
    
    # 查杀进程
    log_info "正在关闭正在运行的程序..."
    killall $PROJECT_NAME 2>/dev/null || true
    
    # 卸载旧版本
    log_info "正在卸载旧版本..."
    dpkg -r $PROJECT_NAME 2>/dev/null || true
    dpkg -P $PROJECT_NAME 2>/dev/null || true

    # 安装新版本
    log_info "正在安装新版本..."
    sudo dpkg -i "$DEB_FILE"
    sudo apt-get install -f -y
    log_success "安装完成！"
}

# ================= macOS 打包与部署 =================

macos_package() {
    log_info "开始创建 PKG 目录结构..."
    local PKG_DIR="$PROJECT_ROOT/pkg_build"
    local APP_NAME="五笔编码查询.app"
    local IDENTIFIER="com.wubiquery.desktop"
    
    # 清理并创建目录
    rm -rf "$PKG_DIR"
    mkdir -p "$PKG_DIR"
    
    # 创建 App 结构
    local CONTENTS_DIR="$PKG_DIR/Applications/$APP_NAME/Contents"
    mkdir -p "$CONTENTS_DIR/MacOS" "$CONTENTS_DIR/Resources"

    # Info.plist
    cat > "$CONTENTS_DIR/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDisplayName</key><string>$APP_NAME</string>
    <key>CFBundleExecutable</key><string>$PROJECT_NAME</string>
    <key>CFBundleIdentifier</key><string>$IDENTIFIER</string>
    <key>CFBundleName</key><string>$PROJECT_NAME</string>
    <key>CFBundleVersion</key><string>$VERSION</string>
    <key>LSMinimumSystemVersion</key><string>10.15</string>
</dict>
</plist>
EOF

    # 复制文件
    cp "$BUILD_DIR/bin/$PROJECT_NAME" "$CONTENTS_DIR/MacOS/"
    chmod +x "$CONTENTS_DIR/MacOS/$PROJECT_NAME"
    cp -r "$PROJECT_ROOT/assets" "$CONTENTS_DIR/Resources/"

    # 打包
    log_info "正在打包 PKG..."
    pkgbuild --root "$PKG_DIR/Applications" \
             --identifier "$IDENTIFIER" \
             --version "$VERSION" \
             --install-location "/Applications" \
             "$RELEASE_DIR/五笔编码查询-$VERSION.pkg"
             
    log_success "PKG 包已生成: $RELEASE_DIR/五笔编码查询-$VERSION.pkg"
}

macos_install() {
    local PKG_FILE="$RELEASE_DIR/五笔编码查询-$VERSION.pkg"
    
    # 查杀进程
    log_info "正在关闭正在运行的程序..."
    killall $PROJECT_NAME 2>/dev/null || true
    
    # 卸载旧版本 (清理记录和应用)
    log_info "正在卸载旧版本..."
    sudo pkgutil --forget "com.wubiquery.desktop" 2>/dev/null || true
    sudo rm -rf "/Applications/$APP_NAME"

    # 安装新版本
    log_info "正在安装新版本..."
    sudo installer -pkg "$PKG_FILE" -target /
    log_success "安装完成！"
}

# ================= 主流程 =================

main() {
    log_info "=== 五笔编码查询 自动化部署 ==="
    
    detect_platform
    
    check_tool "cmake" "sudo apt install cmake / brew install cmake"
    check_tool "make" "sudo apt install build-essential / xcode-select --install"

    clean_and_build

    case "$PLATFORM" in
        ubuntu)
            check_tool "dpkg-deb" "sudo apt install dpkg-dev"
            ubuntu_package
            ubuntu_install
            ;;
        macos)
            check_tool "pkgbuild" "Xcode Command Line Tools"
            macos_package
            macos_install
            ;;
    esac

    log_info "=== 部署结束 ==="
}

main "$@"
