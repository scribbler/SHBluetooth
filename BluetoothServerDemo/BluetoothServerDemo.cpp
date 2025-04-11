// BluetoothServerDemo.cpp: 目标的源文件。
//

#include "BluetoothServerDemo.h"
#include <ThreadSafeServerSocket.h>
#include <guiddef.h>
#include <iostream>
#include <thread>

std::atomic<bool> running(true);

void onReceive(void* handler, const char* data, int length) {
    std::string receivedData(data, length);
    std::cout << "Received data: " << receivedData << std::endl;
}
void messageLoop() {
    while (running.load()) {
        // 可以在这里处理其他消息或执行其他任务
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main()
{
    std::cout << "BTServer begin!\n";
    ThreadSafeServerSocket serverSocket;

    int port = -1;
    if (!serverSocket.initialize(port)) {
        std::cerr << "Failed to initialize server socket on port " << port << std::endl;
        return -1;
    }

    serverSocket.startAccepting(nullptr, onReceive, &serverSocket);

    std::cout << "Server is accepting connections on port " << port << std::endl;

    // 启动消息循环线程
    std::thread loopThread(messageLoop);

    std::cout << "Press Enter to stop the server..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    serverSocket.stopAccepting();
    std::cin.get();

    // 停止消息循环
    running.store(false);
    loopThread.join();


    std::cout << "Server stopped." << std::endl;

	return 0;
}
