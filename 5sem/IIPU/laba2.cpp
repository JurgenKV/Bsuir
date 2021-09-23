#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")
using namespace std;

int main()
{
	setlocale(LC_ALL, "Russian");

	//////// Изготовитель и firmware
	WCHAR path[] = L"\\\\.\\PhysicalDrive0";
	DWORD len = wcslen(path);
	static BYTE buffer[0x1000];
	DWORD retn;
	STORAGE_PROPERTY_QUERY ptr = { StorageDeviceProperty, PropertyStandardQuery };
	HANDLE disk = CreateFile(path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	STORAGE_DEVICE_DESCRIPTOR* descriptor = (STORAGE_DEVICE_DESCRIPTOR*)buffer;
	if (!DeviceIoControl(disk, IOCTL_STORAGE_QUERY_PROPERTY, &ptr, sizeof(ptr), buffer, sizeof(buffer), &retn, 0));

	////////
	// Основная информация
	char VolumeNameBuffer[100];
	char FileSystemNameBuffer[100];
	unsigned long VolumeSerialNumber;
	BOOL GetVolumeInformationFlag = GetVolumeInformationA("c:\\", VolumeNameBuffer, 100, &VolumeSerialNumber, NULL, NULL, FileSystemNameBuffer, 100);
	//
	if (GetVolumeInformationFlag != 0)
	{
		cout << "Модель ";
		if (!(VolumeNameBuffer == "hdd")) {
			cout << VolumeNameBuffer << "ssd" << endl;
		}
		cout << VolumeNameBuffer;
		cout << "Серийный номер " << VolumeSerialNumber << endl;
		cout << "Тип файловой системы " << FileSystemNameBuffer << endl;
		if (descriptor->ProductIdOffset > 0)
			cout << "Изготовитель " << buffer + descriptor->ProductIdOffset << endl;
		cout << "Firmware " << buffer + descriptor->ProductRevisionOffset << endl;
	}
	else cout << "Error" << endl;
	// Память
	unsigned long long lpTotalNumberOfBytes;
	unsigned long long lpTotalNumberOfFreeBytes;
	GetDiskFreeSpaceEx(L"c:\\", NULL, (PULARGE_INTEGER)&lpTotalNumberOfBytes, (PULARGE_INTEGER)&lpTotalNumberOfFreeBytes);

	std::cout << "\nОбъем памяти    - " << lpTotalNumberOfBytes << endl;
	std::cout << "Свободно памяти -  " << lpTotalNumberOfFreeBytes << endl;
	std::cout << "Занято памяти   -  " << lpTotalNumberOfBytes - lpTotalNumberOfFreeBytes << endl;

	MEMORYSTATUS stat;

	GlobalMemoryStatus(&stat);

	cout << stat.dwTotalPhys << " - общая физическая память\n";
	cout << stat.dwTotalVirtual << " - общая виртуальная память\n";
	cout << stat.dwAvailPhys << " - свободная физическая память\n";
	cout << stat.dwAvailVirtual << " - свободная виртуальная память\n";

	int typeInterf;
	cout << "\nТип: " << endl;
	typeInterf = GetDriveType(L"c:\\");
	if (typeInterf == DRIVE_UNKNOWN) cout << "UNKNOWN" << endl;
	if (typeInterf == DRIVE_NO_ROOT_DIR) cout << "DRIVE NO ROOT DIR" << endl;
	if (typeInterf == DRIVE_REMOVABLE) cout << "REMOVABLE" << endl;
	if (typeInterf == DRIVE_FIXED) cout << "FIXED" << endl;
	if (typeInterf == DRIVE_REMOTE) cout << "REMOTE" << endl;
	if (typeInterf == DRIVE_CDROM) cout << "CDROM" << endl;
	if (typeInterf == DRIVE_RAMDISK) cout << "RAMDISK" << endl;

	return 0;
}