## Purpose

通过 Tauri asset 协议向前端提供字根 GIF 图片文件的访问，使前端可以用标准 HTML `<img>` 标签渲染字根拆解图片。

## ADDED Requirements

### Requirement: GIF 图片可通过 asset 协议访问
系统 SHALL 配置 Tauri asset 作用域，允许前端通过 `asset://` 或 Tauri 提供的资源访问协议加载 `assets/images/char/` 目录下的 GIF 文件。

#### Scenario: 前端渲染字根图片
- **WHEN** 前端需要显示某汉字的字根拆解图
- **THEN** 前端通过 `<img>` 标签加载对应的 GIF 文件，浏览器原生渲染 GIF 内容

#### Scenario: 图片文件不存在
- **WHEN** 前端请求的 GIF 文件不存在
- **THEN** `<img>` 标签显示加载失败的占位内容，应用不崩溃

### Requirement: 资源目录跨平台路径解析
系统 SHALL 在 macOS、Windows、Linux 三平台上正确定位 `assets/images/char/` 目录，无论应用以开发模式还是打包安装模式运行。

#### Scenario: 开发模式运行
- **WHEN** 通过 `npm run tauri dev` 启动应用
- **THEN** 系统从项目根目录的 `assets/images/char/` 加载图片

#### Scenario: 打包安装后运行
- **WHEN** 用户安装并运行打包后的应用（.app / .msi / .deb）
- **THEN** 系统从安装目录内的 `assets/images/char/` 加载图片
