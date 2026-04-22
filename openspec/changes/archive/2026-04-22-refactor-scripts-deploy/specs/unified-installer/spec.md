## ADDED Requirements

### Requirement: Unified Installer Script
项目 SHALL 包含一个 `install.sh` 脚本，用于自动化完成构建、打包、卸载和安装流程。

#### Scenario: Ubuntu Installation
- **WHEN** 用户在 Ubuntu 环境下运行 `install.sh`
- **THEN** 脚本执行 `build_ubuntu_deb.sh` 类似逻辑生成 DEB 包
- **AND** 脚本执行 `dpkg -r` 卸载旧版本
- **AND** 脚本执行 `dpkg -i` 安装新包

#### Scenario: macOS Installation
- **WHEN** 用户在 macOS 环境下运行 `install.sh`
- **THEN** 脚本执行 `build_macos_pkg.sh` 类似逻辑生成 PKG 包
- **AND** 脚本执行 `pkgutil --forget` 清理旧记录
- **AND** 脚本执行 `installer` 安装新包

### Requirement: Process Cleanup
在更新安装前，脚本 SHALL 尝试终止正在运行的程序进程。

#### Scenario: Process Termination
- **WHEN** 脚本检测到 `wubi-query-desktop` 正在运行
- **THEN** 发送终止信号并等待进程退出

### Requirement: Build Cleanup
脚本 SHALL 在构建前清理旧的构建目录。

#### Scenario: Fresh Build
- **WHEN** 脚本开始运行
- **THEN** 删除 `build/` 目录以重新生成
