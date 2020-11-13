#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <psapi.h>
#include <mutex>

using namespace std;

int main()
{
	HANDLE hm = OpenMutexA(SYNCHRONIZE, FALSE, "SServer1");
	if (GetLastError() == ERROR_FILE_NOT_FOUND) {
		hm = CreateMutex(NULL, TRUE, "SServer");
	}
	else if (WaitForSingleObject(hm, 0) == WAIT_ABANDONED || WaitForSingleObject(hm, 0) == WAIT_TIMEOUT)
	{
		MessageBox(NULL, "Server is already running\nClick \"OK\" to exit.", "Server exception", MB_OK);
		CloseHandle(hm);
		return 0;
	}

	//////////////////////// INITIATE //////////////////////////////////
	HANDLE hNamedPipe;
	DWORD dwBytesRead;     // для количества прочитанных байтов
	DWORD dwBytesWrite;    // для количества записанных байтов
	char pchMessage[1024] = "";  // для сообщения
	int nMessageLength;   // длина сообщения
	char pchResponse[2048] = "";
	//////////////////////// CREATEPIPE //////////////////////////////////
	hNamedPipe = CreateNamedPipe(
		"\\\\.\\pipe\\ServerPipe1",  // имя канала
		PIPE_ACCESS_DUPLEX,        // читаем из канала и пишем в канал
		PIPE_TYPE_MESSAGE | PIPE_WAIT,   // синхронная передача сообщений
		PIPE_UNLIMITED_INSTANCES,         // максимальное количество экземпляров канала
		0,         // размер выходного буфера по умолчанию
		0,         // размер входного буфера по умолчанию
		INFINITE,  // клиент ждет связь бесконечно долго
		NULL       // безопасность по умолчанию
	);
	// проверяем на успешное создание
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		cerr << "Create named pipe failed." << "\n"
			<< "The last error code: " << GetLastError() << "\n";
		cout << "Press any key to exit.";
		cin.get();

		return 0;
	}
	//////////////////////// WAIT_FOR_CONNECT //////////////////////////////////
	cout << "The server is waiting for connection with a client." << "\n";
	if (!ConnectNamedPipe(
		hNamedPipe,    // дескриптор канала
		NULL      // связь синхронная
	))
	{
		cerr << "Connect named pipe failed." << "\n"
			<< "The last error code: " << GetLastError() << "\n";
		CloseHandle(hNamedPipe);
		cout << "Press any key to exit.";
		cin.get();
		return 0;
	}
	//////////////////////// READING //////////////////////////////////
	if (!ReadFile(
		hNamedPipe,      // дескриптор канала
		pchMessage,      // адрес буфера для ввода данных
		sizeof(pchMessage),  // количество читаемых байтов
		&dwBytesRead,    // количество прочитанных байтов
		NULL))           // передача данных синхронная
	{
		cerr << "Data reading from the named pipe failed." << "\n"
			<< "The last error code: " << GetLastError() << "\n";
		CloseHandle(hNamedPipe);
		cout << "Press any key to exit.";
		cin.get();
		return 0;
	}
	cout << "Server get message from a client:\n" << pchMessage << "\n";

	//////////////////////// WRITE //////////////////////////////////
	MODULEENTRY32 SnapshotModuleEntry;
	PROCESSENTRY32 SnapshotProcessEntry;
	THREADENTRY32 SnapshotThreadEntry;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	SnapshotProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &SnapshotProcessEntry);
	do {
		char proc_name[260];
		sprintf(proc_name, "%s\0", SnapshotProcessEntry.szExeFile);
		if (strcmp(proc_name, "SServer1.exe") == 0 || strcmp(proc_name, "SServer2.exe") == 0 || strcmp(proc_name, "SClient.exe") == 0) {
			HMODULE ListModules[1024];
			DWORD cbNeeded;
			HANDLE hProcess = OpenProcess(
				PROCESS_ALL_ACCESS,
				false,
				SnapshotProcessEntry.th32ProcessID
			);
			EnumProcessModules(
				hProcess,
				ListModules,
				sizeof(ListModules),
				&cbNeeded
			);

			char* str = new char[100];
			sprintf(str, "Process name: %s\nThreads: %d\nModules:\n\0", proc_name, SnapshotProcessEntry.cntThreads);
			strcat(pchResponse, str);
			delete[] str;
			for (int i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
				TCHAR ModuleName[MAX_PATH];
				GetModuleFileNameEx(
					hProcess,
					ListModules[i],
					ModuleName,
					sizeof(ModuleName)
				);
			//	cout << i << '.' << ModuleName << "\n";
				strcat(pchResponse, ModuleName);
				strcat(pchResponse, "\n");
			}
		}

	} while (Process32Next(hSnapshot, &SnapshotProcessEntry) == TRUE);
	strcat(pchResponse, "\0");
	//sprintf(pchResponse, "Server get message successful!");
	nMessageLength = strlen(pchResponse) + 1;
	if (!WriteFile(
		hNamedPipe,      // дескриптор канала
		pchResponse,      // адрес буфера для вывода данных
		nMessageLength,  // количество записываемых байтов
		&dwBytesWrite,   // количество записанных байтов
		NULL             // передача данных синхронная
	))
	{
		cerr << "Write file failed." << "\n"
			<< "The last error code: " << GetLastError() << "\n";
		CloseHandle(hNamedPipe);
		cout << "Press any key to exit.";
		cin.get();
		return 0;
	}
	cout << "Message from server send successful.\n";
	// закрываем дескриптор канала
	CloseHandle(hNamedPipe);
	// завершаем процесс
	cout << "Press any key to exit.";
	cin.get();
	return 0;
}

