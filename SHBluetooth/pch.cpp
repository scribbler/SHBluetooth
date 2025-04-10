// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"

// 当使用预编译的头时，需要使用此源文件，编译才能成功。

#pragma comment(lib, "bthprops.lib")
#pragma comment(lib, "ws2_32.lib")

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4995)
