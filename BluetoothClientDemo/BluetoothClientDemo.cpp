// BluetoothClientDemo.cpp: 目标的源文件。
//

#include "BluetoothClientDemo.h"
#include <ThreadSafeClientSocket.h>
#include <sh_bluetooth.h>
#include <thread>
#include <atomic>
#include <iostream>

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
    std::cout << "BTClient begin!\n";
    ThreadSafeClientSocket clientSocket;

    if (!clientSocket.initialize()) {
        std::cerr << "Failed to initialize client socket" << std::endl;
        return -1;
    }

    //const char* serverIp = "18:87:40:1A:38:A8";/red
    //const char* serverIp = "5b:74:76:ae:64:00"; //y
    //const char* serverIp = "f0:d4:15:a4:36:65";//q
    //const char* serverIp = "3c:55:76:2a:e0:56";
    const char* serverIp = "4C:5F:70:22:DD:27";
    //const char* serverIp = "00:a7:47:23:14:65";
    int port = -1;
    if (!clientSocket.connectToServer(serverIp, port)) {
        std::cerr << "Failed to connect to server at " << serverIp << ":" << port << std::endl;
        return -1;
    }

    clientSocket.startReceiving(onReceive, nullptr);

    std::cout << "Client is connected to server at " << serverIp << ":" << port << std::endl;
    char buffer[128] = "Hello, server!";


    // 启动消息循环线程
    std::thread loopThread(messageLoop);
    // 添加调试信息
    std::cout << "Sleeping for 2 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Woke up from sleep." << std::endl;
    if (clientSocket.sendData(buffer, strlen(buffer)))
    {
        std::cout << "Client Send Data to server: " << buffer << std::endl;
    }
    std::cout << "Press Enter to stop the client..." << std::endl;
    std::cin.get();

    // 停止消息循环
    running.store(false);
    //loopThread.join();

    clientSocket.stopReceiving();
    std::cout << "Client stopped." << std::endl;

    return 0;
}
