
// TeensyControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TeensyControl.h"
#include "TeensyControlDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTeensyControlDlg dialog



CTeensyControlDlg::CTeensyControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TEENSYCONTROL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTeensyControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTeensyControlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTeensyControlDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTeensyControlDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDOK, &CTeensyControlDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTeensyControlDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON3, &CTeensyControlDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CTeensyControlDlg::OnBnClickedButton4)
END_MESSAGE_MAP()

// CTeensyControlDlg message handlers

BOOL CTeensyControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CDateTimeCtrl *dateTimePicker1 = reinterpret_cast<CDateTimeCtrl *>(GetDlgItem(IDC_DATETIMEPICKER1));
	dateTimePicker1->SetFormat(L"yyyy/MM/dd");


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTeensyControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTeensyControlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTeensyControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include "Resource.h"
#define printf printf2

int __cdecl printf2(const char *format, ...)
{
	char str[1024];

	va_list argptr;
	va_start(argptr, format);
	int ret = vsnprintf(str, sizeof(str), format, argptr);
	va_end(argptr);

	OutputDebugStringA(str);

	return ret;
}

void system_error(char *str) { printf2(str); }

char portName[32];
HANDLE file;

void comOpen(void)
{
	COMMTIMEOUTS timeouts;
	DWORD read;
	DCB port;

	wchar_t port_name[32];
	size_t len;
	mbstowcs_s(&len, port_name, 32, portName, 32);

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
	}

	memset(&port, 0, sizeof(port));
	port.DCBlength = sizeof(port);
	if (!GetCommState(file, &port))        			system_error("getting comm state");
	if (!BuildCommDCB(L"baud=19200 parity=n data=8 stop=1", &port))        system_error("building comm DCB");
	if (!SetCommState(file, &port))        			system_error("adjusting port settings");

	// set short timeouts on the comm port.
	timeouts.ReadIntervalTimeout = 1;
	timeouts.ReadTotalTimeoutMultiplier = 1;
	timeouts.ReadTotalTimeoutConstant = 1;
	timeouts.WriteTotalTimeoutMultiplier = 1;
	timeouts.WriteTotalTimeoutConstant = 1;

	if (!SetCommTimeouts(file, &timeouts))        	system_error("setting port time-outs.");

	char ch;
	do
	{	if(BOOL ret = ReadFile(file, &ch, 1, &read, NULL)) continue;
	} while (read>0);

}

void comClose(void)
{
	CloseHandle(file);
}

void T_load(char c, char *text, int nret)
{
	DWORD nn=0;
	DWORD written, read;
	
	char *p = text;
	do { if(BOOL ret = ReadFile(file, p, 1, &read, NULL)) continue; } while (read > 0);
	//
	char txt[4];
	sprintf_s(txt, 4, "?%c", c);
	WriteFile(file, txt, 2, &written, NULL); printf("%s ", txt);
	//
	nn = 0;
	do
	{	BOOL ret = ReadFile(file, p, 1, &read, NULL); if((*p >= ' ') && (*p < 'z')) { nn += read; p += read; }
	if (ret) continue;
	} while (*p != '\n');
	*p = 0;

	printf("%s\n", text);
}

void T_store(char c, char *txt)
{
	char text[32];
	DWORD written;
	sprintf_s(text,32,"!%c%s", c, txt);
	if (WriteFile(file, text, strlen(text), &written, NULL))
		printf("%d %d %s\n", strlen(text), written, text);
	else
		printf("write error %d\n",GetLastError());
}

#include <setupapi.h>

int multi_has(void *buf, const char *str)
{
	const char *p;
	int len;

	len = strlen(str);
	for (p = (const char *)buf; *p; p += strlen(p) + 1) { if (_strnicmp(p, str, len) == 0) return 1; }
	return 0;
}

