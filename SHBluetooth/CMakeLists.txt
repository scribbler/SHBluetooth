# CMakeList.txt: SHBluetooth 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required(VERSION 3.12)
project(SHBluetooth)

### 设置C++标准
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

### 创建可执行文件或库 add_library
add_library ( ${PROJECT_NAME} SHARED )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SHBluetooth PROPERTY CXX_STANDARD 20)
endif()

### 设置字符集为 Unicode
target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)

### 添加头文件目录
target_include_directories(${PROJECT_NAME} 
							PRIVATE
							${CMAKE_CURRENT_SOURCE_DIR}/include
)


### 添加源文件
file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
target_sources(${PROJECT_NAME} 
				PRIVATE 
				${HEADER_FILES}
				${CMAKE_CURRENT_SOURCE_DIR}/SHBluetooth.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/bt_common.h
				${CMAKE_CURRENT_SOURCE_DIR}/bt_common.cpp
				
				${CMAKE_CURRENT_SOURCE_DIR}/pch.h
				${CMAKE_CURRENT_SOURCE_DIR}/pch.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/sh_bluetooth.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/ThreadSafeClientSocket.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/ThreadSafeServerSocket.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/ThreadSafeClientSocketImpl.h
				${CMAKE_CURRENT_SOURCE_DIR}/ThreadSafeClientSocketImpl.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/ThreadSafeServerSocketImpl.h
				${CMAKE_CURRENT_SOURCE_DIR}/ThreadSafeServerSocketImpl.cpp
)

### 添加库依赖
target_link_libraries( ${PROJECT_NAME}

)

### 定义宏
target_compile_definitions(${PROJECT_NAME} 
							PRIVATE 
							SHBLUETOOTH_SHARED_LIBRARY
)

### 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # 可执行文件输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # 动态库输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # 静态库输出目录


### 安装目标
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin # 可执行文件安装目录
        LIBRARY DESTINATION lib # 动态库安装目录
        ARCHIVE DESTINATION lib # 静态库安装目录
)

### 安装头文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ 
        DESTINATION include # 安装头文件到 include 目录
        FILES_MATCHING PATTERN "*.h" # 仅匹配 .h 文件
)
