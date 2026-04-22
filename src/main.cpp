#include <iostream>
#include <exception>
#include <csignal>

#include "Application.h"

/**
 * 信号处理函数
 * 用于优雅地处理程序终止信号
 */
void signalHandler(int signal) {
    std::cout << "\n收到信号 " << signal << "，正在退出程序..." << std::endl;
    exit(0);
}

int main() {
    try {
        std::cout << "=== 五笔编码查询启动 ===" << std::endl;
        std::cout << "版本: 1.0.0" << std::endl;
        std::cout << "构建日期: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "=========================" << std::endl;

        // 设置信号处理
        signal(SIGINT, signalHandler);   // Ctrl+C
        signal(SIGTERM, signalHandler);  // 终止信号

        // 创建应用程序实例
        Application app;

        // 初始化应用程序
        std::cout << "\n正在初始化应用程序..." << std::endl;
        if (!app.initialize()) {
            std::cerr << "应用程序初始化失败" << std::endl;
            return -1;
        }

        // 设置应用程序信息
        app.setApplicationName("五笔编码查询 v1.0");

        // 运行应用程序
        std::cout << "\n开始运行应用程序..." << std::endl;
        std::cout << "提示: 按 ESC 键或关闭窗口退出" << std::endl;
        std::cout << "=========================" << std::endl;

        int result = app.run();

        std::cout << "\n应用程序正常退出" << std::endl;
        return result;

    } catch (const std::exception& e) {
        std::cerr << "程序异常: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "程序未知异常" << std::endl;
        return -1;
    }
}