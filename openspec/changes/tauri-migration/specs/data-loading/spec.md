## Purpose

管理五笔数据文件的加载、解析和内存索引构建，为前端查询提供数据基础。

## ADDED Requirements

### Requirement: TSV 数据文件解析
系统 SHALL 在启动时解析 `assets/data/data-wubi-v86.tsv`（五笔编码数据）和 `assets/data/data-chars.tsv`（汉字基本信息数据）两个 TSV 格式文件，构建内存中的汉字索引。

#### Scenario: 正常加载数据文件
- **WHEN** 应用启动且 TSV 文件存在于 assets 目录中
- **THEN** 系统解析完成，可通过查询接口检索到已加载的汉字数据

#### Scenario: 数据文件缺失
- **WHEN** 应用启动但 TSV 文件不存在
- **THEN** 系统向前端返回错误状态，前端显示"数据加载失败"提示

### Requirement: 数据字段完整性
系统 SHALL 为每个汉字保留以下字段：汉字字符、五笔全码、简码列表、容错码列表、拼音、笔画数、字根拆解描述、是否有字根图片、字根图片路径。

#### Scenario: 五笔编码数据字段映射
- **WHEN** 解析 `data-wubi-v86.tsv` 的一行数据
- **THEN** 系统正确提取 char(汉字)、code(全码)、code_short(简码)、code_more(容错码)、segments(字根拆解) 字段

#### Scenario: 汉字基本信息字段映射
- **WHEN** 解析 `data-chars.tsv` 的一行数据
- **THEN** 系统正确提取 char(汉字)、strokes(笔画数)、pinyin(拼音) 字段，并与已有的五笔编码数据合并

### Requirement: 仅保留有图片的汉字
系统 SHALL 扫描 `assets/images/char/` 目录下的 GIF 文件，过滤掉没有对应字根图片的汉字，只保留有图片的汉字数据。

#### Scenario: 过滤无图片汉字
- **WHEN** 数据加载完成
- **THEN** 内存索引中只包含 `assets/images/char/` 目录下有对应 `.gif` 文件的汉字

#### Scenario: 图片路径正确生成
- **WHEN** 某汉字有对应的 GIF 文件（如 `五.gif`）
- **THEN** 该汉字的图片路径 SHALL 指向可被前端访问的资源位置
