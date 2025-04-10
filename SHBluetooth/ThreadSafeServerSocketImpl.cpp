/* ====================================================================
 *
 * Copyright (c) 2025-2027 The SHBluetooth Project.  All rights reserved.
 *
 * ====================================================================
 * This product includes Bluetooth Secure Communication System,
 * Authored and developed by David Chin (Contact: luckqinwei@hotmail.com) */

#include "pch.h"
#include "ThreadSafeServerSocketImpl.h"
#include "bt_common.h"

const char* SPP_UUID_STR = "00006666-0000-1000-8000-00805F9B34FB";
ThreadSafeServerSocketImpl::ThreadSafeServerSocketImpl() : sock(INVALID_SOCKET) {
    InitializeCriticalSection(&cs);
    running.store(false);
	blocking = 1;
    accepting_paused.store(false);
}

ThreadSafeServerSocketImpl::~ThreadSafeServerSocketImpl() {
    stopAccepting();
    bt::sh_closesocket(sock);
    bt::sh_cleansocket();
    DeleteCriticalSection(&cs);
}

bool ThreadSafeServerSocketImpl::initialize(int port) {
    if (bt::sh_initsocket() == NO_ERROR) {
        sock = bt::sh_createsocket();
        if (sock != INVALID_SOCKET) {
            if (bt::sh_bind_with_port(sock, port) == SOCKET_ERROR) {
                bt::sh_closesocket(sock);
                sock = INVALID_SOCKET;
            }
            else
            {
                ULONG recvBufSize = BUFFER_SIZE;
                if (bt::sh_setsockopt(sock, SOL_SOCKET, SO_RCVBUF, recvBufSize) == SOCKET_ERROR)
                {
                    std::cerr << "Set socket revbuf failed: " << WSAGetLastError() << std::endl;
                }
				bt::sh_set_service(sock, "SHBluetooth server", "", SPP_UUID_STR, 1);

                // 设置套接字为非阻塞模式
                // 1 表示非阻塞模式
                if (bt::sh_setblocking(sock, blocking) != NO_ERROR) {
                    std::cerr << "Failed to set socket to non-blocking mode: " << WSAGetLastError() << std::endl;
                    bt::sh_closesocket(sock);
                    sock = INVALID_SOCKET;
                }
            }
        }
    }
    return sock != INVALID_SOCKET;
}

void ThreadSafeServerSocketImpl::startAccepting(const NewConnectionCallback& newconnection, const ReceiveCallback& callback, void* handler) {
    if (running.load()) {
        return;
    }
    running.store(true);
    acceptThread = std::thread(&ThreadSafeServerSocketImpl::acceptLoop, this, newconnection,callback, handler);
}

void ThreadSafeServerSocketImpl::stopAccepting() {
    if (!running.load()) {
        return;
    }
    running.store(false);

    shutdown(sock, SD_BOTH);  
    bt::sh_closesocket(sock);

    if (acceptThread.joinable()) {
    
        acceptThread.join();
    }

    // 清理客户端套接字和线程
    {
        std::lock_guard<std::mutex> lock(mtx);
        for (SOCKET cs : clientSockets)
        {
            shutdown(cs, SD_BOTH);
            bt::sh_closesocket(cs);
        }
    }

    for (auto& t : clientThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
}
void ThreadSafeServerSocketImpl::pauseAccepting(bool paused)
{
    accepting_paused.store(paused);
}
int ThreadSafeServerSocketImpl::sendData(const char* data, int length)
{
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& clientSocket : clientSockets) {
        char* buffer = (char*)malloc(length + sizeof(DATAEND));
        if (!buffer)
        {
            return -1;
        }

        memcpy(buffer, data, length);
        memcpy(buffer + length, DATAEND, sizeof(DATAEND));
        int result = bt::sh_send(clientSocket, buffer, (length + sizeof(DATAEND)));
        if (result == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        }
        free(buffer);
    }
    return length;

}
int ThreadSafeServerSocketImpl::socket()
{
    return sock;
}

