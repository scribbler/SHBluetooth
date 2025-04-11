/* ====================================================================
 *
 * Copyright (c) 2025-2027 The SHBluetooth Project.  All rights reserved.
 *
 * ====================================================================
 * This product includes Bluetooth Secure Communication System,
 * Authored and developed by David Chin (Contact: luckqinwei@hotmail.com) */

#include "pch.h"
#include "ThreadSafeClientSocketImpl.h"
#include "bt_common.h"

const GUID SPP_UUID = { 0x00006666, 0x0000, 0x1000, { 0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFB } };
ThreadSafeClientSocketImpl::ThreadSafeClientSocketImpl() : sock(INVALID_SOCKET) {
    InitializeCriticalSection(&cs);
    running.store(false);
    blocking = 1;
    bt::sh_initsocket();
}

ThreadSafeClientSocketImpl::~ThreadSafeClientSocketImpl() {
    stopReceiving();
    bt::sh_closesocket(sock);
    bt::sh_cleansocket();
    DeleteCriticalSection(&cs);
}

bool ThreadSafeClientSocketImpl::initialize() {

    stopReceiving();
    sock = bt::sh_createsocket();
    if (sock!= INVALID_SOCKET)
    {
        ULONG recvBufSize = BUFFER_SIZE;  
        if (bt::sh_setsockopt(sock, SOL_SOCKET, SO_RCVBUF, recvBufSize) == SOCKET_ERROR )
        {
            std::cerr << "Set socket revbuf failed: " << WSAGetLastError() << std::endl;
        }

        if (bt::sh_setblocking(sock, blocking) != NO_ERROR) {
            std::cerr << "Failed to set socket to non-blocking mode: " << WSAGetLastError() << std::endl;
            bt::sh_closesocket(sock);
            sock = INVALID_SOCKET;
        }
    }

    return sock != INVALID_SOCKET;
}

bool ThreadSafeClientSocketImpl::connectToServer(const char* addrstr, int port) {

    if (bt::sh_connect_with_serverclassid(sock, addrstr, port, SPP_UUID) == SOCKET_ERROR)
    {
		std::cerr << "connect error: " << WSAGetLastError() << std::endl;
		stopReceiving();
        bt::sh_closesocket(sock);
        sock = INVALID_SOCKET;
		return false;
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

	if (!bt::sh_is_socket_connected(sock))
	{
		std::cerr << "Connect socket is disconnected." << std::endl;
        stopReceiving();
        bt::sh_closesocket(sock);
        sock = INVALID_SOCKET;
		return false;
	}
    return true;
}

int ThreadSafeClientSocketImpl::sendData(const char* data, int length) {
    
    std::lock_guard<std::mutex> lock(mtx);

    char* buffer = (char*)malloc(length + sizeof(DATAEND));
    if (!buffer)
    {
        return -1;
    }
    
    memcpy(buffer, data, length);
    memcpy(buffer + length, DATAEND, sizeof(DATAEND));
    int result = bt::sh_send(sock, buffer, (length + sizeof(DATAEND)) );
    if (result == SOCKET_ERROR) {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
    }
    free(buffer);

    return result;
}
SOCKET ThreadSafeClientSocketImpl::socket()
{
    return (int)sock;
}
void ThreadSafeClientSocketImpl::startReceiving(const ReceiveCallback& callback, void* handler) {
    if (running.load()) {
        return;
    }
    running.store(true);
    receiveThread = std::thread(&ThreadSafeClientSocketImpl::receiveLoop, this, callback, handler);
}
void ThreadSafeClientSocketImpl::stopReceiving() {
    if (!running.load()) {
        return;
    }
    running.store(false);
    
    shutdown(sock, SD_BOTH);
    closesocket(sock);
    
    if (receiveThread.joinable()) {
        receiveThread.join();
    }
}

void ThreadSafeClientSocketImpl::receiveLoop(const ReceiveCallback& callback, void* handler) {
    char buffer[4096];
    int buffer_len = sizeof(buffer) - 1;
    std::vector<char> complete_data;
    while (running.load()) {
        
        EnterCriticalSection(&cs);
        int bytesReceived = bt::sh_recv(sock, buffer, buffer_len);
        LeaveCriticalSection(&cs);
        
        if (bytesReceived > 0) {
            complete_data.insert(complete_data.end(), buffer, buffer + bytesReceived);

            // 处理完整的消息
            size_t pos;
            while ((pos = findMessageEnd(complete_data)) != std::string::npos) {
                // 提取一条完整的消息
                std::vector<char> message(complete_data.begin(), complete_data.begin() + pos);
                callback(handler, message.data(), static_cast<int>(message.size()));

                // 移除已处理的消息
                complete_data.erase(complete_data.begin(), complete_data.begin() + pos + sizeof(DATAEND));
            }
        }
        else if (bytesReceived == 0) {

            if (WSAGetLastError() == WSAEWOULDBLOCK)
            {
                // 非阻塞模式下没有数据可读，稍作休眠以避免忙等
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            else
            {
                complete_data.clear();
                std::cout << "Connection closed by server." << std::endl;
                callback(handler, nullptr, 0);
                break;
            }
        }
        else {
            complete_data.clear();
            std::cerr << "Client Receive failed." << std::endl;
            callback(handler, nullptr, -1);
            break;
        }
    }
}

/// <summary>
/// 获得是否阻塞模式
/// </summary>
/// <returns>true阻塞，false非阻塞</returns>
bool ThreadSafeClientSocketImpl::isBlocking() const {
    return blocking != 0;
}

bool ThreadSafeClientSocketImpl::isConnected()
{
    if (sock != INVALID_SOCKET)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (!bt::sh_is_socket_connected(sock))
        {
            goto end;
        }
        return true;
    }

end:
    bt::sh_closesocket(sock);
    
    return false;
}