int findTeensy(char *name)
{
	GUID guid;
	DWORD n, size, type;
	HDEVINFO info;
	LONG r;
	SP_DEVINFO_DATA data;
	HKEY key;
	BYTE buf[1024];
	int com;

	printf("findTeensy\n");

	if (!SetupDiClassGuidsFromName(L"Ports", &guid, 1, &n) ||
		(info = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT)) == INVALID_HANDLE_VALUE)
	{
		printf("Unable to get GUID for Ports class\n");
		return 0;
	}

	for (n = 0; ; n++) {
		data.cbSize = sizeof(data);
		if (!SetupDiEnumDeviceInfo(info, n, &data)) break;

//		printf("\nDevice Instance: %ld\n", data.DevInst);
		if (!SetupDiGetDeviceRegistryProperty(info, &data,
			SPDRP_HARDWAREID, &type, buf, sizeof(buf), NULL))
		{
			continue;
		}
//		printf("  DeviceRegistryProperty: acquired\n");
//		printf("  type %d %d\n", type,REG_MULTI_SZ);
		size_t len;
		char text[128];
		wcstombs_s(&len, text, 128, (wchar_t *)buf, sizeof(buf));
//		printf("%s\n", text);

		if (type != REG_MULTI_SZ) continue;
		//
		if ( !(multi_has(text, "USB\\Vid_16c0&Pid_0483")	//USB_SERIAL
			|| multi_has(text, "USB\\Vid_16c0&Pid_0487")	//USB_SERIAL_HID
			|| multi_has(text, "USB\\Vid_16c0&Pid_0485")
			|| multi_has(text, "USB\\Vid_16c0&Pid_0476")	//USB_EVERYTHING
			|| multi_has(text, "USB\\Vid_16c0&Pid_048A")	//USB_MIDI_AUDIO_SERIAL
			)) continue;

		printf("Found Teensy:\n");
		key = SetupDiOpenDevRegKey(info, &data, DICS_FLAG_GLOBAL,
			0, DIREG_DEV, KEY_QUERY_VALUE);
		if (key == INVALID_HANDLE_VALUE) continue;
//		printf_verbose("  Registry Key: acquired\n");

		size = sizeof(buf);
		r = RegQueryValueEx(key, L"Name", NULL, &type, buf, &size);
		wcstombs_s(&len, text, 128, (wchar_t *)buf, sizeof(buf));
//		printf("  Name: \"%s\"\n", text);

		r = RegQueryValueEx(key, L"PortName", NULL, &type, buf, &size);
		wcstombs_s(&len, text, 128, (wchar_t *)buf, sizeof(buf));
//		printf("  PortName: \"%s\"\n", text);

		RegCloseKey(key);
		if (r != ERROR_SUCCESS || type != REG_SZ) continue;

		com = 0;
		if (swscanf_s((wchar_t*)buf, L"COM%d", &com) != 1) continue;
		if (com < 1 || com > 999) continue;
		sprintf_s(name, 32, "\\\\.\\COM%d", com);  // Microsoft KB115831
		break;

	}
//	printf("%d\n", com);
	return com;
}

/*
!g val\n:       ESM_Logger sets "gravar" value
!p val\n : ESM_Logger sets "parar" value
!i  val\n : ESM_Logger sets "inico" value
!u  val\n : ESM_Logger sets "second" value
!v  val\n : ESM_Logger sets "third" value
!f val\n : ESM_Logger sets "fim" value
!n val\n : ESM_Logger sets "nome" value
!d datestring\n ESM_Logger sets date
!t timestring\n ESM_Logger sets time
!x delay\n      ESM_Logger exits menu and hibernates for the amount given in delay
*/

void CTeensyControlDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	// load configuration from Teensy
	SYSTEMTIME st, lt;

	int T1=8, T2=12, T3=13, T4=18, ON=1, OFF=1;
	wchar_t name[5]=L"WMXZ";

	wchar_t text[40];
	char txt[40];

	if(!findTeensy(portName)) return;
	comOpen();

	GetSystemTime(&st);
	GetLocalTime(&lt);

	// ge first PC local time
	swprintf(text,40, L"%02d:%02d:%02d", lt.wHour, lt.wMinute, lt.wSecond);
	SetDlgItemText(IDC_EDIT9, (const wchar_t *)text);

	// read from devive and show on gui
	T_load('i', txt,2);
	sscanf_s(txt, "%d", &T1);
	swprintf(text, 40, L"%02d", T1);
	SetDlgItemText(IDC_EDIT1, (const wchar_t *)text);

	T_load('u', txt,2);
	sscanf_s(txt, "%d", &T2);
	swprintf(text, 40, L"%02d", T2);
	SetDlgItemText(IDC_EDIT2, (const wchar_t *)text);

	T_load('v', txt,2);
	sscanf_s(txt, "%d", &T3);
	swprintf(text, 40, L"%02d", T3);
	SetDlgItemText(IDC_EDIT3, (const wchar_t *)text);

	T_load('f', txt,2);
	sscanf_s(txt, "%d", &T4);
	swprintf(text, 40, L"%02d", T4);
	SetDlgItemText(IDC_EDIT4, (const wchar_t *)text);

	T_load('g', txt,2);
	sscanf_s(txt, "%d", &ON);
	swprintf(text, 40, L"%2d", ON);
	SetDlgItemText(IDC_EDIT5, (const wchar_t *)text);

	T_load('p', txt,2);
	sscanf_s(txt, "%d", &OFF);
	swprintf(text, 40, L"%2d", OFF);
	SetDlgItemText(IDC_EDIT6, (const wchar_t *)text);

	T_load('n', txt,4);
	size_t len=0;
	mbstowcs_s(&len, text, 16, txt, 16);
	SetDlgItemText(IDC_EDIT7, (const wchar_t *)text);

	T_load('d', txt, 10);
	len = 0;
	mbstowcs_s(&len, text, 16, txt, 16);
	SetDlgItemText(IDC_DATETIMEPICKER1, (const wchar_t *)text);

	T_load('t', txt, 8);
	len = 0;
	mbstowcs_s(&len, text, 16, txt, 16);
	SetDlgItemText(IDC_EDIT8, (const wchar_t *)text);

	printf("The system time is: %02d:%02d\n", st.wHour, st.wMinute);
	printf("The local time is: %02d:%02d\n", lt.wHour, lt.wMinute);
	comClose();
}


void CTeensyControlDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	// store configuration to Teensy
	int T1 = 8, T2 = 12, T3 = 13, T4 = 18, ON = 1, OFF = 1;
	wchar_t name[5] = L"WMXZ";

	wchar_t text[16];
	char txt[16];

	comOpen();

	// read from devive and show on gui
	GetDlgItemText(IDC_EDIT1, text, 16);
	swscanf_s((const wchar_t *)text, L"%2d", &T1);
	sprintf_s(txt,16, "%02d", T1);
	T_store('i',txt);

	GetDlgItemText(IDC_EDIT2, text, 16);
	swscanf_s((const wchar_t *)text, L"%2d", &T2);
	sprintf_s(txt, 16, "%02d", T2);
	T_store('u', txt);

	GetDlgItemText(IDC_EDIT3, text, 16);
	swscanf_s((const wchar_t *)text, L"%2d", &T3);
	sprintf_s(txt, 16, "%02d", T3);
	T_store('v', txt);

	GetDlgItemText(IDC_EDIT4, text, 16);
	swscanf_s((const wchar_t *)text, L"%2d", &T4);
	sprintf_s(txt, 16, "%02d", T4);
	T_store('f', txt);

	GetDlgItemText(IDC_EDIT5, text, 16);
	swscanf_s((const wchar_t *)text, L"%2d", &ON);
	sprintf_s(txt, 16, "%02d", ON);
	T_store('g', txt);

	GetDlgItemText(IDC_EDIT6, text, 16);
	swscanf_s((const wchar_t *)text, L"%2d", &OFF);
	sprintf_s(txt, 16, "%02d", OFF);
	T_store('p', txt);

	GetDlgItemText(IDC_EDIT7, text, 16);
	size_t len;
	wcstombs_s(&len, txt, 16, text, 16);
	T_store('n', txt);

	comClose();

}


void CTeensyControlDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();
}


void CTeensyControlDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void CTeensyControlDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	// start
	DWORD written;
	comOpen();
	WriteFile(file, "x1", 2, &written, NULL);
	comClose();
}


void CTeensyControlDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	// sync time
	SYSTEMTIME lt;
	GetLocalTime(&lt);

	char txt[32];
	sprintf_s(txt,32,"%02d:%02d:%02d", lt.wHour, lt.wMinute, lt.wSecond);

	comOpen();
	T_store('t', txt);

	wchar_t text[32];
	size_t len;
	len = 0;
	mbstowcs_s(&len, text, 16, txt, 16);
	SetDlgItemText(IDC_EDIT8, (const wchar_t *)text);
	SetDlgItemText(IDC_EDIT9, (const wchar_t *)text);

	sprintf_s(txt, 32, "%04d/%02d/%02d", lt.wYear,lt.wMonth,lt.wDay);
	T_store('d', txt);
	comClose();

	len = 0;
	mbstowcs_s(&len, text, 16, txt, 16);
	SetDlgItemText(IDC_DATETIMEPICKER1, (const wchar_t *)text);
}
