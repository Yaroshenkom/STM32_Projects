
#include "stdafx.h"
#include "COMPort.h"
#include <assert.h>
#include <string>
#include <iostream>

#define TIMEOUT 1000

using namespace std;

COMPort::COMPort()
{
	//By default our port is indefined after creation
	m_Handle = INVALID_HANDLE_VALUE;
}

COMPort::~COMPort()
{
	//Close connection
	Disconnect();
}

bool COMPort::IsOK() const
{
	//Check our port's connection is defined
	return m_Handle != INVALID_HANDLE_VALUE;
}

void COMPort::Disconnect()
{
	if (m_Handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_Handle);
		m_Handle = INVALID_HANDLE_VALUE;
	}
}

void COMPort::Connect(const wstring& port, int baudrate)
{
	Disconnect();

	m_Handle =
		CreateFile(
			port.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,									//Access to port can't be shared, so dwShareMode = 0
			NULL,								//When we work with ports IpSecurityAttributes must be NULL
			OPEN_EXISTING,						//When we work with ports dwCreationDistribution must be OPEN_EXISTING
			FILE_ATTRIBUTE_NORMAL,				//When we work with ports we have to use 0 (synchronous work with port) or FILE_FLAG_OVERLAPPED (asynchronous)
			NULL								//When we work with ports hTemplateFile must be NULL because we don't use template file
		);

	//If we can't create correct m_Handle, we'll exit from application
	if (m_Handle == INVALID_HANDLE_VALUE)
	{
		throw Exception();
	}

	//Setting the type of monitored events - A character was received and placed in the input buffer.
	SetCommMask(m_Handle, EV_RXCHAR);
	//Setting the size of input and output buffers for communication wiyh device
	SetupComm(m_Handle, 1500, 1500);

	//Creation of the COMMTIMEOUTS - structure which contains communication device's time-out parametres
	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;		//The maximum time allowed to elapse before the arrival of the next byte on the communications line, in milliseconds.
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;		//The multiplier used to calculate the total time-out period for read operations, in milliseconds. ReadTimeout = ReadTotalTimeoutMultiplier * number of bytes
	CommTimeOuts.ReadTotalTimeoutConstant = TIMEOUT;	//A constant used to calculate the total time-out period for read operations, in milliseconds. This value will be added to ReadTimeout
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;		//The multiplier used to calculate the total time-out period for write operations, in milliseconds. 
	CommTimeOuts.WriteTotalTimeoutConstant = TIMEOUT;	//A constant used to calculate the total time-out period for write operations, in milliseconds.

	//Setting the timeout parametres of the communication device
	if (!SetCommTimeouts(m_Handle, &CommTimeOuts))
	{
		CloseHandle(m_Handle);
		m_Handle = INVALID_HANDLE_VALUE;
		//If we can't correctly configure timeout parametres, we'll close application
		throw Exception();
	}

	//Creation of DCB structure which contains control control setting for a serial communication device
	DCB ComDCM;

	if(!GetCommState(m_Handle, &ComDCM))
	{
		CloseHandle(m_Handle);
		m_Handle = INVALID_HANDLE_VALUE;
		//If we can't correctly configure timeout parametres, we'll close application
		throw Exception();
	}

	memset(&ComDCM, 0, sizeof(ComDCM));				//Fill the the DCB structure with 0
	ComDCM.DCBlength = sizeof(DCB);
	ComDCM.BaudRate = DWORD(baudrate);
	ComDCM.ByteSize = 8;
	ComDCM.Parity = NOPARITY;
	ComDCM.StopBits = ONESTOPBIT;
	ComDCM.fAbortOnError = TRUE;
	ComDCM.fDtrControl = DTR_CONTROL_DISABLE;
	ComDCM.fRtsControl = RTS_CONTROL_DISABLE;
	ComDCM.fBinary = TRUE;
	ComDCM.fParity = FALSE;
	ComDCM.fInX = FALSE;
	ComDCM.fOutX = FALSE;
	ComDCM.XonChar = 0;
	ComDCM.XoffChar = (unsigned char)0xFF;
	ComDCM.fErrorChar = FALSE;
	ComDCM.fNull = FALSE;
	ComDCM.fOutxCtsFlow = FALSE;
	ComDCM.fOutxDsrFlow = FALSE;
	ComDCM.XonLim = 128;
	ComDCM.XoffLim = 128;
}

char COMPort::Read()
{
	DWORD dwCommModemStatus;
	BYTE Rbyte;
	DWORD RBytesTransferred;

	//We are going to catch sending of byte
	SetCommMask(m_Handle, EV_RXCHAR);
	//Wait for character
	WaitCommEvent(m_Handle, &dwCommModemStatus, 0);

	if (dwCommModemStatus & EV_RXCHAR)
	{
		ReadFile(m_Handle, &Rbyte, 1, &RBytesTransferred, 0);
	}

	return Rbyte;
}

void COMPort::Write(string data)
{
	DWORD BytesWritten;

	WriteFile(m_Handle, data.c_str(), 1, &BytesWritten, NULL);
}