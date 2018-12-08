#include "Console.h"
#include <Windows.h>

#define INPUT_BUFFER_SIZE 32000

const char defaultBuffer[11] = { 'E', 'l', 'i', 't', 'e', 'D', 'e', 'l', 'e', 't', 'e'};
const char zeroesBuffer[11] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const char onesBuffer[11] = { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};


int main()
{
	Console con;
	int overwriteMethod = 0;
	int iterations = 3;
	char* overwriteBuffer;
	char* inputBuffer = (char*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, INPUT_BUFFER_SIZE);

	con.WriteLine("Please specify overwrite method");
	con.WriteLine("Valid options are: default, zeroes/0, and ones/1");
	con.ReadLine(inputBuffer, INPUT_BUFFER_SIZE);



	if (lstrcmpiA(inputBuffer, "zeroes") == 0)
	{
		overwriteMethod = 1;
	}
	else if (lstrcmpiA(inputBuffer, "0") == 0)
	{
		overwriteMethod = 1;
	}
	else if (lstrcmpiA(inputBuffer, "ones") == 0)
	{
		overwriteMethod = 2;
	}
	else if (lstrcmpiA(inputBuffer, "1") == 0)
	{
		overwriteMethod = 2;
	}
	else
	{
		overwriteMethod = 0;
	}

	switch (overwriteMethod)
	{
	case 1:
		overwriteBuffer = (char*) zeroesBuffer;
		con.WriteLine("Using zeroes overwrite method");
		break;
	case 2:
		overwriteBuffer = (char*) onesBuffer;
		con.WriteLine("Using ones overwrite method");
		break;
	default:
		overwriteBuffer = (char*) defaultBuffer;
		con.WriteLine("Using default overwrite method");
		break;
	}

	RtlSecureZeroMemory(inputBuffer, INPUT_BUFFER_SIZE);

	con.WriteLine("Please specify the number of overwrite iterations");
	con.WriteLine("Valid options are 1-9");

	con.ReadLine(inputBuffer, INPUT_BUFFER_SIZE);

	char firstChar = *inputBuffer;

	if (firstChar >= 49 && firstChar <= 57)
	{
		con.Write("Using ");
		con.Write(inputBuffer);
		con.WriteLine(" iterations");

		iterations = firstChar - 48;
	}
	else
	{
		con.WriteLine("Defaulting to 3 iterations");
	}

	RtlSecureZeroMemory(inputBuffer, INPUT_BUFFER_SIZE);

	con.WriteLine("Please enter file path");

	con.ReadLine(inputBuffer, INPUT_BUFFER_SIZE);

	HANDLE fileHandle = CreateFileA(inputBuffer, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER fileSize;
		LARGE_INTEGER totalWritten;

		if (GetFileSizeEx(fileHandle, &fileSize))
		{
			for (int i = 0; i < iterations; i++)
			{
				con.WriteLine("Overwriting file data...");
				RtlSecureZeroMemory(&totalWritten, sizeof(LARGE_INTEGER));
				
				DWORD written;

				SetFilePointer(fileHandle, 0, 0, 0);

				while (totalWritten.QuadPart < fileSize.QuadPart)
				{
					WriteFile(fileHandle, overwriteBuffer, 11, &written, NULL);
		
 					totalWritten.QuadPart += written;
				}
			}

			con.WriteLine("Finished overwriting file...");

			CloseHandle(fileHandle);

			if (DeleteFileA(inputBuffer))
			{
				con.Write("Deleted ");
				con.WriteLine(inputBuffer);
			}
			else
			{
				con.Write("Failed to delete file: ");
				con.WriteLine(inputBuffer);
			}
		}
	}
	else
	{
		con.WriteLine("Could not find/open file for overwriting...please double check input file path");
	}

	CloseHandle(fileHandle);

	con.ReadLine(inputBuffer, INPUT_BUFFER_SIZE);

	HeapFree(GetProcessHeap(), 0, inputBuffer);
}