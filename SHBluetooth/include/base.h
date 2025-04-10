/* ====================================================================
 * Copyright (c) 2025-2027 The SHBluetooth Project.  All rights reserved.
 *
 * This product includes Bluetooth Secure Communication System,
 * Authored and developed by David Chin (Contact: luckqinwei@hotmail.com) */

#ifndef SHBLUETOOTH_HEADER_BASE_H
#define SHBLUETOOTH_HEADER_BASE_H


 // This file should be the first included by all Bluetooth headers.

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <functional>

#if defined(__cplusplus)
extern "C" {
#endif

#define SHBLUETOOTH_API_VERSION 1

#ifndef BT_EXPORT  
#  ifdef _WIN32
#  if defined(SHBLUETOOTH_SHARED_LIBRARY)
#    define BT_EXPORT __declspec(dllexport)
#  else
#    define BT_EXPORT __declspec(dllimport)
#  endif
#  else
#  if defined(SHBT_LIBRARY)
#    define BT_EXPORT __attribute__((visibility("default")))
#  else
#    define BT_EXPORT __attribute__((visibility("default")))
#  endif
#  endif
#endif  // BT_EXPORT

#if defined(__cplusplus)
}  // extern C
#endif  // 

using ReceiveCallback = std::function<void(void* handler, const char*, int)>;
using ExecuteFunction = std::function<bool(void* result)>;
#endif  // SHBLUETOOTH_HEADER_BASE_H
