# 问题记录：Tauri v2 前端无法调用 Rust 后端命令

**创建日期**: 2026-08-16
**解决日期**: 2026-08-30
**状态**: ✅ 已解决
**优先级**: 高（核心功能阻塞）

## 问题描述

迁移至 Tauri v2 后，前端页面能正常加载，但点击"查询"按钮后无任何响应，随后窗口闪退。

## 现象

1. 应用启动后标题栏显示"加载中..."（`get_char_count` 调用未返回）
2. 输入汉字点击"查询"后页面无结果，随后窗口闪退
3. 前端黄色调试状态栏无内容（连 `__TAURI__` 变量检测都未执行）
4. 多次按 ESC 键后必然闪退

## 根本原因

**Tauri v2 中 `window.__TAURI__` 默认不会注入。**

在 `tauri.conf.json` 中需要显式设置 `"withGlobalTauri": true` 才能使用全局 API。
由于未配置此项，`window.__TAURI__` 为 `undefined`，导致：
- `invoke` 函数为 `null`
- 所有 IPC 调用失败
- 前端无法与 Rust 后端通信

## 修复内容

### 1. `src-tauri/tauri.conf.json`
添加 `"withGlobalTauri": true` 配置：
```json
{
  "app": {
    "withGlobalTauri": true,
    "windows": [...]
  }
}
```

### 2. `ui/app.js`
- 删除重复的 `log()` 函数定义（原第42-44行）
- 实现图片加载功能：调用 `get_image_base64` 命令获取 base64 图片数据

## 验证结果

✅ IPC 通信正常 — 启动时加载 6764 个汉字
✅ 查询功能正常 — 输入汉字返回正确结果
✅ 图片加载正常 — 有GIF的汉字显示字根拆解图片
