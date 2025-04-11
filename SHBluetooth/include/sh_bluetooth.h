/* ====================================================================
 *
 * Copyright (c) 2025-2027 The SHBluetooth Project.  All rights reserved.
 *
 * ====================================================================
 * This product includes Bluetooth Secure Communication System,
 * Authored and developed by David Chin (Contact: luckqinwei@hotmail.com) */

#pragma once

#ifndef SH_BLUETOOTH_H_
#define SH_BLUETOOTH_H_

#include "base.h"

#include <string>
#include <vector>

namespace bt
{
#ifdef __cplusplus
	extern "C"
	{
#endif
		struct ServerRecord
		{
			std::string host;
			std::string name;
			std::string description;
			int port;
			std::string protocol;
			char* rawrecord;
			int rawrecord_len;
		};
		BT_EXPORT int sh_initsocket();
		BT_EXPORT int sh_cleansocket();
		BT_EXPORT SOCKET sh_createsocket();
		BT_EXPORT int sh_closesocket(SOCKET sockfd);
		BT_EXPORT int sh_bind(SOCKET sockfd);
		BT_EXPORT int sh_bind_with_port(SOCKET sockfd, int port);
		BT_EXPORT int sh_listen(SOCKET sockfd);
		BT_EXPORT SOCKET sh_accept(SOCKET sockfd);
		BT_EXPORT int sh_connect(SOCKET sockfd,const char* addrstr, int port);
		BT_EXPORT int sh_connect_with_serverclassid(SOCKET sockfd, const char* addrstr, int port, GUID guid);
		BT_EXPORT int sh_send(SOCKET sockfd, const char* data, const int data_len);
		BT_EXPORT int sh_recv(SOCKET sockfd, char* buffer, int buffer_len);
		/// <summary>
		/// sh_getsockname
		/// 返回 socket 绑定的本地地址信息（即本机的地址和端口号）
		/// </summary>
		/// <param name="sockfd">要操作的套接字描述符</param>
		/// <param name="sa_out">指向 sockaddr 结构的指针，用于存放返回的地址信息</param>
		/// <returns>
		/// 成功时：
		/// 调用成功后，函数返回 0，同时参数 name 中会填充对应的地址信息
		/// 失败时：
		/// 如果调用失败，函数返回 - 1（在 Windows 平台下通常返回 SOCKET_ERROR），
		/// 此时可以通过 errno（在 POSIX 系统中）或 WSAGetLastError()（在 Windows 系统中）获取具体错误码，以确定失败原因。
		/// </returns>
		BT_EXPORT int sh_getsockname(SOCKET sockfd, SOCKADDR_BTH* sa_out);

		/// <summary>
		/// sh_getpeername
		/// 返回与该 socket 建立连接的远端地址信息（即对方的地址和端口号）
		/// </summary>
		/// <param name="sockfd">要操作的套接字描述符</param>
		/// <param name="sa_out">指向 sockaddr 结构的指针，用于存放返回的地址信息</param>
		/// <returns>
		/// 成功时：
		/// 调用成功后，函数返回 0，同时参数 name 中会填充对应的地址信息
		/// 失败时：
		/// 如果调用失败，函数返回 - 1（在 Windows 平台下通常返回 SOCKET_ERROR），
		/// 此时可以通过 errno（在 POSIX 系统中）或 WSAGetLastError()（在 Windows 系统中）获取具体错误码，以确定失败原因。
		/// </returns>
		BT_EXPORT int sh_getpeername(SOCKET sockfd, SOCKADDR_BTH* sa_out);

		BT_EXPORT int sh_dup(SOCKET sockfd);
		BT_EXPORT int sh_discover_devices(std::vector<BLUETOOTH_DEVICE_INFO>* devices);
		BT_EXPORT int sh_scan_devices(ExecuteFunction function);
		BT_EXPORT int sh_list_local(std::vector<BLUETOOTH_RADIO_INFO>* radios);
		BT_EXPORT int sh_lookup_name(SOCKADDR_BTH sa, BLUETOOTH_DEVICE_INFO* device_info);
		BT_EXPORT HANDLE sh_get_lookup_radio_hanlde();
		BT_EXPORT int sh_find_service(const char* addrstr, const char* uuidstr, struct ServerRecord* record);
		BT_EXPORT int sh_set_service(SOCKET sockfd, const char* service_name, const char* service_desc, const char* service_class_id_str, int advertise);
		BT_EXPORT int sh_set_service_raw(BYTE* sdpRecord, int sdpRecord_len, int advertise);
		BT_EXPORT int sh_setblocking(SOCKET sockfd, int block);
		BT_EXPORT int sh_settimeout(SOCKET sockfd, double secondTimeout);
		BT_EXPORT int sh_gettimeout(SOCKET sockfd);
		BT_EXPORT int sh_setsockopt(SOCKET sockfd, int level, ULONG optname, ULONG flag);
		BT_EXPORT int sh_getsockopt(SOCKET sockfd, int level, ULONG optname);

		BT_EXPORT HANDLE sh_register_auth(const BLUETOOTH_DEVICE_INFO& deviceInfo);
		BT_EXPORT int sh_unregister_auth(HANDLE authHandle);
		BT_EXPORT int sh_auth_device(const BLUETOOTH_DEVICE_INFO& deviceInfo);
		BT_EXPORT int sh_pair_device_with_addr(std::string addr, PWSTR passkey, HANDLE hRadioHandle);
		BT_EXPORT int sh_pair_device(PBLUETOOTH_DEVICE_INFO, PWSTR passkey, HANDLE hRadioHandle);
		BT_EXPORT int sh_last_error();

		BT_EXPORT bool sh_is_socket_connected(SOCKET sockfd);

#ifdef __cplusplus
	}
#endif
}

#endif // SHBluetooth