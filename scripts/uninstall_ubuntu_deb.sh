#!/bin/sh

# Ubuntu DEB包卸载脚本
# 用于完全卸载五笔编码查询桌面应用程序

# 项目信息
PROJECT_NAME="wubi-query-desktop"
APP_NAME="五笔编码查询"
VERSION="1.0.0"

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo "=== Ubuntu DEB 包卸载脚本 ==="
echo "应用名称: $APP_NAME"
echo "包名: $PROJECT_NAME"
echo ""

# 检查是否为root用户
check_root() {
    if [ "$(id -u)" -ne 0 ]; then
        echo -e "${RED}错误: 此脚本需要root权限运行${NC}"
        echo "请使用: sudo $0"
        exit 1
    fi
}

# 检查包是否已安装
check_installed() {
    if ! dpkg -l | grep -q "^ii.*$PROJECT_NAME "; then
        echo -e "${YELLOW}警告: $PROJECT_NAME 未安装${NC}"

        # 检查是否有残留文件
        check_residual_files

        echo "如果您想强制清理所有相关文件，请运行: sudo $0 --force"
        exit 0
    fi
}

# 检查残留文件
check_residual_files() {
    echo ""
    echo "检查残留文件..."

    residual_found=false

    # 检查可执行文件
    if [ -f "/usr/bin/$PROJECT_NAME" ]; then
        echo -e "${YELLOW}发现残留可执行文件: /usr/bin/$PROJECT_NAME${NC}"
        residual_found=true
    fi

    # 检查共享文件目录
    if [ -d "/usr/share/$PROJECT_NAME" ]; then
        echo -e "${YELLOW}发现残留数据目录: /usr/share/$PROJECT_NAME${NC}"
        residual_found=true
    fi

    # 检查桌面文件
    if [ -f "/usr/share/applications/$PROJECT_NAME.desktop" ]; then
        echo -e "${YELLOW}发现残留桌面文件: /usr/share/applications/$PROJECT_NAME.desktop${NC}"
        residual_found=true
    fi

    # 检查图标文件
    if [ -f "/usr/share/icons/hicolor/256x256/apps/$PROJECT_NAME.png" ]; then
        echo -e "${YELLOW}发现残留图标文件: /usr/share/icons/hicolor/256x256/apps/$PROJECT_NAME.png${NC}"
        residual_found=true
    fi

    if [ "$residual_found" = "false" ]; then
        echo -e "${GREEN}未发现残留文件${NC}"
    fi
}

# 清理残留文件
cleanup_residual_files() {
    echo ""
    echo "清理残留文件..."

    # 删除可执行文件
    if [ -f "/usr/bin/$PROJECT_NAME" ]; then
        echo "删除可执行文件: /usr/bin/$PROJECT_NAME"
        rm -f "/usr/bin/$PROJECT_NAME"
    fi

    # 删除共享文件目录
    if [ -d "/usr/share/$PROJECT_NAME" ]; then
        echo "删除数据目录: /usr/share/$PROJECT_NAME"
        rm -rf "/usr/share/$PROJECT_NAME"
    fi

    # 删除桌面文件
    if [ -f "/usr/share/applications/$PROJECT_NAME.desktop" ]; then
        echo "删除桌面文件: /usr/share/applications/$PROJECT_NAME.desktop"
        rm -f "/usr/share/applications/$PROJECT_NAME.desktop"
    fi

    # 删除图标文件
    if [ -f "/usr/share/icons/hicolor/256x256/apps/$PROJECT_NAME.png" ]; then
        echo "删除图标文件: /usr/share/icons/hicolor/256x256/apps/$PROJECT_NAME.png"
        rm -f "/usr/share/icons/hicolor/256x256/apps/$PROJECT_NAME.png"
    fi

    # 删除文档目录
    if [ -d "/usr/share/doc/$PROJECT_NAME" ]; then
        echo "删除文档目录: /usr/share/doc/$PROJECT_NAME"
        rm -rf "/usr/share/doc/$PROJECT_NAME"
    fi

    # 删除手册页
    if [ -f "/usr/share/man/man1/$PROJECT_NAME.1.gz" ]; then
        echo "删除手册页: /usr/share/man/man1/$PROJECT_NAME.1.gz"
        rm -f "/usr/share/man/man1/$PROJECT_NAME.1.gz"
    fi
}

# 更新系统缓存
update_system_caches() {
    echo ""
    echo "更新系统缓存..."

    # 更新桌面数据库
    echo "更新桌面数据库..."
    update-desktop-database -q /usr/share/applications 2>/dev/null || true

    # 更新图标缓存
    echo "更新图标缓存..."
    gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor 2>/dev/null || true

    # 重建手册页索引
    echo "重建手册页索引..."
    mandb -q 2>/dev/null || true
}

# 显示卸载前信息
show_uninstall_info() {
    echo "包信息:"
    dpkg -l | grep "$PROJECT_NAME" | grep "^ii"
    echo ""

    # 显示安装的文件
    echo "已安装的文件:"
    dpkg -L "$PROJECT_NAME" 2>/dev/null | head -20
    file_count=$(dpkg -L "$PROJECT_NAME" 2>/dev/null | wc -l)
    if [ "$file_count" -gt 20 ]; then
        echo "  ... (共 $file_count 个文件)"
    fi
    echo ""
}

