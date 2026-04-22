## Context

当前项目结构存在以下问题：
1. `src/ImageLoader.h` 位于 `src/` 根目录，而其他头文件均在 `src/include/` 下，位置不统一
2. `CMakeLists.txt` 使用全局 `include_directories`，不符合现代 CMake 最佳实践
3. `POST_BUILD` 阶段使用 `copy_directory` 复制 6764 张图片，每次构建都执行，速度较慢
4. 资产复制命令使用 `|| true` 掩盖潜在错误
5. 项目缺少 `tests/` 目录和测试框架支持

## Goals / Non-Goals

**Goals:**
- 统一头文件位置，将所有 `.h` 文件移至 `src/include/`
- 使用 `target_include_directories` 替代 `include_directories`，按目标指定包含目录
- 优化资源复制逻辑，仅在文件变动时复制，移除 `|| true`
- 添加 `tests/` 目录结构和基础 CMake 测试配置

**Non-Goals:**
- 不引入外部测试框架（如 Google Test），仅保留目录结构和占位配置
- 不改变现有构建输出目录或可执行文件名称
- 不修改现有源代码逻辑或 UI 行为

## Decisions

1. **头文件位置统一**：将 `ImageLoader.h` 移至 `src/include/`，并更新 `CMakeLists.txt` 和源文件中的 `#include` 路径。这保持了一致性，便于 IDE 识别和维护。

2. **Modern CMake 实践**：使用 `target_include_directories(<target> PUBLIC|PRIVATE ...)` 替代全局 `include_directories`。这避免了污染全局命名空间，使依赖关系更清晰。

3. **资源复制优化**：将 `copy_directory` 替换为逐个文件复制或使用 `copy_if_different` 配合文件列表。CMake 的 `copy_if_different` 仅在源文件比目标文件新时才复制，避免不必要的 IO。

4. **测试目录结构**：创建 `tests/` 目录和 `tests/CMakeLists.txt`，但暂不引入具体测试框架，仅预留 `enable_testing()` 和 `add_subdirectory(tests)` 配置。

## Risks / Trade-offs

- [资源复制逻辑变更可能导致遗漏文件] → 使用明确的文件列表而非通配符，并在构建日志中输出复制信息
- [移动头文件可能影响某些 IDE 的索引] → 更新 CMakeLists.txt 中的头文件列表，确保 IDE 能正确识别
- [移除 `|| true` 可能在某些环境下导致构建失败] → 改用条件判断或 `copy_if_different` 确保命令在文件不存在时优雅处理
