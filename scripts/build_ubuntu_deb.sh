#!/bin/sh

# Ubuntu DEB包构建脚本
# 使用标准Debian打包工具创建DEB安装包

# 临时文件清理函数
cleanup() {
    if [ -d "$DEB_BUILD_DIR" ]; then
        echo "清理临时目录: $DEB_BUILD_DIR"
        rm -rf "$DEB_BUILD_DIR"
    fi
}

# 设置退出时清理
trap cleanup EXIT

PROJECT_NAME="wubi-query-desktop"
APP_NAME="五笔编码查询"
VERSION="1.0.0"
MAINTAINER="Wubi Query Desktop Team <wubi-query@example.com>"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
DEB_BUILD_DIR="$PROJECT_ROOT/deb_build"
RELEASE_DIR="$PROJECT_ROOT/release"

echo "=== Ubuntu DEB 包构建脚本 ==="
echo "应用名称: $APP_NAME"
echo "版本: $VERSION"
echo "维护者: $MAINTAINER"

# 检查必要的工具
if ! command -v dpkg-deb >/dev/null 2>&1; then
    echo "错误: 未找到 dpkg-deb 工具"
    echo "请安装: sudo apt-get install dpkg-dev"
    exit 1
fi

if ! command -v fakeroot >/dev/null 2>&1; then
    echo "错误: 未找到 fakeroot 工具"
    echo "请安装: sudo apt-get install fakeroot"
    exit 1
fi

if ! command -v cmake >/dev/null 2>&1; then
    echo "错误: 未找到 cmake 工具"
    echo "请安装: sudo apt-get install cmake"
    exit 1
fi

if ! command -v gdebi >/dev/null 2>&1; then
    echo "警告: 未找到 gdebi 工具"
    echo "推荐安装: sudo apt-get install gdebi"
    echo "gdebi 可以自动处理包依赖关系，比 dpkg 更方便"
    echo ""
fi

# 创建并清理临时目录
if [ -d "$DEB_BUILD_DIR" ]; then
    rm -rf "$DEB_BUILD_DIR"
fi
mkdir -p "$DEB_BUILD_DIR"

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
    if ! cmake .. -DCMAKE_BUILD_TYPE=Release; then
        echo "错误: CMake配置失败"
        exit 1
    fi
else
    echo "使用现有构建目录: $BUILD_DIR"
    cd "$BUILD_DIR"
fi

# 清理之前的构建
echo "清理之前的构建..."
if ! make clean 2>/dev/null; then
    echo "警告: make clean 失败，继续构建..."
fi

# 构建应用
echo "构建应用..."
if ! make -j$(nproc 2>/dev/null || echo 4); then
    echo "错误: 构建失败"
    exit 1
fi

# 检查构建是否成功
if [ ! -f "bin/$PROJECT_NAME" ]; then
    echo "错误: 构建失败，未找到可执行文件"
    echo "构建目录内容:"
    ls -la bin/ 2>/dev/null || echo "  bin目录不存在"
    exit 1
fi

echo "构建成功: $(ls -lh bin/$PROJECT_NAME | awk '{print $5}')"

# 创建DEB包目录结构
DEB_ROOT="$DEB_BUILD_DIR/$PROJECT_NAME"
DEB_BIN_DIR="$DEB_ROOT/usr/bin"
DEB_SHARE_DIR="$DEB_ROOT/usr/share/$PROJECT_NAME"
DEB_DESKTOP_DIR="$DEB_ROOT/usr/share/applications"
DEB_ICON_DIR="$DEB_ROOT/usr/share/icons/hicolor/256x256/apps"
DEB_DOC_DIR="$DEB_ROOT/usr/share/doc/$PROJECT_NAME"
DEB_MAN_DIR="$DEB_ROOT/usr/share/man/man1"
DEB_CONTROL_DIR="$DEB_ROOT/DEBIAN"

echo "创建DEB包目录结构..."
mkdir -p "$DEB_BIN_DIR"
mkdir -p "$DEB_SHARE_DIR"
mkdir -p "$DEB_DESKTOP_DIR"
mkdir -p "$DEB_ICON_DIR"
mkdir -p "$DEB_DOC_DIR"
mkdir -p "$DEB_MAN_DIR"
mkdir -p "$DEB_CONTROL_DIR"

# 复制可执行文件
echo "复制应用文件..."
# 切换回项目根目录
cd "$PROJECT_ROOT"
cp "$BUILD_DIR/bin/$PROJECT_NAME" "$DEB_BIN_DIR/"
chmod +x "$DEB_BIN_DIR/$PROJECT_NAME"

# 复制资源文件
if [ -d "assets" ]; then
    cp -r assets "$DEB_SHARE_DIR/"
fi
if [ -d "fonts" ]; then
    cp -r fonts "$DEB_SHARE_DIR/"
fi

