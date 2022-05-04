#pragma once
/*
* Author: Mincoding
* LICENSE: MIT
*/
#pragma once;

#include <windows.h>
#include <iostream>

class Serial
{
private:
	HANDLE handler;
	bool connected;
	COMSTAT status;
	DWORD errors;
public:
	explicit Serial(const char* portName, int BaudRate);
	~Serial();

	int read(const char* buffer, unsigned int buf_size);
	bool write(const char* buffer, unsigned int buf_size);
	bool isConnected();
	void close();
};

Serial::Serial(const char* portName, int BaudRate)
{
	this->connected = false;

	this->handler = CreateFileA(static_cast<LPCSTR>(portName),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (this->handler == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			std::cerr << "ERROR: Handle was not attached.Reason : " << portName << " not available\n";
		}
		else
		{
			std::cerr << "ERROR!!!\n";
		}
	}
	else
	{
		DCB dcbSerialParameters = { 0 };

		if (!GetCommState(this->handler, &dcbSerialParameters))
		{
			std::cerr << "Failed to get current serial parameters\n";
		}
		else
		{
			dcbSerialParameters.BaudRate = BaudRate;
			dcbSerialParameters.ByteSize = 8;
			dcbSerialParameters.StopBits = ONESTOPBIT;
			dcbSerialParameters.Parity = NOPARITY;
			dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

			if (!SetCommState(handler, &dcbSerialParameters))
			{
				std::cout << "ALERT: could not set serial port parameters\n";
			}
			else
			{
				this->connected = true;
				PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
				Sleep(2000);
			}
		}
	}
}

Serial::~Serial()
{
	if (this->connected)
	{
		this->connected = false;
		CloseHandle(this->handler);
	}
}

// Reading bytes from serial port to buffer;
// returns read bytes count, or if error occurs, returns 0
int Serial::read(const char* buffer, unsigned int buf_size)
{
	DWORD bytesRead{};
	unsigned int toRead = 0;

	ClearCommError(this->handler, &this->errors, &this->status);

	if (this->status.cbInQue > 0)
	{
		if (this->status.cbInQue > buf_size)
		{
			toRead = buf_size;
		}
		else
		{
			toRead = this->status.cbInQue;
		}
	}

	memset((void*)buffer, 0, buf_size);

	if (ReadFile(this->handler, (void*)buffer, toRead, &bytesRead, NULL))
	{
		return bytesRead;
	}

	return 0;
}

// Sending provided buffer to serial port;
// returns true if succeed, false if not
bool Serial::write(const char* buffer, unsigned int buf_size)
{
	DWORD bytesSend;

	if (!WriteFile(this->handler, (void*)buffer, buf_size, &bytesSend, 0))
	{
		ClearCommError(this->handler, &this->errors, &this->status);
		return false;
	}

	return true;
}

// Checking if serial port is connected
bool Serial::isConnected()
{
	if (!ClearCommError(this->handler, &this->errors, &this->status))
	{
		this->connected = false;
	}

	return this->connected;
}

void Serial::close()
{
	CloseHandle(this->handler);
}
