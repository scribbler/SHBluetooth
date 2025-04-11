/* ====================================================================
 *
 * Copyright (c) 2025-2027 The SHBluetooth Project.  All rights reserved.
 *
 * ====================================================================
 * This product includes Bluetooth Secure Communication System,
 * Authored and developed by David Chin (Contact: luckqinwei@hotmail.com) */

#include "pch.h"
#include "include/ThreadSafeClientSocket.h"
#include "ThreadSafeClientSocketImpl.h"

ThreadSafeClientSocket::ThreadSafeClientSocket() : pImpl(new ThreadSafeClientSocketImpl()) {}
ThreadSafeClientSocket::~ThreadSafeClientSocket() {
    delete pImpl;
}

bool ThreadSafeClientSocket::initialize() {

    return pImpl->initialize();
}

bool ThreadSafeClientSocket::connectToServer(const char* addrstr, int port) {

    return pImpl->connectToServer(addrstr,port);
}

int ThreadSafeClientSocket::sendData(const char* data, int length) {

    return pImpl->sendData(data, length);
}
SOCKET ThreadSafeClientSocket::socket()
{
    return pImpl->socket();
}
void ThreadSafeClientSocket::startReceiving(const ReceiveCallback& callback, void* handler)
{
	pImpl->startReceiving(callback, handler);
}
void ThreadSafeClientSocket::stopReceiving()
{
	pImpl->stopReceiving();
}
bool ThreadSafeClientSocket::isConnected()
{
    return pImpl->isConnected();
}
