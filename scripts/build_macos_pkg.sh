#!/bin/bash

# 简化的macOS PKG构建脚本
# 使用macOS原生工具创建专业安装包

set -e

# 临时文件清理函数
cleanup() {
    if [ -d "$PKG_DIR" ]; then
        echo "清理临时目录: $PKG_DIR"
        rm -rf "$PKG_DIR"
    fi
}

# 设置退出时清理
trap cleanup EXIT
trap cleanup ERR

PROJECT_NAME="wubi-query-desktop"
APP_NAME="五笔编码查询"
VERSION="1.0.0"
IDENTIFIER="com.wubiquery.desktop"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
PKG_DIR="$PROJECT_ROOT/pkg_temp"
RELEASE_DIR="$PROJECT_ROOT/release"

echo "=== macOS PKG 构建脚本 ==="
echo "应用名称: $APP_NAME"
echo "版本: $VERSION"
echo "包标识符: $IDENTIFIER"

# 创建并清理临时目录
if [ -d "$PKG_DIR" ]; then
    rm -rf "$PKG_DIR"
fi
mkdir -p "$PKG_DIR"

# 创建release目录
if [ ! -d "$RELEASE_DIR" ]; then
    echo "创建release目录: $RELEASE_DIR"
    mkdir -p "$RELEASE_DIR"
fi

# 创建构建目录
if [ ! -d "$BUILD_DIR" ]; then
    echo "创建构建目录: $BUILD_DIR"
    mkdir -p "$BUILD_DIR"
    echo "配置CMake..."
    cd "$BUILD_DIR"
    cmake .. -DCMAKE_BUILD_TYPE=Release
else
    echo "使用现有构建目录: $BUILD_DIR"
fi

# 进入构建目录
cd "$BUILD_DIR"

# 构建应用
echo "构建应用..."
make clean || true
make -j4

# 创建应用程序包结构
APP_ROOT="$PKG_DIR/Applications"
mkdir -p "$APP_ROOT"

APP_DIR="$APP_ROOT/$APP_NAME.app"
CONTENTS_DIR="$APP_DIR/Contents"
MACOS_DIR="$CONTENTS_DIR/MacOS"
RESOURCES_DIR="$CONTENTS_DIR/Resources"

echo "创建应用程序包..."
mkdir -p "$MACOS_DIR"
mkdir -p "$RESOURCES_DIR"

# 创建Info.plist
cat > "$CONTENTS_DIR/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDisplayName</key>
    <string>Wubi Query Desktop</string>
    <key>CFBundleExecutable</key>
    <string>wubi-query-desktop</string>
    <key>CFBundleIdentifier</key>
    <string>$IDENTIFIER</string>
    <key>CFBundleName</key>
    <string>$APP_NAME</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>$VERSION</string>
    <key>CFBundleVersion</key>
    <string>$VERSION</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.15</string>
    <key>NSHighResolutionCapable</key>
    <true/>
</dict>
</plist>
EOF

# 复制可执行文件和资源
echo "复制应用文件..."
cp "bin/$PROJECT_NAME" "$MACOS_DIR/"
chmod +x "$MACOS_DIR/$PROJECT_NAME"

# 复制资源文件到应用程序包中
cp -r assets "$RESOURCES_DIR/"
cp -r fonts "$RESOURCES_DIR/"

# 创建资源文件夹用于pkg（数据文件）
PKG_RESOURCES_DIR="$PKG_DIR/usr/local/share/$PROJECT_NAME"
mkdir -p "$PKG_RESOURCES_DIR"
cp -r assets "$PKG_RESOURCES_DIR/"
cp -r fonts "$PKG_RESOURCES_DIR/"

# 创建安装脚本
INSTALL_SCRIPT="$PKG_DIR/postinstall"
cat > "$INSTALL_SCRIPT" << 'EOF'
#!/bin/bash

# 安装后脚本
echo "正在安装 Wubi Query Desktop..."

# 设置资源权限
chmod -R 755 /usr/local/share/wubi-query-desktop
chown -R root:wheel /usr/local/share/wubi-query-desktop

echo "安装完成！"
exit 0
EOF

chmod +x "$INSTALL_SCRIPT"

# 创建卸载脚本
UNINSTALL_SCRIPT="$PKG_ROOT/../uninstall.sh"
cat > "$UNINSTALL_SCRIPT" << EOF
#!/bin/bash

# 卸载脚本
echo "正在卸载 Wubi Query Desktop..."

# 删除应用程序
rm -rf "/Applications/五笔编码查询.app"

# 删除资源文件
rm -rf "/usr/local/share/$PROJECT_NAME"

echo "卸载完成！"
EOF

chmod +x "$UNINSTALL_SCRIPT"

# 创建PKG
echo "创建PKG安装包..."
pkgbuild \
    --root "$PKG_DIR" \
    --identifier "$IDENTIFIER" \
    --version "$VERSION" \
    --install-location "/" \
    --scripts "$PKG_DIR" \
    --component-plist "$PROJECT_ROOT/config/component.plist" \
    "$RELEASE_DIR/五笔编码查询-$VERSION.pkg"

echo "=========================================="
echo "PKG安装包创建完成！"
echo "文件位置: $RELEASE_DIR/五笔编码查询-$VERSION.pkg"
ls -lh "$RELEASE_DIR/五笔编码查询-$VERSION.pkg"
echo "=========================================="
echo "安装命令: sudo installer -pkg $RELEASE_DIR/五笔编码查询-$VERSION.pkg -target /"
echo "卸载脚本: $PROJECT_ROOT/uninstall.sh"