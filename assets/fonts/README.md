# 开源跨平台字体库

这个目录包含项目使用的开源字体文件。

## 字体文件

### 1. 文泉驿正黑 (WenQuanYi Zen Hei)
- **文件**: `wqy-zenhei.ttc`
- **许可证**: GPLv2 with font exception
- **支持**: 完整的中日韩字符集

### 2. 文泉驿微米黑 (WenQuanYi Micro Hei)
- **文件**: `wqy-microhei.ttc`
- **许可证**: GPLv2 with font exception
- **支持**: 简化的中日韩字符集

### 3. DejaVu Sans
- **文件**: `DejaVuSans.ttf`
- **许可证**: Bitstream Vera License
- **支持**: 拉丁字符和基本符号

## 使用方法

应用程序按以下优先级加载字体：
1. `wqy-zenhei.ttc` - 首选中文字体
2. `wqy-microhei.ttc` - 轻量级中文字体
3. `DejaVuSans.ttf` - 备用字体

## 获取方式

运行下载脚本：
```bash
./scripts/download_fonts.sh --system-only
```

## 许可证

所有字体都是开源的，可以自由使用和分发。