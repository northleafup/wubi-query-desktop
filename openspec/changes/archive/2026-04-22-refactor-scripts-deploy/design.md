## Context

当前存在三个独立的脚本处理构建、打包和卸载。macOS 和 Ubuntu 的打包和安装方式完全不同，导致代码重复且逻辑分散。新的 `install.sh` 需要在一个脚本中处理所有平台的差异，并提供原子化的更新体验。

## Goals / Non-Goals

**Goals:**
- 实现单入口脚本 `install.sh`，自动处理平台差异。
- 包含构建、清理、杀进程、卸载、安装的全生命周期管理。
- 兼容 macOS (PKG) 和 Ubuntu (DEB) 的原生工具链。

**Non-Goals:**
- 不引入额外的包管理器（如 brew 或 apt 安装脚本本身）。
- 不处理 Windows 平台（保持项目原有定位）。

## Decisions

1. **平台检测**: 使用 `uname -s` 进行平台判断。`Darwin` 对应 macOS，`Linux` 对应 Ubuntu。
2. **进程查杀**: 使用 `killall wubi-query-desktop` 确保程序在更新前已完全退出，防止文件被占用。
3. **卸载逻辑**:
   - **Ubuntu**: 使用 `sudo dpkg -r wubi-query-desktop` 和 `purge` 清理残留。
   - **macOS**: 使用 `sudo pkgutil --forget` 清理 pkg 记录，并手动删除 `/Applications/五笔编码查询.app` 和资源目录。
4. **构建清理**: 脚本开始时强制删除 `build/` 目录，确保环境纯净。

## Risks / Trade-offs

- [权限问题] → 脚本中涉及 sudo 的操作会提示用户输入密码，需确保用户具备 sudo 权限。
- [macOS pkgutil 清理] → 如果之前不是通过 pkg 安装的，`pkgutil --forget` 可能会报错，需要添加错误忽略处理。
