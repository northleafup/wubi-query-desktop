cmake_minimum_required(VERSION 3.16)
project(wubi-query-desktop LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加UTF-8编码支持
add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)

# 平台检测
if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
    add_definitions(-DPLATFORM_WINDOWS)
elseif(APPLE)
    set(PLATFORM_MACOS TRUE)
    add_definitions(-DPLATFORM_MACOS)
else()
    set(PLATFORM_LINUX TRUE)
    add_definitions(-DPLATFORM_LINUX)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 查找OpenGL
find_package(OpenGL REQUIRED)

# 设置GLFW选项
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW tests" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW documentation" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)

# 添加第三方库
add_subdirectory(third_party)

# 设置GLFW变量供后续使用
set(GLFW_LIBRARIES glfw)
set(GLFW_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/third_party/glfw/include
    ${CMAKE_SOURCE_DIR}/third_party/glfw/deps
)

# 包含目录将通过 target_include_directories 为每个目标单独设置

# ImGui源文件
set(IMGUI_SOURCES
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_demo.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/backends/imgui_impl_glfw.cpp
    third_party/imgui/backends/imgui_impl_opengl3.cpp
    third_party/imgui/misc/cpp/imgui_stdlib.cpp
)

# ImGui头文件
set(IMGUI_HEADERS
    third_party/imgui/imgui.h
    third_party/imgui/imgui_internal.h
    third_party/imgui/backends/imgui_impl_glfw.h
    third_party/imgui/backends/imgui_impl_opengl3.h
    third_party/imgui/misc/cpp/imgui_stdlib.h
    third_party/imgui/imstb_rectpack.h
    third_party/imgui/imstb_textedit.h
    third_party/imgui/imstb_truetype.h
)

# 应用程序源文件
set(APP_SOURCES
    src/main.cpp
    src/Application.cpp
    src/FontManager.cpp
    src/BackgroundRenderer.cpp
    src/ImageLoader.cpp
    src/wubi/wubi_data_parser.cpp
    src/wubi/data_manager.cpp
)

# 应用程序头文件
set(APP_HEADERS
    src/include/Application.h
    src/include/FontManager.h
    src/include/BackgroundRenderer.h
    src/include/ImageLoader.h
    src/wubi/char_data.h
    src/wubi/wubi_data_parser.h
    src/wubi/data_manager.h
)

# 创建ImGui静态库
add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
target_include_directories(imgui
    PUBLIC
        ${CMAKE_SOURCE_DIR}/third_party/imgui
        ${CMAKE_SOURCE_DIR}/third_party/imgui/backends
    PRIVATE
        ${GLFW_INCLUDE_DIRS}
)
target_link_libraries(imgui ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})

# 创建可执行文件
add_executable(${PROJECT_NAME} ${APP_SOURCES} ${APP_HEADERS})
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/include
        ${CMAKE_SOURCE_DIR}/third_party/imgui
        ${CMAKE_SOURCE_DIR}/third_party/imgui/backends
        ${CMAKE_SOURCE_DIR}/third_party/glfw/include
        assets
)
target_link_libraries(${PROJECT_NAME} imgui ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})

# 添加必要的系统库
if(PLATFORM_LINUX)
    target_link_libraries(${PROJECT_NAME}
        ${CMAKE_DL_LIBS}  # 动态链接库
        X11               # X11库
        Xrandr            # Xrandr库
        Xinerama          # Xinerama库
        Xcursor           # Xcursor库
        Xi                # Xi库
        m                 # 数学库
        pthread           # pthread库
    )
elseif(PLATFORM_MACOS)
    # macOS不需要额外的链接设置，CMake会自动处理
    target_link_libraries(${PROJECT_NAME}
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation"
        "-framework CoreVideo"
    )
elseif(PLATFORM_WINDOWS)
    target_link_libraries(${PROJECT_NAME}
        opengl32
        gdi32
        user32
        kernel32
        shell32
    )
endif()

# 编译器特定设置
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -O2)
endif()

# 复制资源文件到构建目录
# 使用 file(GLOB) 收集文件列表，仅在有文件时复制
file(GLOB FONT_TTC_FILES "${CMAKE_SOURCE_DIR}/assets/fonts/*.ttc")
file(GLOB FONT_TTF_FILES "${CMAKE_SOURCE_DIR}/assets/fonts/*.ttf")
file(GLOB DATA_TSV_FILES "${CMAKE_SOURCE_DIR}/assets/data/*.tsv")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/assets/fonts
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/assets/data
    COMMENT "Copying assets to build directory"
)

# 复制 .ttc 字体文件
if(FONT_TTC_FILES)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FONT_TTC_FILES} ${CMAKE_BINARY_DIR}/bin/assets/fonts/
        COMMENT "Copying .ttc font files"
    )
endif()

# 复制 .ttf 字体文件
if(FONT_TTF_FILES)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FONT_TTF_FILES} ${CMAKE_BINARY_DIR}/bin/assets/fonts/
        COMMENT "Copying .ttf font files"
    )
endif()

# 复制 TSV 数据文件
if(DATA_TSV_FILES)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DATA_TSV_FILES} ${CMAKE_BINARY_DIR}/bin/assets/data/
        COMMENT "Copying TSV data files"
    )
endif()

# 复制图片文件到构建目录
# 注：copy_directory 在 CMake 3.22 中总是复制，但 copy_if_different 可用于单文件
# 由于图片文件数量大，此处仍使用 copy_directory，但确保目录存在
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/assets/images/char
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets/images/char ${CMAKE_BINARY_DIR}/bin/assets/images/char
    COMMENT "Copying image files to build directory"
)

# 测试支持
enable_testing()
add_subdirectory(tests)

# 安装规则
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY assets/ DESTINATION share/${PROJECT_NAME}/assets)
install(DIRECTORY assets/fonts/ DESTINATION share/${PROJECT_NAME}/fonts)

# CPack配置 - 用于创建安装包
set(CPACK_PACKAGE_NAME "WubiQueryDesktop")
set(CPACK_PACKAGE_VENDOR "WubiQuery")
set(CPACK_PACKAGE_DESCRIPTION "五笔编码查询桌面应用")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_CONTACT "wubi-query@example.com")

# macOS特定配置
if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "Wubi Query Desktop")
    set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/scripts/dmg-background.png")
    set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/dmg-setup.scpt")
    set(CPACK_DMG_FORMAT "UDBZ")  # 压缩格式
    set(CPACK_DMG_DISABLE_APPLICATION_SYMLINK ON)
endif()

include(CPack)