/* ====================================================================
 * Copyright (c) 2025-2027 The SHBluetooth Project.  All rights reserved.
 *
 * This product includes Bluetooth Secure Communication System,
 * Authored and developed by David Chin (Contact: luckqinwei@hotmail.com) */

#pragma once


std::string getMAC(BLUETOOTH_ADDRESS Daddress);
std::string String_To_UTF8(const std::string& str);
std::string wstring2string(const std::wstring& ws);
std::string BthAddrToString(BTH_ADDR addr);
void ba2str(BTH_ADDR ba, char* addr, size_t len);
bool str2ba(const std::string& str, BTH_ADDR& bthAddr);
int str2uuid(const char* uuid_str, GUID* uuid);
LPWSTR ConvertLPSTRToLPWSTR(LPSTR lpstr);
LPSTR ConvertLPWSTRToLPSTR(LPWSTR lpwstr);
size_t findMessageEnd(const std::vector<char>& data);