void ThreadSafeServerSocketImpl::acceptLoop(const NewConnectionCallback& newconnection, const ReceiveCallback& callback, void* handler) {
    if (bt::sh_listen(sock) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        return;
    }
    
    while (running.load()) {
        // 检查现有的客户端套接字是否有效
        {
            std::lock_guard<std::mutex> lock(mtx);
            for (auto it = clientSockets.begin(); it != clientSockets.end();)
            {
                SOCKET clientSocket = *it;
                if ( !bt::sh_is_socket_connected(clientSocket))
                {
                    // 套接字无效，移除并关闭
                    std::cerr << "Removing invalid client socket." << std::endl;
                    shutdown(clientSocket, SD_BOTH);
                    bt::sh_closesocket(clientSocket);
                    it = clientSockets.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }
       
        if (accepting_paused.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            continue;
        }

        // 接受新的客户端连接
        SOCKET clientSocket = bt::sh_accept(sock);
        if (clientSocket == INVALID_SOCKET) {
            int error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK || error == WSAEINVAL || error == WSAENOTSOCK || error == WSANOTINITIALISED) {
                // 没有新的连接，稍作休眠以避免忙等
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                continue;
            } else if (!running.load()) {
                std::cout << "Accept interrupted by shutdown." << std::endl;
                break;
            } else {
                std::cerr << "Accept failed: " << error << std::endl;
                continue;
            }
        }
        
        {
            std::lock_guard<std::mutex> lock(mtx);
			if (!clientSockets.empty()) 
            {
				// 如果已有客户端连接，拒绝新连接
				std::cerr << "A client is already connected. Rejecting new connection." << std::endl;
				shutdown(clientSocket, SD_BOTH);
				bt::sh_closesocket(clientSocket);
				continue;
			}
            clientSockets.push_back(clientSocket);
            newconnection(this, clientSocket);
        }
        clientThreads.emplace_back(&ThreadSafeServerSocketImpl::clientHandler, this, clientSocket, callback, handler);
    }
}

void ThreadSafeServerSocketImpl::clientHandler(SOCKET clientSocket, const ReceiveCallback& callback, void* handler) {
    char buffer[4096] = {0};
    int buffer_len = sizeof(buffer) - 1;
    std::vector<char> complete_data;
    while (running.load()) {
        EnterCriticalSection(&cs);
        int bytesReceived = bt::sh_recv(clientSocket, buffer, buffer_len);
        LeaveCriticalSection(&cs);
        
        if (bytesReceived > 0) {
            std::lock_guard<std::mutex> lock(mtx);
            complete_data.insert(complete_data.end(), buffer, buffer + bytesReceived);
            
			// 处理完整的消息
            size_t pos;
            while ((pos = findMessageEnd(complete_data)) != std::string::npos) {
                // 提取一条完整的消息
                std::vector<char> message(complete_data.begin(), complete_data.begin() + pos);
                callback(handler, message.data(), message.size());

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
                std::cout << "Connection closed by client." << std::endl;
                callback(handler, nullptr, 0);
                break;
            }
        }
        else {
            complete_data.clear();
            std::cerr << "Server Receive failed." << std::endl;
            callback(handler, nullptr, -1);
            break;
        }
    }

    bt::sh_closesocket(clientSocket);
    
    {
        std::lock_guard<std::mutex> lock(mtx);
        clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), clientSocket), clientSockets.end());
    }
}

/// <summary>
/// 获得是否阻塞模式
/// </summary>
/// <returns>true阻塞，false非阻塞</returns>
bool ThreadSafeServerSocketImpl::isBlocking() const {
    return blocking != 0;
}

bool ThreadSafeServerSocketImpl::isConnected()
{
    if (sock == INVALID_SOCKET)
    {
        return false;
    }

    std::lock_guard<std::mutex> lock(mtx);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    bool result = false;
    for (auto it = clientSockets.begin(); it != clientSockets.end();)
    {
        SOCKET clientSocket = *it;
        if (!bt::sh_is_socket_connected(clientSocket))
        {
            shutdown(clientSocket, SD_BOTH);
            bt::sh_closesocket(clientSocket);
            it = clientSockets.erase(it);
            result = false;
        }
        else
        {
            ++it;
            result = true;
        }
    }

    return result;
}
