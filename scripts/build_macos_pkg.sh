#!/bin/bash

# macOS PKG 打包脚本
# 功能：将已构建的应用打包为 .pkg 安装包

set -e

PROJECT_NAME="wubi-query-desktop"
APP_NAME="五笔编码查询.app"
VERSION="1.0.0"
IDENTIFIER="com.wubiquery.desktop"

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
RELEASE_DIR="$PROJECT_ROOT/release"
PKG_DIR="$PROJECT_ROOT/pkg_build"

# ================= 颜色定义 =================
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# ================= 主流程 =================

if [ ! -f "$BUILD_DIR/bin/$PROJECT_NAME" ]; then
    log_error "未找到可执行文件，请先运行 ./scripts/build.sh"
    exit 1
fi

log_info "开始创建 PKG 目录结构..."
rm -rf "$PKG_DIR"
CONTENTS_DIR="$PKG_DIR/Applications/$APP_NAME/Contents"
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
mkdir -p "$RELEASE_DIR"
log_info "正在打包 PKG..."
pkgbuild --root "$PKG_DIR/Applications" \
         --identifier "$IDENTIFIER" \
         --version "$VERSION" \
         --install-location "/Applications" \
         "$RELEASE_DIR/五笔编码查询-$VERSION.pkg"

log_success "PKG 包已生成: $RELEASE_DIR/五笔编码查询-$VERSION.pkg"
