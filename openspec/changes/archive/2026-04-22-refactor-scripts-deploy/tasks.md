## 1. 脚本重构

- [x] 1.1 创建 `scripts/deploy.sh` 框架，包含平台检测逻辑
- [x] 1.2 将 `build_ubuntu_deb.sh` 的构建和打包逻辑迁移至 `deploy.sh` 的 Ubuntu 分支
- [x] 1.3 将 `build_macos_pkg.sh` 的构建和打包逻辑迁移至 `deploy.sh` 的 macOS 分支
- [x] 1.4 实现进程查杀功能 (killall wubi-query-desktop)
- [x] 1.5 实现旧版本卸载功能 (dpkg -r / pkgutil --forget / rm)
- [x] 1.6 实现新版本安装功能 (dpkg -i / installer)
- [x] 1.7 删除旧脚本 (`build_ubuntu_deb.sh`, `build_macos_pkg.sh`, `uninstall_ubuntu_deb.sh`)

## 2. 验证

- [x] 2.1 运行 `./scripts/deploy.sh` (模拟 Ubuntu 环境)，验证全流程无报错
- [x] 2.2 确认 `scripts/` 目录下仅保留 `run.sh` 和 `deploy.sh`
