#include "stdafx.h"

/*
COMMTIMEOUTS timeouts;

timeouts.ReadIntervalTimeout = 1;
timeouts.ReadTotalTimeoutMultiplier = 1;
timeouts.ReadTotalTimeoutConstant = 1;
timeouts.WriteTotalTimeoutMultiplier = 1;
timeouts.WriteTotalTimeoutConstant = 1;
if (!SetCommTimeouts(m_hCommPort, &timeouts))
// setting timeouts failed.
*/
//from https://stackoverflow.com/questions/6036716/serial-comm-using-writefile-readfile
#include <stdio.h>
#include <conio.h>
#include <string.h>

#ifndef STRICT
	#define STRICT
#endif
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int find_teensy_serial(char *name);

void system_error(char *name) {
	// Retrieve, format, and print out a message from the last error.  The 
	// `name' that's passed should be in the form of a present tense noun 
	// (phrase) such as "opening file".
	//
	char *ptr = NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		0,
		GetLastError(),
		0,
		(LPWSTR)ptr,
		1024,
		NULL);

	TRACE(stderr, "\nError %s: %s\n", name, ptr);
	LocalFree(ptr);
}

void findTeensy(void)
{
	int ii;
	char name[40];
	TRACE("\n **** Find teensy on Serial ports ****\n");
	ii= find_teensy_serial(name);
	TRACE("%d %s\n", ii, name);
}

/*int main(int argc, char **argv) {

	int ch;
	char buffer[1];
	HANDLE file;
	COMMTIMEOUTS timeouts;
	DWORD read, written;
	DCB port;
	HANDLE keyboard = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode;
	char port_name[128] = "\\\\.\\COM3";
	char init[] = ""; // e.g., "ATZ" to completely reset a modem.

	if (argc > 2)
		sprintf(port_name, "\\\\.\\COM%c", argv[1][0]);

	// open the comm port.
	file = CreateFile(port_name,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (INVALID_HANDLE_VALUE == file) {
		system_error("opening file");
		return 1;
	}

	// get the current DCB, and adjust a few bits to our liking.
	memset(&port, 0, sizeof(port));
	port.DCBlength = sizeof(port);
	if (!GetCommState(file, &port))        			system_error("getting comm state");
	if (!BuildCommDCB("baud=19200 parity=n data=8 stop=1", &port))        system_error("building comm DCB");
	if (!SetCommState(file, &port))        			system_error("adjusting port settings");

	// set short timeouts on the comm port.
	timeouts.ReadIntervalTimeout = 1;
	timeouts.ReadTotalTimeoutMultiplier = 1;
	timeouts.ReadTotalTimeoutConstant = 1;
	timeouts.WriteTotalTimeoutMultiplier = 1;
	timeouts.WriteTotalTimeoutConstant = 1;

	if (!SetCommTimeouts(file, &timeouts))        	system_error("setting port time-outs.");

	// set keyboard to raw reading.
	if (!GetConsoleMode(keyboard, &mode))        	system_error("getting keyboard mode");
	mode &= ~ENABLE_PROCESSED_INPUT;
	if (!SetConsoleMode(keyboard, mode))        	system_error("setting keyboard mode");

	//    if (!EscapeCommFunction(file, CLRDTR))        	system_error("clearing DTR");
	//    Sleep(200);
	//    if (!EscapeCommFunction(file, SETDTR))        	system_error("setting DTR");

	//   if ( !WriteFile(file, init, sizeof(init), &written, NULL))        system_error("writing data to port");
	//    if (written != sizeof(init))        			system_error("not all data written to port");

	// basic terminal loop:
	do {
		// check for data on port and display it on screen.
		ReadFile(file, buffer, sizeof(buffer), &read, NULL);
		if (read)
			WriteFile(screen, buffer, read, &written, NULL);

		// check for keypress, and write any out the port.
		if (kbhit()) {
			ch = getch();
			WriteFile(file, &ch, 1, &written, NULL);
			if (written != 1)        			system_error("not all data written to port");
			WriteFile(screen, &ch, 1, &written, NULL);
			if (written != 1)        			system_error("not all data written to screen");
		}
		// until user hits ctrl-backspace.
	} while (ch != 127);

	// close up and go home.
	CloseHandle(keyboard);
	CloseHandle(file);
	return 0;
}
*/

/*********** following for soft reboot on teensy **********************/

int multi_has(void *buf, const char *str)
{
	const char *p;
	int len;

	len = strlen(str);
	for (p = (const char *)buf; *p; p += strlen(p) + 1) { if (_strnicmp(p, str, len) == 0) return 1; }
	return 0;
}

#include <setupapi.h>

int find_teensy_serial(char *name)
{
	GUID guid;
	DWORD n, size, type;
	HDEVINFO info;
	LONG r;
	SP_DEVINFO_DATA data;
	HKEY key;
	BYTE buf[1024];
	int com=0;

	printf("\n **** Find teensy on Serial ports ****\n");
	if (!SetupDiClassGuidsFromName((PCWSTR)"Ports", &guid, 1, &n) ||
		(info = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT)) == INVALID_HANDLE_VALUE)
	{
		printf("Unable to get GUID for Ports class\n");
		return 0;
	}

	for (n = 0; ; n++) {
		printf("%d\n ",n);
		data.cbSize = sizeof(data);
		if (!SetupDiEnumDeviceInfo(info, n, &data)) break;
		printf("\nDevice Instance: %ld\n", data.DevInst);
		if (!SetupDiGetDeviceRegistryProperty(info, &data,
			SPDRP_HARDWAREID, &type, buf, sizeof(buf), NULL))
		{
			continue;
		}
		printf("  DeviceRegistryProperty: acquired\n");
		//		printf_verbose("  type %d %d %s\n", type,REG_MULTI_SZ,buf);

		if (type != REG_MULTI_SZ) continue;
		//		printf_verbose("  Key type: REG_MULTI_SZ\n");
		//		if (verbose) print_multi(buf);
		if (!(multi_has(buf, "USB\\Vid_16c0&Pid_0483")	//USB_SERIAL
			|| multi_has(buf, "USB\\Vid_16c0&Pid_0487")	//USB_SERIAL_HID
			|| multi_has(buf, "USB\\Vid_16c0&Pid_0485")
			|| multi_has(buf, "USB\\Vid_16c0&Pid_0476")	//USB_EVERYTHING
			|| multi_has(buf, "USB\\Vid_16c0&Pid_048A")	//USB_MIDI_AUDIO_SERIAL
			)) continue;

		printf("Found Teensy:\n");
		key = SetupDiOpenDevRegKey(info, &data, DICS_FLAG_GLOBAL,
			0, DIREG_DEV, KEY_QUERY_VALUE);
		if (key == INVALID_HANDLE_VALUE) continue;
		//		printf_verbose("  Registry Key: acquired\n");

		size = sizeof(buf);
		r = RegQueryValueEx(key, (LPCWSTR)"Name", NULL, &type, buf, &size);
		printf("  Name: \"%s\"\n", buf);

		r = RegQueryValueEx(key, (LPCWSTR)"PortName", NULL, &type, buf, &size);
		printf("  PortName: \"%s\"\n", buf);
		RegCloseKey(key);
		if (r != ERROR_SUCCESS || type != REG_SZ) continue;

		com = 0;
		if (sscanf_s((char *)buf, "COM%d", &com) != 1) continue;
		if (com < 1 || com > 999) continue;
		sprintf_s(name, strlen(name), "\\\\.\\COM%d", com);  // Microsoft KB115831
		break;
		//		add_serial_name(name);
	}
	SetupDiDestroyDeviceInfoList(info);
	return com;

}
