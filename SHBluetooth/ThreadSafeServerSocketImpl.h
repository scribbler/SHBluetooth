/* ====================================================================
 *
 * Copyright (c) 2025-2027 The SHBluetooth Project.  All rights reserved.
 *
 * ====================================================================
 * This product includes Bluetooth Secure Communication System,
 * Authored and developed by David Chin (Contact: luckqinwei@hotmail.com) */


#pragma once
#include "sh_bluetooth.h"
#include <thread>
#include <vector>
#include <mutex>
#include "ThreadSafeSocket.h"
class ThreadSafeServerSocketImpl : public ThreadSafeSocket
{
public:
    ThreadSafeServerSocketImpl();
    ~ThreadSafeServerSocketImpl() override;

    bool initialize(int port);
    void startAccepting(const NewConnectionCallback& newconnection, const ReceiveCallback& callback, void* handler);
    void stopAccepting();
    void pauseAccepting(bool paused);
    bool isBlocking() const;
    bool isConnected();
public:
    int sendData(const char* data, int length) override;
    int socket() override;
private:
    void acceptLoop(const NewConnectionCallback& newconnection, const ReceiveCallback& callback, void* handler);
    void clientHandler(SOCKET clientSocket, const ReceiveCallback& callback, void* handler);
    

    SOCKET sock;
    CRITICAL_SECTION cs;
    //bool running;
    std::thread acceptThread;
    std::vector<std::thread> clientThreads;
    std::vector<SOCKET> clientSockets; 
    std::mutex mtx;
    std::atomic<bool> running;
    std::condition_variable cv;
    int blocking;
    std::atomic<bool> accepting_paused;
};

