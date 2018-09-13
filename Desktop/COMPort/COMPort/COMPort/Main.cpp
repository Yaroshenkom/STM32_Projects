// Main.cpp: определяет точку входа для консольного приложения.

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include "COMPort.h"

using namespace std;

int main()
{
	COMPort COM5;
	char rbuf;
	string wbuf;

	COM5.Connect(L"COM5", 19200);
	for (int i = 0; i < 16; i++)
	{
		cin >> wbuf;
		COM5.Write(wbuf);
	}
	/*while (1) {
		cbuf = COM5.Read();
		cout << cbuf << endl;
	}*/
	return 0;
}