/* ====================================================================
 *
 * Copyright (c) 2025-2027 The SHBluetooth Project.  All rights reserved.
 *
 * ====================================================================
 * This product includes Bluetooth Secure Communication System,
 * Authored and developed by David Chin (Contact: luckqinwei@hotmail.com) */

#pragma once

#include "base.h"
#include "ThreadSafeSocket.h"

class ThreadSafeClientSocketImpl;
class BT_EXPORT ThreadSafeClientSocket : public ThreadSafeSocket
{
public:
    ThreadSafeClientSocket();
    ~ThreadSafeClientSocket() override;

    bool initialize();

    bool connectToServer(const char* ipAddress, int port);
    bool isConnected();
    void startReceiving(const ReceiveCallback& callback,void* handler);
    void stopReceiving();
public: 
    // ThreadSafeSocket
    int sendData(const char* data, int length) override;
    int socket() override;
private:
    ThreadSafeClientSocketImpl* pImpl;

};

