#pragma once

#define NOMINMAX

#include <Windows.h>
#include <vector>
#include <string>

using namespace std;

struct COMPort
{
		COMPort();
		virtual ~COMPort();

		bool IsOK() const;

		void Connect(const wstring& port, int baudrate);
		void Disconnect();

		virtual void Write(string data);
		virtual char Read();

		HANDLE m_Handle;
};

struct Exception{};