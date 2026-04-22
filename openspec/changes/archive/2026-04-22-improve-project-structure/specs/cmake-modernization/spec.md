## ADDED Requirements

### Requirement: Target-based include directories
CMakeLists.txt SHALL 使用 `target_include_directories` 为每个目标指定包含目录，替代全局 `include_directories`。

#### Scenario: imgui library includes
- **WHEN** 编译 `imgui` 静态库
- **THEN** `imgui` 目标通过 `target_include_directories` 获取 GLFW 和 ImGui 头文件路径

#### Scenario: Main executable includes
- **WHEN** 编译 `wubi-query-desktop` 可执行文件
- **THEN** 可执行文件目标通过 `target_include_directories` 获取 `src/include/`、`third_party/imgui` 和 `third_party/glfw/include` 路径

### Requirement: Header file organization
所有头文件 SHALL 位于 `src/include/` 目录下，源文件目录 `src/` 中不应存在 `.h` 文件。

#### Scenario: ImageLoader.h location
- **WHEN** 查看项目结构
- **THEN** `ImageLoader.h` 位于 `src/include/` 而非 `src/` 根目录

#### Scenario: Include paths updated
- **WHEN** 源文件引用 `ImageLoader.h`
- **THEN** `#include` 语句使用正确路径（如 `#include "ImageLoader.h"` 或相对路径）
