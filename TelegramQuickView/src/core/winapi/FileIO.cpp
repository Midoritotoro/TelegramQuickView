#include "FileIO.h"

bool WriteChannelsToFile(LPCWSTR channels, bool replaceCurrentChannels)
{
	LPCWSTR FileName = TEXT("TargetChannels.txt");
	HANDLE File = NULL;
	DWORD lpNumberOfBytesWritten = NULL, Count = NULL, dwSize = NULL;

	File = CreateFile(FileName, GENERIC_WRITE | GENERIC_READ, 0, NULL, replaceCurrentChannels ? TRUNCATE_EXISTING : OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (File == INVALID_HANDLE_VALUE) {
		CloseHandle(File);
		return false;
	}

	_Analysis_assume_(File != NULL);
	int utf8BufferSize = WideCharToMultiByte(CP_UTF8, 0, channels, -1, NULL, 0, NULL, NULL);
	char* utf8Buffer = new char[utf8BufferSize];
	WideCharToMultiByte(CP_UTF8, 0, channels, -1, utf8Buffer, utf8BufferSize, NULL, NULL);
	dwSize = strlen(utf8Buffer) * sizeof(char);
	replaceCurrentChannels ? SetFilePointer(File, 0, NULL, FILE_BEGIN) : SetFilePointer(File, 0, NULL, FILE_END);
	
	if (!WriteFile(File, utf8Buffer, dwSize, &lpNumberOfBytesWritten, NULL)) {
		CloseHandle(File);
		return false;
	}

	CloseHandle(File);

	return true;
}

char* ReadChannelsFromFile() {
	LPCWSTR fileName = TEXT("TargetChannels.txt");
    HANDLE file = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file == INVALID_HANDLE_VALUE)
        return NULL;

    DWORD fileSize = GetFileSize(file, NULL);
    if (fileSize == INVALID_FILE_SIZE)
    {
        CloseHandle(file);
        return NULL;
    }

    char* buffer = new char[fileSize + 1];
    DWORD bytesRead;

    if (!ReadFile(file, buffer, fileSize, &bytesRead, NULL))
    {
        CloseHandle(file);
        delete[] buffer;
        return NULL;
    }

    buffer[fileSize] = '\0';
    CloseHandle(file);

    return buffer;
}