# 创建桌面文件
echo "创建桌面文件..."
cat > "$DEB_DESKTOP_DIR/$PROJECT_NAME.desktop" << EOF
[Desktop Entry]
Name=Wubi Query Desktop
Name[zh_CN]=五笔编码查询
GenericName=Wubi Input Method Query Tool
GenericName[zh_CN]=五笔输入法编码查询工具
Comment=A desktop application for querying Wubi input method codes
Comment[zh_CN]=五笔输入法编码查询桌面应用程序
Exec=env WUBI_ASSETS_PATH=/usr/share/wubi-query-desktop/assets $PROJECT_NAME
Icon=$PROJECT_NAME
Terminal=false
Type=Application
Categories=Utility;Education;Office;Development;
Keywords=Wubi;Input;Method;Chinese;Query;五笔;编码;字典;
StartupNotify=true
DBusActivatable=true
Actions=Preferences;Help;
MimeType=text/plain;text/x-wubi;

[Desktop Action Preferences]
Name=Preferences
Name[zh_CN]=偏好设置
Exec=$PROJECT_NAME --preferences

[Desktop Action Help]
Name=Help
Name[zh_CN]=帮助
Exec=$PROJECT_NAME --help
EOF

# 安装多尺寸图标到 hicolor 主题目录
ICON_SIZES="16 24 32 48 64 128 256"
echo "安装应用图标..."
for size in $ICON_SIZES; do
    ICON_DIR="$DEB_ROOT/usr/share/icons/hicolor/${size}x${size}/apps"
    mkdir -p "$ICON_DIR"
    if [ -f "$PROJECT_ROOT/assets/icons/app_${size}x${size}.png" ]; then
        cp "$PROJECT_ROOT/assets/icons/app_${size}x${size}.png" "$ICON_DIR/$PROJECT_NAME.png"
        echo "  安装 ${size}x${size} 图标"
    else
        echo "  警告: 缺少 ${size}x${size} 图标文件"
    fi
done

# 创建文档
echo "创建文档..."
cat > "$DEB_DOC_DIR/README" << EOF
五笔编码查询桌面应用 v$VERSION

这是一个基于C++17和ImGui开发的五笔编码查询桌面应用程序，
支持13,928个汉字的五笔编码查询，包含字根拆解图片显示功能。

特性:
- 实时五笔编码查询
- 支持单字和词组查询
- 显示字根拆解图片
- 现代化用户界面
- 跨平台支持

使用方法:
- 启动应用: 五笔编码查询
- 或在应用菜单中搜索 "Wubi Query Desktop"
- 在搜索框中输入要查询的汉字或词组
- 查看编码信息、字根拆解和拼音信息
- 按ESC键或关闭窗口退出

项目主页: https://github.com/hantang/search-wubi
EOF

cat > "$DEB_DOC_DIR/copyright" << EOF
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: $PROJECT_NAME
Source: https://github.com/hantang/search-wubi

Files: *
Copyright: 2024 Wubi Query Desktop Team
License: MIT

License: MIT
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 .
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 .
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
EOF

# 创建手册页
echo "创建手册页..."
cat > "$DEB_MAN_DIR/$PROJECT_NAME.1" << EOF
.TH "WUBI-QUERY-DESKTOP" "1" "$(date +%Y-%m-%d)" "$VERSION" "User Commands"

.SH NAME
五笔编码查询 \- 五笔输入法编码查询桌面应用

.SH SYNOPSIS
.B $PROJECT_NAME

.SH DESCRIPTION
这是一个基于C++17和ImGui开发的五笔编码查询桌面应用程序。
支持13,928个汉字的五笔编码查询，包含字根拆解图片显示功能。

.SH FEATURES
.TP
.B 实时查询
输入即查询，无需按回车键

.TP
.B 多字符支持
支持单字和词组查询，如"中国"、"北京"等

.TP
.B 字根显示
显示每个汉字的字根拆解图片和拆分信息

.TP
.B 现代化界面
美观的用户界面，响应式设计

.SH USAGE
启动应用后，在搜索框中输入要查询的汉字或词组，即可查看详细的编码信息、字根拆解和拼音信息。

.SH EXIT STATUS
.TP
.B 0
正常退出

.TP
.B 1
发生错误退出

.SH AUTHOR
Wubi Query Desktop Team <wubi-query@example.com>

.SH SEE ALSO
项目主页: https://github.com/hantang/search-wubi

.SH COPYRIGHT
2024 Wubi Query Desktop Team. MIT License.
EOF

gzip -c "$DEB_MAN_DIR/$PROJECT_NAME.1" > "$DEB_MAN_DIR/$PROJECT_NAME.1.gz"
rm "$DEB_MAN_DIR/$PROJECT_NAME.1"

