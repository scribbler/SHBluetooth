/* ====================================================================
 *
 * Copyright (c) 2025-2027 The SHBluetooth Project.  All rights reserved.
 *
 * ====================================================================
 * This product includes Bluetooth Secure Communication System,
 * Authored and developed by David Chin (Contact: luckqinwei@hotmail.com) */

#pragma once

#include "sh_bluetooth.h"
#include <iostream>
#include <thread>
#include <mutex>
#include "ThreadSafeSocket.h"

class ThreadSafeClientSocketImpl : public ThreadSafeSocket
{
public:
    ThreadSafeClientSocketImpl();
    ~ThreadSafeClientSocketImpl() override;

    bool initialize();

    bool connectToServer(const char* ipAddress, int port);
    bool isConnected();

    void startReceiving(const ReceiveCallback& callback, void* handler);
    void stopReceiving();

    bool isBlocking() const;
public:
    // ThreadSafeSocket
    int sendData(const char* data, int length) override;
    int socket() override;
private:
    void receiveLoop(const ReceiveCallback& callback, void* handler);

private:
    SOCKET sock;
    CRITICAL_SECTION cs;
    std::thread receiveThread;
    std::atomic<bool> running;
    std::mutex mtx;
    int blocking;
};

