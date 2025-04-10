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

class ThreadSafeServerSocketImpl; 
class BT_EXPORT ThreadSafeServerSocket : public ThreadSafeSocket
{
public:
    ThreadSafeServerSocket();
    ~ThreadSafeServerSocket()override;

    bool initialize(int port);
    void startAccepting(const NewConnectionCallback& newconnection,const ReceiveCallback& callback, void* handler);
    void stopAccepting();
    void pauseAccepting(bool paused);
    bool isConnected();
public:
    // ThreadSafeSocket
    int sendData(const char* data, int length) override;
    int socket() override;
private:
    ThreadSafeServerSocketImpl* pImpl; 
};

