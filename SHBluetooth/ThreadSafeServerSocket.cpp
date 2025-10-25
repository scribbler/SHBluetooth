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

SOCKET ThreadSafeServerSocket::socket()
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

// C interface for creating and managing ThreadSafeServerSocket instances
ThreadSafeServerSocket* CreateThreadSafeServerSocketInstance() {
	return new ThreadSafeServerSocket();
}

void DeleteThreadSafeServerSocketInstance(ThreadSafeServerSocket* instance) {
	if (instance)
		delete instance;
	instance = nullptr;
}
bool InstanceInitialize(ThreadSafeServerSocket* instance, int port)
{
	if (!instance)
		return false;
	return instance->initialize(port);
}
void InstanceStartAccepting(ThreadSafeServerSocket* instance, const NewConnectionCallback& newconnection, const ReceiveCallback& callback, void* handler)
{
	if (!instance)
		return;
	instance->startAccepting(newconnection, callback, handler);
}
void InstanceStopAccepting(ThreadSafeServerSocket* instance)
{
	if (!instance)
		return;
	instance->stopAccepting();
}
void InstancePauseAccepting(ThreadSafeServerSocket* instance, bool paused)
{
	if (!instance)
		return;
	instance->pauseAccepting(paused);
}
bool InstanceIsConnected(ThreadSafeServerSocket* instance)
{
	if(!instance)
		return false;
	return instance->isConnected();
}
int InstanceSendData(ThreadSafeServerSocket* instance, const char* data, int length)
{
	if (!instance || !data || length <= 0)
		return -1; // Invalid parameters
	return instance->sendData(data, length);
}
SOCKET InstanceSocket(ThreadSafeServerSocket* instance)
{
	if (!instance)
		return INVALID_SOCKET; // Invalid instance
	return instance->socket();
}
