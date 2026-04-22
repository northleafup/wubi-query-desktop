## ADDED Requirements

### Requirement: Conditional asset copying
POST_BUILD 命令 SHALL 仅在源文件比目标文件新时复制资源文件，避免不必要的 IO 操作。

#### Scenario: File unchanged
- **WHEN** 源资源文件未修改且目标文件已存在
- **THEN** 不执行复制操作，构建日志不显示复制信息

#### Scenario: File changed
- **WHEN** 源资源文件修改时间晚于目标文件
- **THEN** 执行复制操作，目标文件被更新

### Requirement: Explicit asset file lists
资产复制命令 SHALL 使用明确的文件列表或通配符配合 `copy_if_different`，而非 `copy_directory` 全盘复制。

#### Scenario: Fonts copied
- **WHEN** POST_BUILD 阶段执行
- **THEN** `assets/fonts/*.ttf` 和 `*.ttc` 被复制到构建目录

#### Scenario: Data files copied
- **WHEN** POST_BUILD 阶段执行
- **THEN** `assets/data/*.tsv` 被复制到构建目录

#### Scenario: Images copied efficiently
- **WHEN** POST_BUILD 阶段执行
- **THEN** `assets/images/char/` 下的图片文件仅在变动时被复制

### Requirement: No error suppression
资产复制命令 SHALL 不使用 `|| true` 掩盖错误，改用条件判断或 `copy_if_different` 处理文件不存在的情况。

#### Scenario: Missing font files
- **WHEN** `assets/fonts/` 目录下无 `.ttf` 或 `.ttc` 文件
- **THEN** 构建不失败，不显示错误信息
