## Why

项目当前目录结构和构建配置存在多处不规范之处，包括头文件位置混乱、CMake 使用过时的全局 `include_directories`、资源文件复制效率低、缺少测试目录等。这些问
题影响代码可维护性、构建速度和开发体验。

## What Changes

- 将 `src/ImageLoader.h` 移至 `src/include/` 目录，保持头文件统一位置
- 将 `CMakeLists.txt` 中的 `include_directories` 替换为 `target_include_directories`，采用更现代的 CMake 实践
- 优化资源文件复制逻辑：移除 `|| true` 错误掩盖，仅在文件变动时复制图片
- 新增 `tests/` 目录结构和基础测试框架支持
- 改进 `CMakeLists.txt` 中资产复制命令，使用更精确的条件判断

## Capabilities

### New Capabilities
- `test-framework`: 添加单元测试目录结构和 CMake 测试支持配置
- `asset-copy-optimization`: 优化构建时资源文件复制策略，避免不必要的重复复制
- `cmake-modernization`: 将 CMake 配置从全局 include_directories 升级到 target_include_directories

### Modified Capabilities
<!-- No existing specs to modify -->

## Impact

- `CMakeLists.txt`: 主要修改文件，包含包含目录、复制命令和测试支持
- `src/ImageLoader.h`: 移动到 `src/include/`
- `src/include/`: 新增 `ImageLoader.h`
- `tests/`: 新增测试目录及基础配置
- 构建脚本和运行脚本无需修改
