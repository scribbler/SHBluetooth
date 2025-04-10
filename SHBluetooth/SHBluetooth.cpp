// SHBluetooth.cpp: 定义应用程序的入口点。
//

#include "SHBluetooth.h"
#include "sh_bluetooth.h"

using namespace std;

int main()
{
	cout << "Hello CMake." << endl;
	bt::sh_pair_device_with_addr("4c:5f:70:22:dd:27", NULL, NULL);
	return 0;
}
