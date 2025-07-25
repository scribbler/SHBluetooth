/* ====================================================================
 *
 * Copyright (c) 2025-2027 The SHBluetooth Project.  All rights reserved.
 *
 * ====================================================================
 * This product includes Bluetooth Secure Communication System,
 * Authored and developed by David Chin (Contact: luckqinwei@hotmail.com) */

#include "pch.h"
#include "include/sh_bluetooth.h"
#include "bt_common.h"

namespace bt {
    #define _CHECK_OR_RAISE_WSA(cond) \
        if( !(cond) ) {  return -1; }

    BOOL CALLBACK auth_callback_ex(LPVOID pvParam, PBLUETOOTH_AUTHENTICATION_CALLBACK_PARAMS pAuthCallbackParams)
    {
        BLUETOOTH_AUTHENTICATION_METHOD authMethod = pAuthCallbackParams->authenticationMethod;
        BLUETOOTH_AUTHENTICATE_RESPONSE response;
        response.bthAddressRemote = pAuthCallbackParams->deviceInfo.Address;
        response.authMethod = pAuthCallbackParams->authenticationMethod; // == BLUETOOTH_AUTHENTICATION_METHOD_LEGACY
        response.negativeResponse = false;

        //UCHAR pin[] = "6666";
        //copy(pin, pin + sizeof(pin), response.pinInfo.pin);
        //response.pinInfo.pinLength = sizeof(pin) - 1;

        HRESULT err = BluetoothSendAuthenticationResponseEx(NULL, &response);
        if (err != ERROR_SUCCESS)
        {
            cerr << "Authentication send error = " << err << endl;
            return false;
        }
        return true;
    }

	int sh_initsocket()
	{
        // 初始化 Winsock
        WSADATA wsaData;
        if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed." << std::endl;
            return -1;
        }

