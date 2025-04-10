/* ====================================================================
 *
 * Copyright (c) 2025-2027 The SHBluetooth Project.  All rights reserved.
 *
 * ====================================================================
 * This product includes Bluetooth Secure Communication System,
 * Authored and developed by David Chin (Contact: luckqinwei@hotmail.com) */

#ifndef THREAD_SAFE_SOCKET_H
#define THREAD_SAFE_SOCKET_H

#include "base.h" 

class ThreadSafeSocket;
using NewConnectionCallback = std::function<void(ThreadSafeSocket* safesocket,int socket)>;
class BT_EXPORT ThreadSafeSocket
{
public:
	virtual int sendData(const char* data, int length) = 0;
	virtual int socket() = 0;
	virtual ~ThreadSafeSocket() {}	
};
#endif // THREAD_SAFE_SOCKET_H