# 执行卸载
execute_uninstall() {
    echo -e "${BLUE}开始卸载 $PROJECT_NAME...${NC}"
    echo ""

    # 首先尝试使用dpkg删除
    echo "步骤1: 删除软件包..."
    if dpkg -r "$PROJECT_NAME"; then
        echo -e "${GREEN}软件包删除成功${NC}"
    else
        echo -e "${YELLOW}警告: dpkg删除失败，尝试强制删除...${NC}"
        dpkg -r --force-depends "$PROJECT_NAME" || echo -e "${RED}强制删除也失败了${NC}"
    fi
    echo ""

    # 清理配置文件
    echo "步骤2: 清理配置文件..."
    if dpkg -P "$PROJECT_NAME" 2>/dev/null; then
        echo -e "${GREEN}配置文件清理成功${NC}"
    else
        echo -e "${YELLOW}警告: 配置文件清理失败，可能需要手动清理${NC}"
    fi
    echo ""

    # 清理残留文件
    echo "步骤3: 清理残留文件..."
    cleanup_residual_files
    echo ""

    # 更新系统缓存
    echo "步骤4: 更新系统缓存..."
    update_system_caches
    echo ""

    # 检查卸载结果
    echo "步骤5: 验证卸载结果..."
    if dpkg -l | grep -q "^rc.*$PROJECT_NAME "; then
        echo -e "${YELLOW}发现残留配置，正在清理...${NC}"
        dpkg -P "$PROJECT_NAME" 2>/dev/null || true
    fi

    if dpkg -l | grep -q "$PROJECT_NAME "; then
        echo -e "${RED}卸载不完全，包管理器中仍有记录${NC}"
        dpkg -l | grep "$PROJECT_NAME"
    else
        echo -e "${GREEN}卸载成功！${NC}"
    fi
}

# 显示使用帮助
show_help() {
    echo "用法: $0 [选项]"
    echo ""
    echo "选项:"
    echo "  -h, --help     显示此帮助信息"
    echo "  --dry-run      仅显示将要执行的操作，不实际执行"
    echo "  --force        强制清理，即使包未安装也清理残留文件"
    echo "  --purge-only   仅清理配置文件和残留文件"
    echo ""
    echo "此脚本用于完全卸载五笔编码查询桌面应用程序"
    echo "包括删除程序文件、配置文件、桌面快捷方式和图标等"
    echo ""
}

# 干运行模式
dry_run() {
    echo -e "${BLUE}=== 干运行模式 ===${NC}"
    echo "这些操作将会执行（但不会实际执行）："
    echo ""

    if dpkg -l | grep -q "^ii.*$PROJECT_NAME "; then
        echo "1. 删除软件包: dpkg -r $PROJECT_NAME"
        echo "2. 清理配置文件: dpkg -P $PROJECT_NAME"
    else
        echo "1. 软件包未安装，跳过包管理器操作"
    fi

    echo "3. 清理以下残留文件:"

    # 检查并显示将要删除的文件
    [ -f "/usr/bin/$PROJECT_NAME" ] && echo "   - /usr/bin/$PROJECT_NAME"
    [ -d "/usr/share/$PROJECT_NAME" ] && echo "   - /usr/share/$PROJECT_NAME (目录)"
    [ -f "/usr/share/applications/$PROJECT_NAME.desktop" ] && echo "   - /usr/share/applications/$PROJECT_NAME.desktop"
    [ -f "/usr/share/icons/hicolor/256x256/apps/$PROJECT_NAME.png" ] && echo "   - /usr/share/icons/hicolor/256x256/apps/$PROJECT_NAME.png"
    [ -d "/usr/share/doc/$PROJECT_NAME" ] && echo "   - /usr/share/doc/$PROJECT_NAME (目录)"
    [ -f "/usr/share/man/man1/$PROJECT_NAME.1.gz" ] && echo "   - /usr/share/man/man1/$PROJECT_NAME.1.gz"

    echo ""
    echo "4. 更新系统缓存:"
    echo "   - update-desktop-database"
    echo "   - gtk-update-icon-cache"
    echo "   - mandb"
    echo ""
    echo -e "${GREEN}干运行完成，没有实际执行任何操作${NC}"
}

# 仅清理模式
purge_only() {
    echo -e "${BLUE}=== 仅清理模式 ===${NC}"
    echo "清理配置文件和残留文件..."

    # 清理配置文件
    if dpkg -l | grep -q "^rc.*$PROJECT_NAME "; then
        echo "清理包管理器中的配置记录..."
        dpkg -P "$PROJECT_NAME" 2>/dev/null || echo -e "${YELLOW}清理配置记录失败${NC}"
    fi

    # 清理残留文件
    cleanup_residual_files

    # 更新系统缓存
    update_system_caches

    echo -e "${GREEN}清理完成${NC}"
}

# 主函数
main() {
    case "${1:-}" in
        -h|--help)
            show_help
            exit 0
            ;;
        --dry-run)
            check_root
            dry_run
            exit 0
            ;;
        --force)
            check_root
            echo -e "${BLUE}强制清理模式${NC}"
            execute_uninstall
            exit 0
            ;;
        --purge-only)
            check_root
            purge_only
            exit 0
            ;;
        "")
            # 默认卸载模式
            check_root
            check_installed
            show_uninstall_info

            echo -e "${YELLOW}确认要卸载 $PROJECT_NAME 吗？${NC}"
            echo -e "${YELLOW}这将删除应用程序和所有相关文件${NC}"
            printf "输入 'yes' 继续: "
            read -r confirmation
            echo ""

            if [ "$confirmation" = "yes" ]; then
                execute_uninstall
            else
                echo -e "${YELLOW}卸载已取消${NC}"
                exit 0
            fi
            ;;
        *)
            echo -e "${RED}错误: 未知选项 '$1'${NC}"
            echo ""
            show_help
            exit 1
            ;;
    esac
}

# 运行主函数
main "$@"