# 创建DEBIAN控制文件
echo "创建DEBIAN控制文件..."
cat > "$DEB_CONTROL_DIR/control" << EOF
Package: $PROJECT_NAME
Version: $VERSION
Section: utils
Priority: optional
Architecture: amd64
Depends: libc6, libgcc-s1, libstdc++6, libgl1, libglfw3, libxrandr2, libxinerama1, libxcursor1, libxi6
Maintainer: $MAINTAINER
Description: 五笔输入法编码查询桌面应用
 这是一个基于C++17和ImGui开发的五笔编码查询桌面应用程序，
 支持13,928个汉字的五笔编码查询，包含字根拆解图片显示功能。
 .
 特性:
  * 实时五笔编码查询
  * 支持单字和词组查询
  * 显示字根拆解图片
  * 现代化用户界面
  * 跨平台支持
EOF

# 创建安装脚本
cat > "$DEB_CONTROL_DIR/postinst" << 'EOF'
#!/bin/sh
# 安装后脚本

set -e

if [ "$1" = "configure" ]; then
    # 修复资源文件权限
    chmod -R 755 /usr/share/wubi-query-desktop/assets/ 2>/dev/null || true

    # 确保字体文件可读
    chmod 644 /usr/share/wubi-query-desktop/assets/fonts/* 2>/dev/null || true

    # 确保图片文件可读
    chmod 644 /usr/share/wubi-query-desktop/assets/images/char/* 2>/dev/null || true

    # 更新桌面数据库
    update-desktop-database -q /usr/share/applications || true

    # 更新图标缓存
    gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor || true

    echo "五笔编码查询桌面应用安装完成！"
    echo "您可以在应用菜单中找到 'Wubi Query Desktop'"
    echo "或者在终端中运行: wubi-query-desktop"
fi

exit 0
EOF

# 创建卸载脚本
cat > "$DEB_CONTROL_DIR/prerm" << 'EOF'
#!/bin/sh
# 卸载前脚本

set -e

if [ "$1" = "remove" ]; then
    echo "正在卸载五笔编码查询桌面应用..."
fi

exit 0
EOF

cat > "$DEB_CONTROL_DIR/postrm" << 'EOF'
#!/bin/sh
# 卸载后脚本

set -e

if [ "$1" = "purge" ]; then
    # 更新桌面数据库
    update-desktop-database -q /usr/share/applications || true

    # 更新图标缓存
    gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor || true
fi

exit 0
EOF

# 设置脚本权限
chmod +x "$DEB_CONTROL_DIR/postinst"
chmod +x "$DEB_CONTROL_DIR/prerm"
chmod +x "$DEB_CONTROL_DIR/postrm"

# 计算安装大小
INSTALLED_SIZE=$(du -s "$DEB_ROOT" | cut -f1)

# 更新控制文件中的安装大小
sed -i "s/^Installed-Size:.*/Installed-Size: $INSTALLED_SIZE/" "$DEB_CONTROL_DIR/control"

# 创建DEB包
echo "创建DEB包..."
cd "$DEB_BUILD_DIR"
fakeroot dpkg-deb --build "$PROJECT_NAME"

# 检查生成的包
if [ ! -f "$PROJECT_NAME.deb" ]; then
    echo "错误: DEB包创建失败"
    exit 1
fi

# 复制到release目录
DEB_FILE="$PROJECT_NAME-$VERSION-amd64.deb"
mv -f "$PROJECT_NAME.deb" "$RELEASE_DIR/$DEB_FILE"

# 检查包的完整性
echo "检查包的完整性..."
dpkg-deb --info "$RELEASE_DIR/$DEB_FILE"
dpkg-deb --contents "$RELEASE_DIR/$DEB_FILE" | head -10

echo "=========================================="
echo "DEB包创建完成！"
echo "文件位置: $RELEASE_DIR/$DEB_FILE"
ls -lh "$RELEASE_DIR/$DEB_FILE"

# 列出release目录中的所有包
echo ""
echo "Release目录中的所有包:"
ls -lh "$RELEASE_DIR/"*.deb 2>/dev/null || echo "  暂无其他包"

echo "=========================================="
echo "从release目录安装:"
cd "$RELEASE_DIR"
if command -v gdebi >/dev/null 2>&1; then
    echo "推荐安装命令: sudo gdebi $DEB_FILE"
    echo "              (gdebi 会自动处理依赖关系)"
else
    echo "安装命令: sudo dpkg -i $DEB_FILE"
    echo "          sudo apt-get install -f  # 解决依赖问题"
    echo ""
    echo "建议安装 gdebi 以获得更好的安装体验:"
    echo "sudo apt-get install gdebi"
fi
echo "卸载命令: sudo dpkg -r $PROJECT_NAME"
echo "          sudo dpkg -P $PROJECT_NAME  # 完全删除包括配置"
echo ""
echo "安装后可以在应用菜单中找到 'Wubi Query Desktop'"
echo "或者在终端中运行: wubi-query-desktop"
cd "$PROJECT_ROOT"