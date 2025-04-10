/* ====================================================================
 *
 * Copyright (c) 2025-2027 The SHBluetooth Project.  All rights reserved.
 *
 * ====================================================================
 * This product includes Bluetooth Secure Communication System,
 * Authored and developed by David Chin (Contact: luckqinwei@hotmail.com) */

#include "pch.h"
#include "include/ThreadSafeServerSocket.h"
#include "ThreadSafeServerSocketImpl.h"

ThreadSafeServerSocket::ThreadSafeServerSocket() : pImpl(new ThreadSafeServerSocketImpl()) {}

ThreadSafeServerSocket::~ThreadSafeServerSocket() {
    delete pImpl;
}

bool ThreadSafeServerSocket::initialize(int port) {
    return pImpl->initialize(port);
}

void ThreadSafeServerSocket::startAccepting(const NewConnectionCallback& newconnection, const ReceiveCallback& callback, void* handler) {
    pImpl->startAccepting(newconnection, callback, handler);
}

void ThreadSafeServerSocket::stopAccepting() {
    pImpl->stopAccepting();
}
int ThreadSafeServerSocket::sendData(const char* data, int length)
{
    return pImpl->sendData(data, length);
}

int ThreadSafeServerSocket::socket()
{
    return pImpl->socket();
}
void ThreadSafeServerSocket::pauseAccepting(bool paused)
{
    pImpl->pauseAccepting(paused);
}
bool ThreadSafeServerSocket::isConnected()
{
    return pImpl->isConnected();
}

