
project(BluetoothClientDemo)

add_executable(${PROJECT_NAME} "BluetoothClientDemo.cpp" "BluetoothClientDemo.h")

target_link_libraries(${PROJECT_NAME} "SHBluetooth")
target_include_directories(${PROJECT_NAME} 
							PRIVATE
							${CMAKE_SOURCE_DIR}/SHBluetooth/include
)
