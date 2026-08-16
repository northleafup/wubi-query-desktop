# 问题记录：Tauri v2 前端无法调用 Rust 后端命令

**创建日期**: 2026-08-16
**状态**: 待解决
**优先级**: 高（核心功能阻塞）

## 问题描述

迁移至 Tauri v2 后，前端页面能正常加载，但点击"查询"按钮后无任何响应，随后窗口闪退。

## 现象

1. 应用启动后标题栏显示"加载中..."（`get_char_count` 调用未返回）
2. 输入汉字点击"查询"后页面无结果，随后窗口闪退
3. 前端黄色调试状态栏无内容（连 `__TAURI__` 变量检测都未执行）
4. 多次按 ESC 键后必然闪退

## 已排查/尝试

- [x] `convertFileSrc` 路径问题 → 已改为 base64 图片传输，问题仍存在
- [x] ESC 键 JS 处理导致崩溃 → 已移除 JS 端的 ESC 处理，问题仍存在
- [x] CSP 策略阻止脚本执行 → 已放宽 CSP（添加 `'unsafe-eval'`），问题仍存在
- [x] 确认 Rust 后端正常加载数据（6764 个汉字）
- [x] 确认 `cargo build` 编译通过无错误
- [ ] 未确认 `window.__TAURI__` 是否存在
- [ ] 未确认 `tauri.conf.json` 的 `build.devUrl` 是否影响打包后运行
- [ ] 未确认 Tauri v2 `generate_context!` 宏是否正确读取配置

## 可能原因

### 1. 前端 JS 未加载/未执行
- CSP 策略可能仍然过严
- `index.html` 中 `<script src="app.js">` 路径可能有问题
- Tauri v2 对本地文件协议的 CSP 默认限制

### 2. IPC 通信失败
- `tauri.conf.json` 中 `identifier` 配置可能与 capabilities 不匹配
- `capabilities/default.json` 权限配置可能缺少 `core:command:default`
- Tauri v2 的 Command 注册机制与 v1 不同

### 3. 窗口闪退原因
- 可能是 WebView 渲染异常
- 可能是 Rust 端 panic 未捕获（`expect` 调用）
- 可能是内存问题（base64 传输大文件）

## 解决方向

1. **最小化复现**：创建一个最简单的 Tauri v2 项目，验证 IPC 是否正常
2. **检查 Tauri v2 文档**：确认 `invoke` 的正确调用方式和权限配置
3. **检查 capabilities**：确认 `default.json` 包含 `core:default` 和 `core:command:default`
4. **检查 devUrl**：打包运行时 `devUrl` 配置可能导致问题
5. **添加 Rust 端 panic handler**：捕获并输出错误信息

## 相关文件

- `src-tauri/src/lib.rs` — Rust 后端入口
- `src-tauri/src/data.rs` — 数据层
- `src-tauri/tauri.conf.json` — Tauri 配置
- `src-tauri/capabilities/default.json` — 权限配置
- `ui/index.html` — 前端页面
- `ui/app.js` — 前端逻辑
- `ui/style.css` — 前端样式
