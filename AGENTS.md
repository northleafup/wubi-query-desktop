# AGENTS.md — wubi-query-desktop

## 关键约束
- 本目录下所有操作允许；禁止使用绝对路径，必须使用相对路径
- 项目基于 Tauri v2 技术栈：Rust 后端 + 纯 HTML/CSS/JS 前端

## 开发者命令
```bash
# 开发模式运行
cd src-tauri && cargo run

# 或从项目根目录（需先安装依赖）
npm install
npx tauri dev

# 打包构建
npx tauri build
```

## 项目结构
```
src-tauri/                  # Rust 后端
  src/
    main.rs                 # Tauri 入口
    lib.rs                  # Tauri 配置 + Command 注册
    data.rs                 # TSV 数据解析 + 汉字查询
  Cargo.toml                # Rust 依赖配置
  tauri.conf.json           # Tauri 窗口/打包/安全配置
  capabilities/             # 权限配置
  icons/                    # 应用图标（Tauri 生成，含 icns/ico）
ui/                         # 前端（纯 HTML/CSS/JS）
  index.html                # 主页面
  style.css                 # 样式
  app.js                    # 查询逻辑
resources/                  # 运行时资源
  data/                     # TSV 数据文件（被 gitignore）
    data-wubi-v86.tsv       # 五笔编码数据
    data-chars.tsv          # 汉字基本信息
  images/char/              # 6764 张字根 GIF（被 gitignore）
docs/                       # 开发文档
  ISSUES.md                 # 问题记录
  BUILD.md                  # 构建指南
```

## 数据说明
- `data-wubi-v86.tsv`: TSV 格式，字段：char, code, code_short, code_more, key_unit, stroke_unit, char_unit, units, flag, segments
- `data-chars.tsv`: TSV 格式，字段：unicode, char, group, level, ids, strokes, pinyin, stroke_seq, radical, freq, basic
- 启动时仅加载有对应 GIF 图片的汉字（约 6764 个）

## 测试
- 项目没有单元测试
- 验证方式：运行应用，手动测试查询功能

## 打包
- macOS: `npx tauri build` → 生成 `.app` / `.dmg`
- Windows: `npx tauri build` → 生成 `.msi` / `.exe`
- Linux: `npx tauri build` → 生成 `.deb` / `.AppImage`
- 打包产物输出到 `src-tauri/target/release/bundle/`

## 注意事项
- 中文渲染由系统 WebView 原生支持，无需额外字体
- GIF 图片通过 Tauri asset 协议访问，前端用 `<img>` 标签直接渲染
- 退出方式：ESC 键或关闭窗口
- 资源文件（TSV、GIF）被 .gitignore 忽略，需单独获取
