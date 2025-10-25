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
    SOCKET socket() override;
private:
    ThreadSafeServerSocketImpl* pImpl; 
};

#ifdef __cplusplus
extern "C"
{
#endif
	
    BT_EXPORT ThreadSafeServerSocket* CreateThreadSafeServerSocketInstance();
	BT_EXPORT void DeleteThreadSafeServerSocketInstance(ThreadSafeServerSocket* instance);
    BT_EXPORT bool InstanceInitialize(ThreadSafeServerSocket* instance,int port);
    BT_EXPORT void InstanceStartAccepting(ThreadSafeServerSocket* instance,const NewConnectionCallback& newconnection, const ReceiveCallback& callback, void* handler);
    BT_EXPORT void InstanceStopAccepting(ThreadSafeServerSocket* instance);
    BT_EXPORT void InstancePauseAccepting(ThreadSafeServerSocket* instance,bool paused);
    BT_EXPORT bool InstanceIsConnected(ThreadSafeServerSocket* instance);
    BT_EXPORT int InstanceSendData(ThreadSafeServerSocket* instance, const char* data, int length);
    BT_EXPORT SOCKET InstanceSocket(ThreadSafeServerSocket* instance);

#if defined(__cplusplus)
}  // extern C
#endif  
