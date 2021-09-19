#pragma comment (lib, "setupapi.lib")
#include <iostream>
#include <windows.h>
#include <setupapi.h>
#include <string>
#include <iomanip>

using namespace std;

#define BUFF 300

void outPutTxt(BYTE* bf) {
	char st[200];
	int j = 0;
	for (int i = 0; i < 200; i++) {
		st[i] = ((char)bf[i]);
	}

	cout << "\nVendorID = ";
	while (j != 26) {
		if (j > 7 && j < 24) {
			cout << st[j];
		}
		j++;
	}
	cout << "\nDeviceID = ";
	while (j != 43) {
		if (j > 25 && j < 42) {
			cout << st[j];
		}
		j++;
	}
}

void outPutTxtName(BYTE* bf) {
	char st[300];
	int j = 0;
	cout << "\nName = ";
	for (int i = 0; i < 150; i++) {
		st[i] = ((char)bf[i]);
		if ((int)st[i] == 26 || (int)st[i] == 4 || (int)st[i] == 20 || (int)st[i] == (int)'>' || (int)st[i] == (int)'<' || (int)st[i] == (int)';' || (int)st[i] == (int)'♦' || (int)st[i] == (int)'=' || (int)st[i] == (int)'@' || (int)st[i] == (int)'>' || (int)st[i] == 31)
			continue;
		if ((int)st[i] == 38)
		 break;
		if ((int)st[i] == 92)
			break;
		cout << st[i];
	}

}

void printDeviceInfo(HDEVINFO hdevinfo) {

	SP_DEVINFO_DATA DevInfoDD;
	DevInfoDD.cbSize = sizeof(SP_DEVINFO_DATA);

	BYTE* curBuffer = new BYTE[BUFF];
	int i = 0;
	while (SetupDiEnumDeviceInfo(hdevinfo, i, &DevInfoDD)) {

		if (SetupDiGetDeviceRegistryProperty(hdevinfo, &DevInfoDD, SPDRP_DEVICEDESC, NULL, curBuffer, BUFF, NULL)) {
			outPutTxtName(curBuffer);
		}

		if (SetupDiGetDeviceRegistryProperty(hdevinfo, &DevInfoDD, SPDRP_HARDWAREID, NULL, curBuffer, BUFF, NULL)) {
			outPutTxt(curBuffer);
		}
		i++;
	}
	delete curBuffer;
}

int main() {

	setlocale(LC_ALL, "Russian");

	HDEVINFO hdevinfo = SetupDiGetClassDevs(NULL, L"PCI", NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);

	if (hdevinfo == INVALID_HANDLE_VALUE)
		return GetLastError();

	printDeviceInfo(hdevinfo);

	if (!SetupDiDestroyDeviceInfoList(hdevinfo))
		return -1;

	return 0;
}