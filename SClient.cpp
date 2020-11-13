#include <windows.h>
#include <iostream>

using namespace std;
int main()
{
  bool menu = 1;
  while (menu) {
    system("cls");
    std::cout
            << "1. Get info from server 1.\n"
            << "2. Get info from server 2.\n"
            << "3. Exit" << "\n";
    int choise;
    cin >> choise;
    switch (choise)
    {
      case 1: {
        ////////////////////////INITIATE//////////////////////////////////
        TCHAR   pipeName[80] = "\\\\.\\pipe\\ServerPipe1";
        HANDLE   hNamedPipe;
        DWORD   dwBytesWritten;    // для количества записанных байтов
        DWORD   dwBytesRead;       // для количества прочитанных байтов
        char  pchMessage[1024];   // для сообщения
        int    nMessageLength;     // длина сообщения
        char  pchResponse[2048];
        ////////////////////////CONNECT_TO_PIPE//////////////////////////////////
        hNamedPipe = CreateFile(
                pipeName,          // имя канала
                GENERIC_READ | GENERIC_WRITE,       // читаем и записываем в канал
                FILE_SHARE_READ | FILE_SHARE_WRITE, // разрешаем чтение и запись
                NULL,                  // безопасность по умолчанию
                OPEN_EXISTING,         // открываем существующий канал
                FILE_ATTRIBUTE_NORMAL, // атрибуты по умолчанию
                NULL);                 // дополнительных атрибутов нет
        // проверяем связь с каналом
        if (hNamedPipe == INVALID_HANDLE_VALUE)
        {
          cerr << "Connection with the named pipe failed." << "\n"
               << "The last error code: " << GetLastError() << "\n";
          cout << "Press any key to exit.";
          cin.get();
          return 0;
        }
        ////////////////////////SEND//////////////////////////////////
        sprintf(pchMessage, "The client requests information!");
        strcat(pchMessage, "\0");
        nMessageLength = sizeof(pchMessage) - 1;
        if (!WriteFile(
                hNamedPipe,      // дескриптор канала
                pchMessage,      // данные
                nMessageLength,  // размер данных
                &dwBytesWritten, // количество записанных байтов
                NULL))           // синхронная запись
        {
          // ошибка записи
          cerr << "Write file failed: " << "\n"
               << "The last error code: " << GetLastError() << "\n";
          CloseHandle(hNamedPipe);
          cout << "Press any key to exit.";
          cin.get();

          return 0;
        }

        //////////////////////// READ //////////////////////////////////
        if (!ReadFile(
                hNamedPipe,      // дескриптор канала
                pchResponse,      // данные
                sizeof(pchResponse),  // размер данных
                &dwBytesRead,    // количество записанных байтов
                NULL))           // синхронное чтение
        {
          // ошибка чтения
          cerr << "Read file failed: " << "\n"
               << "The last error code: " << GetLastError() << "\n";
          CloseHandle(hNamedPipe);
          cout << "Press any key to exit.";
          cin.get();
          return 0;
        }
        cout << "Read ends with error: " << GetLastError() << "\n";
        ////////////////////////OUT_TO_CONSOLE//////////////////////////////////
        cout << "\nThe client received the message from a server: "
             << endl << pchResponse << "\n";
        // закрываем дескриптор канала
        CloseHandle(hNamedPipe);
        // завершаем процесс
        cout << "Press any key to exit.";
        system("pause");
        break;
      }
      case 2: {
        ////////////////////////INITIATE//////////////////////////////////
        TCHAR   pipeName[80] = "\\\\.\\pipe\\ServerPipe2";
        HANDLE   hNamedPipe;
        DWORD   dwBytesWritten;    // для количества записанных байтов
        DWORD   dwBytesRead;       // для количества прочитанных байтов
        char  pchMessage[1024];   // для сообщения
        int    nMessageLength;     // длина сообщения
        char  pchResponse[1024];
        ////////////////////////CONNECT_TO_PIPE//////////////////////////////////
        hNamedPipe = CreateFile(
                pipeName,          // имя канала
                GENERIC_READ | GENERIC_WRITE,       // читаем и записываем в канал
                FILE_SHARE_READ | FILE_SHARE_WRITE, // разрешаем чтение и запись
                NULL,                  // безопасность по умолчанию
                OPEN_EXISTING,         // открываем существующий канал
                FILE_ATTRIBUTE_NORMAL, // атрибуты по умолчанию
                NULL);                 // дополнительных атрибутов нет
        // проверяем связь с каналом
        if (hNamedPipe == INVALID_HANDLE_VALUE)
        {
          cerr << "Connection with the named pipe failed." << "\n"
               << "The last error code: " << GetLastError() << "\n";
          cout << "Press any key to exit.";
          cin.get();
          return 0;
        }
        ////////////////////////SEND//////////////////////////////////
        sprintf(pchMessage, "The client requests information!");
        strcat(pchMessage, "\0");
        nMessageLength = sizeof(pchMessage) - 1;
        if (!WriteFile(
                hNamedPipe,      // дескриптор канала
                pchMessage,      // данные
                nMessageLength,  // размер данных
                &dwBytesWritten, // количество записанных байтов
                NULL))           // синхронная запись
        {
          // ошибка записи
          cerr << "Write file failed: " << "\n"
               << "The last error code: " << GetLastError() << "\n";
          CloseHandle(hNamedPipe);
          cout << "Press any key to exit.";
          cin.get();

          return 0;
        }
        ////////////////////////READ//////////////////////////////////
        if (!ReadFile(
                hNamedPipe,      // дескриптор канала
                pchResponse,      // данные
                sizeof(pchResponse),  // размер данных
                &dwBytesRead,    // количество записанных байтов
                NULL))           // синхронное чтение
        {
          // ошибка чтения
          cerr << "Read file failed: " << endl
               << "The last error code: " << GetLastError() << endl;
          CloseHandle(hNamedPipe);
          cout << "Press any key to exit.\n";
          cin.get();
          return 0;
        }
        ////////////////////////OUT_TO_CONSOLE//////////////////////////////////
        cout << "\nThe client received the message from a server: "
             << endl << pchResponse << endl;
        // закрываем дескриптор канала
        CloseHandle(hNamedPipe);
        // завершаем процесс
        cout << "Press any key to exit.\n";
        system("pause");
        break;
      }
      case 3: {
        menu = 0;
        break;
      }
      default:
        std::cout << "Wrong number!\n";
        break;
    }
  }
  return 0;
}
