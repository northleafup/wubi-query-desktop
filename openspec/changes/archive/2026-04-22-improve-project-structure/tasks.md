## 1. 头文件位置统一

- [x] 1.1 将 `src/ImageLoader.h` 移动到 `src/include/ImageLoader.h`
- [x] 1.2 更新 `CMakeLists.txt` 中 `APP_HEADERS` 列表的 `ImageLoader.h` 路径
- [x] 1.3 更新 `src/ImageLoader.cpp` 中的 `#include` 路径（如需要）
- [x] 1.4 更新其他引用 `ImageLoader.h` 的源文件包含路径

## 2. CMake Modernization - target_include_directories

- [x] 2.1 移除 `CMakeLists.txt` 中的全局 `include_directories` 调用
- [x] 2.2 为 `imgui` 目标添加 `target_include_directories(imgui PUBLIC ...)`
- [x] 2.3 为 `wubi-query-desktop` 目标添加 `target_include_directories(${PROJECT_NAME} PRIVATE ...)`
- [x] 2.4 验证构建成功，无头文件找不到错误

## 3. 资源复制优化

- [x] 3.1 移除 `POST_BUILD` 中的 `|| true` 错误掩盖
- [x] 3.2 将 `copy_directory` 替换为基于文件列表的 `copy_if_different`
- [x] 3.3 使用条件命令处理可能不存在的字体/数据文件
- [x] 3.4 验证构建后资产目录完整且正确

## 4. 测试框架支持

- [x] 4.1 创建 `tests/` 目录
- [x] 4.2 创建 `tests/CMakeLists.txt` 基础配置
- [x] 4.3 在主 `CMakeLists.txt` 中添加 `enable_testing()` 和 `add_subdirectory(tests)`
- [x] 4.4 验证 `ctest` 命令可正常运行（即使没有测试用例）

## 5. 验证与清理

- [x] 5.1 运行 `./scripts/build.sh` 验证完整构建流程
- [x] 5.2 运行 `./scripts/run.sh` 验证程序正常启动
- [x] 5.3 检查构建日志确认资源复制仅在有变动时执行
- [x] 5.4 确认所有头文件位于 `src/include/` 目录