        return 0;
	}
    int sh_cleansocket()
    {
        ::WSACleanup();
        return 0;
    }
    SOCKET sh_createsocket()
    {
        int family = AF_BTH;
        int type = SOCK_STREAM;
        int proto = BTHPROTO_RFCOMM;
        SOCKET sockfd = -1;

        // 创建 RFCOMM 套接字
        sockfd = ::socket(AF_BTH, type, proto);
        if (sockfd == INVALID_SOCKET) {
            std::cerr << "Socket creation failed." << std::endl;
            return -1;
        }

        return sockfd;
    }

    int sh_closesocket(SOCKET sockfd)
    {
        CLOSE_SOCKET(sockfd);
        return 0;
    }
    
    int sh_bind(SOCKET sockfd)
    {    
        return sh_bind_with_port(sockfd,BT_PORT_ANY);
    }
    int sh_bind_with_port(SOCKET sockfd, int port)
    {
        SOCKADDR_BTH sa = { 0 };
        int sa_len = sizeof(sa);
        sa.addressFamily = AF_BTH;
        sa.port = port<0? BT_PORT_ANY:port;
        sa.btAddr = 0;

        if (::bind(sockfd, (LPSOCKADDR)&sa, sa_len) == SOCKET_ERROR) {
            std::cerr << "Bind failed." << std::endl;
            return 1;
        }
        return 0;
    }
    int sh_listen(SOCKET sockfd)
    {
        int backlog = 10;

        // 开始监听连接
        if (::listen(sockfd, backlog) == SOCKET_ERROR) {
            std::cerr << "Listen failed." << std::endl;
            return 1;
        }
      
        return 0;
    }

    SOCKET sh_accept(SOCKET sockfd)
    {
        SOCKET clientfd = -1;
        SOCKADDR_BTH sa = { 0 };
        int sa_len = sizeof(sa);

        // 接受客户端连接
        clientfd = ::accept(sockfd, (LPSOCKADDR)&sa, &sa_len);
        if (clientfd == INVALID_SOCKET) {
            return -1;
        }

        // 可以打印客户端IP和端口
        std::cout << "Client connected." << BthAddrToString(sa.btAddr)<< sa .port << std::endl;

        return clientfd;
    }

    int sh_connect(SOCKET sockfd, const char* addrstr, int port)
    {
        GUID guid = {0};
        return sh_connect_with_serverclassid(sockfd, addrstr, port, guid);
    }
    int sh_connect_with_serverclassid(SOCKET sockfd, const char* addrstr, int port, GUID guid)
    {
        SOCKADDR_BTH sa = { 0 };
        int sa_len = sizeof(sa);
     
        if (addrstr == nullptr)
        {
            return -1;
        }

        if (SOCKET_ERROR == WSAStringToAddress(ConvertLPSTRToLPWSTR((LPSTR)addrstr), AF_BTH, NULL, (LPSOCKADDR)&sa, &sa_len)) {
            return -1;
        }
        
        sa.addressFamily = AF_BTH;
        sa.port = port<0? BT_PORT_ANY:port;
        sa.serviceClassId = guid;
        
        // 连接到服务端
        if (::connect(sockfd, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR) {
            std::cerr << "Connection failed." << std::endl;
            return -1;
        }
        return 0;
    }

    int sh_send(SOCKET sockfd, const char * data, const int data_len)
    {
		/* 常用的标志包括：

    	 0: 默认标志，没有特殊的选项。
		 MSG_DONTROUTE : 数据包不经过路由，直接发送到本地网络。
		 MSG_DONTWAIT : 非阻塞操作，如果操作会阻塞，则返回错误。
		 MSG_NOSIGNAL : 如果连接已断开，不发送SIGPIPE信号。
         */
        int flags = 0;

        int result = ::send(sockfd, data, (int)data_len, flags);
        if (result == SOCKET_ERROR) {
            return SOCKET_ERROR;
        }
        return result;
    };
 

    int sh_recv(SOCKET sockfd, char* buffer, int buffer_len)
    {
        int bytesRead;
        bytesRead = ::recv(sockfd, buffer, buffer_len, 0);
        if (SOCKET_ERROR == bytesRead) {
            int error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK) {
                // 非阻塞模式下没有数据可读
                return 0;
            }
            else {
                std::cerr << "Received error: " << error << std::endl;
                return SOCKET_ERROR;
            }
        }
        
        return bytesRead;
    }

    int sh_getsockname(SOCKET sockfd, SOCKADDR_BTH* sa_out)
    {   
        if (sa_out == nullptr)
            return -1;

        memset(sa_out, 0, sizeof(SOCKADDR_BTH));
        sa_out->addressFamily = AF_BTH;
        int sa_len = sizeof(SOCKADDR_BTH);
        if (::getsockname(sockfd, (SOCKADDR*)sa_out, &sa_len) == SOCKET_ERROR)
        {
            printf("getsockname() failed: %d\n", WSAGetLastError());
            return -1;
        }
;
        printf("服务端监听的 RFCOMM 通道：%d\n", sa_out->port);
        return 0;
    }

    int sh_getpeername(SOCKET sockfd, SOCKADDR_BTH* sa_out)
    {
        if (sa_out == nullptr)
            return -1;

        memset(sa_out, 0, sizeof(SOCKADDR_BTH));
        sa_out->addressFamily = AF_BTH;
        int sa_len = sizeof(SOCKADDR_BTH);

        if (getpeername(sockfd, (SOCKADDR*)sa_out, &sa_len) == SOCKET_ERROR)
        {
            printf("getsockname() failed: %d\n", WSAGetLastError());
            return -1;
        }
        return 0;
    }

    int sh_dup(SOCKET sockfd)
    {
        int newsockfd = -1;
        DWORD pid;
        WSAPROTOCOL_INFO pi = { 0 };

		// prepare to duplicate
		pid = GetCurrentProcessId();
		if (WSADuplicateSocket(sockfd, pid, &pi) == SOCKET_ERROR)
		{
			printf("WSADuplicateSocket() failed: %d\n", WSAGetLastError());
			return -1;
		}
  
        // duplicate!
		newsockfd = (int)WSASocket(FROM_PROTOCOL_INFO,
			FROM_PROTOCOL_INFO,
			FROM_PROTOCOL_INFO,
			&pi, 0, 0);
        if (newsockfd == INVALID_SOCKET)
        {
            printf("WSASocket() failed: %d\n", WSAGetLastError());
            return -1;
        }

        return newsockfd;
    }

    int sh_discover_devices(std::vector<BLUETOOTH_DEVICE_INFO> * devices)
    {
        if (devices == nullptr)
            return -1;
        BLUETOOTH_DEVICE_INFO device_info;
        BLUETOOTH_DEVICE_SEARCH_PARAMS search_criteria;
        HBLUETOOTH_DEVICE_FIND found_device_handle;
        BOOL next = TRUE;
     
        int duration = 8;
          
        ZeroMemory(&device_info, sizeof(BLUETOOTH_DEVICE_INFO));
        ZeroMemory(&search_criteria, sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS));

        device_info.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
        search_criteria.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
        search_criteria.fReturnAuthenticated = TRUE;
        search_criteria.fReturnRemembered = TRUE;
        search_criteria.fReturnConnected = TRUE;
        search_criteria.fReturnUnknown = TRUE;
        search_criteria.fIssueInquiry = TRUE;
        search_criteria.cTimeoutMultiplier = duration;
        search_criteria.hRadio = NULL;

        //Py_BEGIN_ALLOW_THREADS;
        found_device_handle = BluetoothFindFirstDevice(&search_criteria, &device_info);
        //Py_END_ALLOW_THREADS;
        if (found_device_handle == NULL)
        {
            std::cerr << "No Bluetooth devices found." << GetLastError() << std::endl;
            return -1;
        }

		while (next) {
            devices->push_back(device_info);
			//Py_BEGIN_ALLOW_THREADS;
			next = BluetoothFindNextDevice(found_device_handle, &device_info);
			//Py_END_ALLOW_THREADS;
		}
        BluetoothFindDeviceClose(found_device_handle);

        return 0;
    }
    int sh_scan_devices(ExecuteFunction function)
    {
        if (function == nullptr)
            return -1;
        BLUETOOTH_DEVICE_INFO device_info;
        BLUETOOTH_DEVICE_SEARCH_PARAMS search_criteria;
        HBLUETOOTH_DEVICE_FIND found_device_handle;
        BOOL next = TRUE;

        int duration = 8;

        ZeroMemory(&device_info, sizeof(BLUETOOTH_DEVICE_INFO));
        ZeroMemory(&search_criteria, sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS));

        device_info.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
        search_criteria.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
        search_criteria.fReturnAuthenticated = TRUE;
        search_criteria.fReturnRemembered = TRUE;
        search_criteria.fReturnConnected = TRUE;
        search_criteria.fReturnUnknown = TRUE;
        search_criteria.fIssueInquiry = TRUE;
        search_criteria.cTimeoutMultiplier = duration;
        search_criteria.hRadio = NULL;

        //Py_BEGIN_ALLOW_THREADS;
        found_device_handle = BluetoothFindFirstDevice(&search_criteria, &device_info);
        //Py_END_ALLOW_THREADS;
        if (found_device_handle == NULL)
        {
            std::cerr << "No Bluetooth devices found." << GetLastError() << std::endl;
            return -1;
        }

        while (next) {
            
            if ( function(&device_info) )
            {
                break;
            }
            //Py_BEGIN_ALLOW_THREADS;
            next = BluetoothFindNextDevice(found_device_handle, &device_info);
            //Py_END_ALLOW_THREADS;
        }
        BluetoothFindDeviceClose(found_device_handle);

        return 0;
    }
    int sh_list_local(std::vector<BLUETOOTH_RADIO_INFO>* radios)
    {
        if (radios == nullptr)
            return -1;

        HANDLE radioHanlde = NULL;
        HBLUETOOTH_RADIO_FIND rodioFindHandle = NULL;
        BOOL next = TRUE;
        BLUETOOTH_FIND_RADIO_PARAMS find_radio = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };
     
        //Py_BEGIN_ALLOW_THREADS;
        rodioFindHandle = BluetoothFindFirstRadio(&find_radio, &radioHanlde);
        //Py_END_ALLOW_THREADS;
        if (rodioFindHandle == NULL)
        {
            std::cerr << "No Local Bluetooth found." << GetLastError() << std::endl;
            return -1;
        }

        while (next) {
           
            BLUETOOTH_RADIO_INFO radioInfo = { sizeof(BLUETOOTH_RADIO_INFO), 0, };

            // Then get the radio device info....
            if (BluetoothGetRadioInfo(radioHanlde, &radioInfo) == ERROR_SUCCESS)
            {
                radios->push_back(radioInfo);
            }
			CloseHandle(radioHanlde);
            next = BluetoothFindNextRadio(rodioFindHandle, &radioHanlde);
        }
        BluetoothFindRadioClose(rodioFindHandle);
        return 0;
    }

    int sh_lookup_name(SOCKADDR_BTH sa, BLUETOOTH_DEVICE_INFO* device_info)
    {
        if (device_info == NULL)
        {
            return -1;
        }
        HANDLE radioHanlde = NULL;
        HBLUETOOTH_RADIO_FIND rodioFindHandle = NULL;
        
        BLUETOOTH_FIND_RADIO_PARAMS find_radio = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };
 

        //Py_BEGIN_ALLOW_THREADS;
        rodioFindHandle = BluetoothFindFirstRadio(&find_radio, &radioHanlde);
        //Py_END_ALLOW_THREADS;
        if (rodioFindHandle == NULL)
        {
            std::cerr << "No Local Bluetooth found." << GetLastError() << std::endl;
            return -1;
        }

        memset(device_info, 0, sizeof(BLUETOOTH_DEVICE_INFO));
        device_info->dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
        device_info->Address.ullLong = sa.btAddr;
		if (BluetoothGetDeviceInfo(radioHanlde, device_info) == INVALID_SOCKET)
		{
            BluetoothFindRadioClose(rodioFindHandle);
            return -1;
		}

		BluetoothFindRadioClose(rodioFindHandle);
        return 0;
	}
    HANDLE sh_get_lookup_radio_hanlde()
    {
        HANDLE radioHanlde = NULL;
        HBLUETOOTH_RADIO_FIND rodioFindHandle = NULL;

        BLUETOOTH_FIND_RADIO_PARAMS find_radio = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };


        //Py_BEGIN_ALLOW_THREADS;
        rodioFindHandle = BluetoothFindFirstRadio(&find_radio, &radioHanlde);
        //Py_END_ALLOW_THREADS;
        if (rodioFindHandle == NULL)
        {
            std::cerr << "No Local Bluetooth found." << GetLastError() << std::endl;
            return NULL;
        }

        BluetoothFindRadioClose(rodioFindHandle);
        return radioHanlde;
    }
    // ======================= SDP FUNCTIONS ======================
    int sh_find_service(const char* addrstr, const char* uuidstr, struct ServerRecord *record)
    {
        assert(record != NULL);
        // inquiry data structure
        DWORD qs_len = sizeof(WSAQUERYSET);
        WSAQUERYSET* qs = (WSAQUERYSET*)malloc(qs_len);
        if (qs == NULL)
        {
            printf("malloc() failed: %d\n", WSAGetLastError());
			return -1;
        }
        DWORD flags = LUP_FLUSHCACHE | LUP_RETURN_ALL;
        flags = LUP_CONTAINERS | LUP_RETURN_NAME | LUP_RETURN_ADDR;
        HANDLE hLookup;
        int done = 0;
        int status = 0;
        GUID uuid = { 0 };
        char localAddressBuf[20] = { 0 };

        ZeroMemory(qs, qs_len);
        qs->dwSize = sizeof(WSAQUERYSET);
        qs->dwNameSpace = NS_BTH;
        qs->dwNumberOfCsAddrs = 0;
       
        if (addrstr)
        {
            if (0 == strcmp(addrstr, "localhost"))
            {
                // bind a temporary socket and get its Bluetooth address
                SOCKADDR_BTH sa = { 0 };
                int sa_len = sizeof(sa);
                SOCKET tmpfd = ::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);

                _CHECK_OR_RAISE_WSA(tmpfd >= 0);

                sa.addressFamily = AF_BTH;
                sa.port = BT_PORT_ANY;
                _CHECK_OR_RAISE_WSA(NO_ERROR == ::bind(tmpfd, (LPSOCKADDR)&sa, sa_len));
                _CHECK_OR_RAISE_WSA(NO_ERROR == ::getsockname(tmpfd, (LPSOCKADDR)&sa, &sa_len));

                ba2str(sa.btAddr, localAddressBuf, _countof(localAddressBuf));
                sh_closesocket(tmpfd);

                flags |= LUP_RES_SERVICE;
            }
            else {
                strcpy_s(localAddressBuf, _countof(localAddressBuf), addrstr);
            }
        }
        //qs->lpszContext = ConvertLPSTRToLPWSTR(localAddressBuf);
        
        if (strlen(uuidstr) != 36 || uuidstr[8] != '-' || uuidstr[13] != '-'
            || uuidstr[18] != '-' || uuidstr[23] != '-') {
            std::cerr << "Invalid UUID!." << std::endl;
            return -1;
        }

        str2uuid(uuidstr, &uuid);
        //qs->lpServiceClassId = &uuid;
        
        //Py_BEGIN_ALLOW_THREADS;
     
        WSAQUERYSET qs1 = { 0 };
        qs1.dwSize = sizeof(WSAQUERYSET);
        qs1.dwNameSpace = NS_BTH;
        qs1.dwNumberOfCsAddrs = 0;
        status = WSALookupServiceBegin(qs, flags, &hLookup);
        //Py_END_ALLOW_THREADS;
        if (SOCKET_ERROR == status) {
            int err_code = WSAGetLastError();
            switch (err_code) {
            case WSASERVICE_NOT_FOUND:
                std::cerr << "Service not found." << std::endl;
                break;
            case WSAEFAULT:
                std::cerr << "Buffer too small." << std::endl;
                break;
            case WSAEINVAL:
                std::cerr << "Invalid parameter." << std::endl;
                break;
            default:
                std::cerr << "WSALookupServiceBegin failed with error: " << err_code << std::endl;
                break;
            }
            if (WSASERVICE_NOT_FOUND == err_code) {
                // this device does not advertise any services.  return an
                // empty list
                free(qs);
                return 0;
            }
            else {
                // unexpected error.  raise an exception
                std::cerr << "unexpected error.  raise an exception!" << std::endl;
                free(qs);
                return 0;
            }
        }

        // iterate through the inquiry results
        while (!done) {
            //Py_BEGIN_ALLOW_THREADS;
            char buffer[1024] = {0};
            LPWSAQUERYSET pResultQS = (LPWSAQUERYSET)buffer;
            DWORD bufferLength = sizeof(buffer);
            status = WSALookupServiceNext(hLookup, flags, &bufferLength, pResultQS);
            //Py_END_ALLOW_THREADS;
            if (NO_ERROR == status) {
                int proto;
                int port;
                
                CSADDR_INFO* csinfo = NULL;

                // set host name
                //record->host = localAddressBuf;
                //
                //// set service name
                //record->name = (const char*)pResultQS->lpszServiceInstanceName;
                //
                //// set description
                //record->description = (const char*)pResultQS->lpszComment;
                
                // set protocol and port
                csinfo = pResultQS->lpcsaBuffer;
                if (csinfo != NULL) {
                    proto = csinfo->iProtocol;
                    port = ((SOCKADDR_BTH*)csinfo->RemoteAddr.lpSockaddr)->port;

                    record->port = port;
                    if (proto == BTHPROTO_RFCOMM) {
                        record->protocol = "RFCOMM";
                    }
                    else if (proto == BTHPROTO_L2CAP) {
                        record->protocol = "L2CAP";
                    }
                    else {
                        record->protocol = "UNKNOWN";
                        record->port = -1;
                    }
                }
                else {
                    record->protocol = "UNKNOWN";
                    record->port = -1;
                }

                // add the raw service record
                if(pResultQS->lpBlob&&pResultQS->lpBlob->cbSize>0)
                {
                    record->rawrecord = (char*)malloc(pResultQS->lpBlob->cbSize);
                    record->rawrecord_len = pResultQS->lpBlob->cbSize;
                    memcpy(record->rawrecord, pResultQS->lpBlob->pBlobData, pResultQS->lpBlob->cbSize);
                }
            }
            else {
                int error = WSAGetLastError();

                if (error == WSAEFAULT) {
                    // the qs data structure is too small.  allocate a bigger
                    // buffer and try again.
                    free(qs);
                    qs = (WSAQUERYSET*)malloc(qs_len);
                }
                else if (error == WSA_E_NO_MORE) {
                    // no more results.
                    done = 1;
                }
                else {
                    // unexpected error.  raise an exception.
                    std::cerr << "unexpected error.  raise an exception!" << std::endl;
                    return -1;
                }
            }
        }
        //Py_BEGIN_ALLOW_THREADS;
        WSALookupServiceEnd(hLookup);
        //Py_END_ALLOW_THREADS;
        free(qs);

        return 0;
    }
    

    int sh_set_service_raw(BYTE* sdpRecord, int sdpRecord_len, int advertise)
    {
        WSAQUERYSET sdpQuerySet = { 0 };
        WSAESETSERVICEOP op;

        BTH_SET_SERVICE* si = NULL;
        int silen = -1;
        ULONG sdpVersion = BTH_SDP_VERSION;
        BLOB blob = { 0 };
        int status = -1;
   

        silen = sizeof(BTH_SET_SERVICE) + sdpRecord_len - 1;
        si = (BTH_SET_SERVICE*)malloc(silen);
        if (!si)
        {
            printf("malloc() failed: %d\n", WSAGetLastError());
            return -1;
        }   
        ZeroMemory(si, silen);

        si->pSdpVersion = &sdpVersion;
        si->pRecordHandle = NULL;
        si->fCodService = 0;
        si->Reserved;
        si->ulRecordLength = sdpRecord_len;
        memcpy(si->pRecord, sdpRecord, sdpRecord_len);
        op = advertise ? RNRSERVICE_REGISTER : RNRSERVICE_DELETE;

        blob.cbSize = silen;
        blob.pBlobData = (BYTE*)si;

        sdpQuerySet.dwSize = sizeof(sdpQuerySet);
        sdpQuerySet.lpBlob = &blob;
        sdpQuerySet.dwNameSpace = NS_BTH;

        status = WSASetService(&sdpQuerySet, op, 0);
        free(si);

        if (SOCKET_ERROR == status) {
            printf("WSASetService() failed: %d\n", WSAGetLastError());
            return -1;
        }

        return 0;
    }
   
    int sh_set_service(SOCKET sockfd, const char* service_name, const char* service_desc, const char* service_class_id_str, int advertise)
    {
        WSAQUERYSET sdpQuerySet = { 0 };
        WSAESETSERVICEOP op;

        SOCKADDR_BTH sa = { 0 };
        int sa_len = sizeof(sa);
 
        CSADDR_INFO sockInfo = { 0 };
        GUID uuid = { 0 };
        
        op = advertise ? RNRSERVICE_REGISTER : RNRSERVICE_DELETE;

        if (SOCKET_ERROR == getsockname(sockfd, (SOCKADDR*)&sa, &sa_len)) {
            printf("getsockname() failed: %d\n", WSAGetLastError());
            return -1;
		}

        sockInfo.iProtocol = BTHPROTO_RFCOMM;
        sockInfo.iSocketType = SOCK_STREAM;
        sockInfo.LocalAddr.lpSockaddr = (LPSOCKADDR)&sa;
        sockInfo.LocalAddr.iSockaddrLength = sizeof(sa);
        sockInfo.RemoteAddr.lpSockaddr = NULL;
        sockInfo.RemoteAddr.iSockaddrLength = 0;

        sdpQuerySet.dwSize = sizeof(WSAQUERYSET);
        sdpQuerySet.dwNameSpace = NS_BTH;
        sdpQuerySet.lpcsaBuffer = &sockInfo;
        sdpQuerySet.lpszServiceInstanceName = ConvertLPSTRToLPWSTR((LPSTR)service_name);
        sdpQuerySet.lpszComment = ConvertLPSTRToLPWSTR((LPSTR)service_desc);
		str2uuid(service_class_id_str, &uuid);
        sdpQuerySet.lpServiceClassId = (LPGUID)&uuid;
        sdpQuerySet.dwNumberOfCsAddrs = 1;
        
		if (SOCKET_ERROR == WSASetService(&sdpQuerySet, op, 0)) {
			printf("WSASetService() failed: %d\n", WSAGetLastError());
			return -1;
		}

        return 0;
    }
    
    int sh_setblocking(SOCKET sockfd, int block)
    {
        unsigned long sh_block = !block; //set zero to non-zero and non-zero to zero
        ioctlsocket(sockfd, FIONBIO, &sh_block);

        return 0;
    }

    int sh_settimeout(SOCKET sockfd, double secondTimeout)
    {
        DWORD timeout = -1;
        int timeoutLen = sizeof(DWORD);
        timeout = (DWORD)(secondTimeout * 1000);

        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,
            timeoutLen) == 0) {
            printf("setsockopt() receive failed: %d\n", WSAGetLastError());
            return 0;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout,
            timeoutLen) != SOCKET_ERROR) {
            printf("setsockopt() send failed: %d\n", WSAGetLastError());
            return -1;
        }
        return 0;
    }
    
    int sh_gettimeout(SOCKET sockfd)
    {
        DWORD recv_timeout = -1;
        int recv_timeoutLen = sizeof(DWORD);
        double timeout = -1;

        if (getsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&recv_timeout,
            &recv_timeoutLen) == SOCKET_ERROR) {
            printf("setsockopt() receive failed: %d\n", WSAGetLastError());
            return -1;
        }

        timeout = (double)recv_timeout / 1000;

        return (int)timeout;
    }
    
    int sh_setsockopt(SOCKET sockfd, int level, ULONG optname, ULONG flag)
    {
        if (setsockopt(sockfd, level, optname, (char*)&flag, sizeof(ULONG))  == SOCKET_ERROR) {
            printf("setsockopt() failed: %d\n", WSAGetLastError());
            return -1;
        }
        return 0;
    }

    int sh_getsockopt(SOCKET sockfd, int level, ULONG optname)
    {
        ULONG flag;
        int flagsize = sizeof flag;

        if (getsockopt(sockfd, level, optname, (char*)&flag, &flagsize) == SOCKET_ERROR) {
            printf("getsockopt() failed: %d\n", WSAGetLastError());
            return -1;
        }
        return flag;
    }
    HANDLE sh_register_auth(const BLUETOOTH_DEVICE_INFO& deviceInfo)
    {
        HBLUETOOTH_AUTHENTICATION_REGISTRATION handle = NULL;
        DWORD dwRet = BluetoothRegisterForAuthenticationEx(&deviceInfo, &handle, &auth_callback_ex, NULL);
        if (dwRet != ERROR_SUCCESS)
        {
            DWORD err = GetLastError();
            cout << "BluetoothRegisterForAuthentication Error" << err << endl;
            return NULL;
        }

        return handle;
    }

    int sh_unregister_auth(HANDLE authHandle)
    {
        BluetoothUnregisterAuthentication(authHandle);
        return 0;
    }
    int sh_auth_device(const BLUETOOTH_DEVICE_INFO& deviceInfo)
    {
        DWORD result = 0;
        if (!deviceInfo.fAuthenticated)
        {
            result = BluetoothAuthenticateDeviceEx(
                NULL,
                NULL,
                const_cast<BLUETOOTH_DEVICE_INFO*>(&deviceInfo),
                NULL,
                MITMProtectionNotRequired
            );
            if (result != ERROR_SUCCESS) {
                switch (result)
                {
                case ERROR_CANCELLED:
                    cerr << "用户取消了身份验证或配对操作" << endl;
                    break;
                case ERROR_INVALID_PARAMETER:
                    cerr << "传递给函数的参数无效" << endl;
                    break;
                case ERROR_NO_MORE_ITEMS:
                    cerr << "没有更多的设备可以配对" << endl;
                    break;
                case ERROR_NOT_SUPPORTED:
                    cerr << "不支持请求的操作" << endl;
                    break;
                case ERROR_GEN_FAILURE:
                    cerr << "通用失败错误，连接到系统的设备不能正常工作。" << endl;
                    break;
                case ERROR_BUSY:
                    cerr << "蓝牙堆栈忙" << endl;
                    break;
                case ERROR_TIMEOUT:
                    cerr << "操作超时" << endl;
                    break;
                case ERROR_DEVICE_NOT_CONNECTED:
                    cerr << "蓝牙设备未连接" << endl;
                    break;
                case ERROR_DEVICE_NOT_AVAILABLE:
                    cerr << "设备不可用" << endl;
                    break;
                case ERROR_NOT_AUTHENTICATED:
                    cout << "由于用户尚未经过身份验证，因此未执行请求的操作。" << endl;
                    break;
                default:
                    cerr <<"其他错误代码："<< result << ",校验码出错，请手动进行设备连接" << endl;
                    break;
                }
                goto end;
            }
        }

		cout << "身份验证成功，蓝牙设备已成功配对" << endl;
	end:
		if (BluetoothUpdateDeviceRecord(&deviceInfo) != ERROR_SUCCESS)
		{
			cerr << "更新本地蓝牙无缓存设备记录失败！" << endl;
		}
		else
		{
			cout << "更新本地蓝牙无缓存设备记录成功！" << endl;
		}

        return result;
    }

    int sh_pair_device(PBLUETOOTH_DEVICE_INFO device, PWSTR passkey, HANDLE hRadioHandle)
    {
        if (device == nullptr)
            return -1;

        PWSTR AUTHENTICATION_PASSKEY = const_cast<PWSTR>(passkey);
        HBLUETOOTH_AUTHENTICATION_REGISTRATION hCallbackHandle = 0;
        DWORD result = 0;
        if (!device->fAuthenticated )
        {
            result = BluetoothAuthenticateDeviceEx(
                NULL,
                hRadioHandle,
                device,
                NULL,
                MITMProtectionRequiredBonding
            );
            if (result != ERROR_SUCCESS) {
                switch (result)
                {
                case ERROR_CANCELLED:
                    cout << "用户取消了身份验证或配对操作" << endl;
                    break;
                case ERROR_INVALID_PARAMETER:
                    cout << "传递给函数的参数无效" << endl;
                    break;
                case ERROR_NO_MORE_ITEMS:
                    cout << "没有更多的设备可以配对" << endl;
                    break;
                case ERROR_NOT_SUPPORTED:
                    cout << "不支持请求的操作" << endl;
                    break;
                case ERROR_GEN_FAILURE:
                    cerr << "通用失败错误，连接到系统的设备不能正常工作。" << endl;
                    break;
                case ERROR_BUSY:
                    cout << "蓝牙堆栈忙" << endl;
                    break;
                case ERROR_TIMEOUT:
                    cout << "操作超时" << endl;
                    break;
                case ERROR_DEVICE_NOT_CONNECTED:
                    cout << "蓝牙设备未连接" << endl;
                    break;
                case ERROR_DEVICE_NOT_AVAILABLE:
                    cout << "设备不可用" << endl;
                    break;
                case ERROR_NOT_AUTHENTICATED:
                    cout << "由于用户尚未经过身份验证，因此未执行请求的操作。" << endl;
                    break;
                default:
                    cout << result << ",校验码出错，请手动进行设备连接" << endl;
                    break;
                }
                goto end;
            }
        }
    

		cout << "身份验证成功，蓝牙设备已成功配对" << endl;;

	end:
		if (BluetoothUpdateDeviceRecord(device) != ERROR_SUCCESS)
		{
			cout << "更新本地蓝牙无缓存设备记录失败！" << endl;
		}
		else
		{
			cout << "更新本地蓝牙无缓存设备记录成功！" << endl;
		}

        return result;
    }

    int sh_pair_device_with_addr(std::string addr, PWSTR passkey,  HANDLE hRadioHandle)
    {
        if (addr.empty() || addr.size() != 17)
        {
            return -1;
        }
        BLUETOOTH_DEVICE_INFO device = { sizeof(BLUETOOTH_DEVICE_INFO) };
       
        sscanf_s(addr.c_str(), "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
            &device.Address.rgBytes[5],
            &device.Address.rgBytes[4],
            &device.Address.rgBytes[3],
            &device.Address.rgBytes[2],
            &device.Address.rgBytes[1],
            &device.Address.rgBytes[0]);

		//std::function<bool(void* result) > execute = [&device](void* handler) -> bool {

  //          BLUETOOTH_DEVICE_INFO* info = (BLUETOOTH_DEVICE_INFO*)handler;
  //          if (info&&info->Address.ullLong == device.Address.ullLong)
  //          {
  //              memcpy(&device, info, sizeof(BLUETOOTH_DEVICE_INFO));
  //              return true;
  //          }
  //          return false;
		//};
  //      sh_scan_devices(execute);
        return sh_pair_device(&device, passkey, hRadioHandle);
    }

    int sh_last_error()
    {
        return GET_LAST_ERROR;
    }

    bool sh_is_socket_connected(SOCKET sockfd)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        timeval timeout;
        timeout.tv_sec = 0; 
        timeout.tv_usec = 100;

        int result = select(0, &readfds, nullptr, nullptr, &timeout);
        if (result > 0) {
            if (FD_ISSET(sockfd, &readfds)) {
                char buffer[1];
                int bytesReceived = recv(sockfd, buffer, sizeof(buffer), 0);
                if (bytesReceived == 0) {
                    int error = GET_LAST_ERROR;
                    if (error == WSAEWOULDBLOCK ) {
                        return true;
                    }
                    else
                    { 
                        // 连接已正常关闭
                        std::cerr << "bytesReceived == 0: " << GET_LAST_ERROR << std::endl;
                        return false;
                    }
                }
                else if (bytesReceived == SOCKET_ERROR) {
                    int error = GET_LAST_ERROR;
                    std::cerr << "SOCKET_ERROR == 0: " << GET_LAST_ERROR << std::endl;
                    if (error == WSAECONNRESET || error == WSAENOTCONN) {
                        // 连接被重置或未连接
                        return false;
                    }
                }
            }
        }
        else if (result == 0) {
            // select 超时，套接字没有可读数据
            return true;
        }
        else {
            std::cerr << "Select failed with error: " << GET_LAST_ERROR << std::endl;
            return false; // 发生错误，假定连接已断开
        }

        return true;
    }
}
