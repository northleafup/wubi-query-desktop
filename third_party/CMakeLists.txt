# 第三方库构建文件

# 添加GLFW子目录
add_subdirectory(glfw)

# 设置GLFW为静态库
set_target_properties(glfw PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    CXX_STANDARD 17
)