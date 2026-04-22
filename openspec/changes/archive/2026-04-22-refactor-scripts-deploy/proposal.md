## Why

现有的多个独立脚本（构建、打包、卸载）使得维护和分发变得繁琐，且缺乏一键更新的能力。通过合并并增强脚本，提供跨平台的一键构建、清理、安装流程，可以显著提升开发迭代效率和分发体验。

## What Changes

- **删除**: `scripts/build_ubuntu_deb.sh`, `scripts/build_macos_pkg.sh`, `scripts/uninstall_ubuntu_deb.sh`。
- **新增**: `scripts/install.sh`，整合以下功能：
  - 自动识别平台（macOS/Ubuntu）。
  - 自动清理旧构建 (`build/` 目录)。
  - 生成当前平台的安装包（DEB 或 PKG）。
  - 查杀正在运行的程序进程。
  - 卸载旧版本软件。
  - 自动安装新生成的软件包。
- **保留**: `scripts/run.sh`。

## Capabilities

### New Capabilities
- `unified-installer`: 跨平台的一键构建、打包、清理、卸载和安装脚本，支持 macOS 和 Ubuntu 环境。

### Modified Capabilities
<!-- None -->

## Impact

- `scripts/` 目录结构大幅简化，仅保留 `run.sh` 和 `install.sh`。
- 开发者和用户只需运行 `./scripts/install.sh` 即可完成全量更新，无需手动执行多条命令。
- 自动化处理了旧进程残留和卸载不干净的问题。
