## ADDED Requirements

### Requirement: Test directory structure
项目 SHALL 包含 `tests/` 目录，用于存放所有单元测试和集成测试文件。

#### Scenario: Directory exists
- **WHEN** 查看项目根目录
- **THEN** `tests/` 目录存在且为空（预留结构）

### Requirement: Test CMake configuration
`CMakeLists.txt` SHALL 启用测试支持，包含 `enable_testing()` 和 `add_subdirectory(tests)` 指令。

#### Scenario: Testing enabled
- **WHEN** CMake 配置阶段执行
- **THEN** `enable_testing()` 被调用，测试框架初始化

### Requirement: Tests CMakeLists
`tests/` 目录 SHALL 包含独立的 `CMakeLists.txt`，用于定义测试目标和运行配置。

#### Scenario: Tests subdirectory configured
- **WHEN** 主 `CMakeLists.txt` 执行 `add_subdirectory(tests)`
- **THEN** `tests/CMakeLists.txt` 被正确加载，无配置错误
