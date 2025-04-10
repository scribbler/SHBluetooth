/* ====================================================================
 *
 * Copyright (c) 2025-2027 The SHBluetooth Project.  All rights reserved.
 *
 * ====================================================================
 * This product includes Bluetooth Secure Communication System,
 * Authored and developed by David Chin (Contact: luckqinwei@hotmail.com) */

#include "pch.h"
#include "bt_common.h"

// Function to convert LPSTR to LPWSTR
LPWSTR ConvertLPSTRToLPWSTR(LPSTR lpstr) {
	int len = MultiByteToWideChar(CP_ACP, 0, lpstr, -1, NULL, 0);
	LPWSTR lpwstr = new WCHAR[len];
	MultiByteToWideChar(CP_ACP, 0, lpstr, -1, lpwstr, len);
	return lpwstr;
}

// Function to convert LPWSTR to LPSTR
LPSTR ConvertLPWSTRToLPSTR(LPWSTR lpwstr) {
	int len = WideCharToMultiByte(CP_ACP, 0, lpwstr, -1, NULL, 0, NULL, NULL);
	LPSTR lpstr = new CHAR[len];
	WideCharToMultiByte(CP_ACP, 0, lpwstr, -1, lpstr, len, NULL, NULL);
	return lpstr;
}

string wstring2string(const wstring ws)
{
	string curLocale = setlocale(LC_ALL, NULL);
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char* _Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	size_t convertedChars = 0;
	wcstombs_s(&convertedChars, _Dest, sizeof(_Dest), _Source, _Dsize);

	string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}
string String_To_UTF8(const string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}
string getMAC(BLUETOOTH_ADDRESS Daddress)
{
	/*string addr;
	addr = addr.sprintf("%02x:%02x:%02x:%02x:%02x:%02x", Daddress.rgBytes[5], Daddress.rgBytes[4], Daddress.rgBytes[3], Daddress.rgBytes[2], Daddress.rgBytes[1], Daddress.rgBytes[0]);
	return addr;*/
	ostringstream oss;
	oss << hex << setfill('0') << uppercase;
	for (int i = 5; i >= 0; --i) {
		oss << setw(2) << static_cast<int>(Daddress.rgBytes[i]);
		if (i > 0) {
			oss << ":";
		}
	}
	return oss.str();
}
string BthAddrToString(BTH_ADDR addr) {
	BYTE bytes[6];
	bytes[0] = (BYTE)((addr >> 40) & 0xFF);
	bytes[1] = (BYTE)((addr >> 32) & 0xFF);
	bytes[2] = (BYTE)((addr >> 24) & 0xFF);
	bytes[3] = (BYTE)((addr >> 16) & 0xFF);
	bytes[4] = (BYTE)((addr >> 8) & 0xFF);
	bytes[5] = (BYTE)(addr & 0xFF);

	char buffer[20] = { 0 };
		
	snprintf(buffer, 20, "%02X:%02X:%02X:%02X:%02X:%02X",
		bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);

	return string(buffer);
}

void ba2str(BTH_ADDR ba, char* addr, size_t len)
{
	int i;
	unsigned char bytes[6];
	for (i = 0; i < 6; i++) {
		bytes[5 - i] = (unsigned char)((ba >> (i * 8)) & 0xff);
	}
	sprintf_s(addr, len, "%02X:%02X:%02X:%02X:%02X:%02X",
		bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);
}

bool str2ba(const std::string& str, BTH_ADDR& bthAddr) {
	SOCKADDR_BTH sockaddrBth = { 0 };
	int sockaddrBthLen = sizeof(sockaddrBth);

	// Initialize WSA
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed." << std::endl;
		return false;
	}

	// Prepare the string for WSAStringToAddress
	std::string addressString = "00:00:00:00:00:00 " + str;
	LPSTR addrString = const_cast<LPSTR>(addressString.c_str());

	// Convert string to SOCKADDR_BTH
	if (WSAStringToAddress(ConvertLPSTRToLPWSTR(addrString), AF_BTH, nullptr, (LPSOCKADDR)&sockaddrBth, &sockaddrBthLen) != 0) {
		std::cerr << "WSAStringToAddress failed." << std::endl;
		WSACleanup();
		return false;
	}

	// Extract BTH_ADDR
	bthAddr = sockaddrBth.btAddr;

	// Clean up WSA
	WSACleanup();

	return true;
}

int str2uuid(const char* uuid_str, GUID* uuid)
{
	// Parse uuid128 standard format: 12345678-9012-3456-7890-123456789012
	int i;
	char buf[20] = { 0 };

	strncpy_s(buf, _countof(buf), uuid_str, 8);
	uuid->Data1 = strtoul(buf, NULL, 16);
	memset(buf, 0, sizeof(buf));

	strncpy_s(buf, _countof(buf), uuid_str + 9, 4);
	uuid->Data2 = (unsigned short)strtoul(buf, NULL, 16);
	memset(buf, 0, sizeof(buf));

	strncpy_s(buf, _countof(buf), uuid_str + 14, 4);
	uuid->Data3 = (unsigned short)strtoul(buf, NULL, 16);
	memset(buf, 0, sizeof(buf));

	strncpy_s(buf, _countof(buf), uuid_str + 19, 4);
	strncpy_s(buf + 4, _countof(buf) - 4, uuid_str + 24, 12);
	for (i = 0; i < 8; i++) {
		char buf2[3] = { buf[2 * i], buf[2 * i + 1], 0 };
		uuid->Data4[i] = (unsigned char)strtoul(buf2, NULL, 16);
	}

	return 0;
}

size_t findMessageEnd(const std::vector<char>& data) {
	const size_t DATAEND_SIZE = sizeof(DATAEND);

	if (data.size() < DATAEND_SIZE) {
		return std::string::npos; // 数据不足以包含结束标志
	}

	// 查找结束标志的位置
	for (size_t i = 0; i <= data.size() - DATAEND_SIZE; ++i) {
		if (std::equal(data.begin() + i, data.begin() + i + DATAEND_SIZE, DATAEND)) {
			return i; // 返回结束标志的起始位置
		}
	}

	return std::string::npos; // 未找到结束标志
}
