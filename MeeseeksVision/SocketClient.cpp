#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <fstream>

#include "MeeseeksProperties.h"

extern MeeseeksProperties g_mp;


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

extern bool g_bSendParams;

CString g_fileName;
bool g_bLoadFile = false;
bool g_bSaveFile = false;
bool g_bGetStats = false;

typedef struct
{
   unsigned int header;
   unsigned int cmd;
} ClientCommand;

class ClientSocket
{
   public    : ClientSocket();

               bool Connect(const char *pAddress, int port);
               int  Send(void *pData, int length);
               int  Receive(void *pData, int length);
               void Close();
   protected :
               SOCKET m_hSocket;
};

ClientSocket::ClientSocket()
{
   m_hSocket = INVALID_SOCKET;
}

bool ClientSocket::Connect(const char *pAddress, int port)
{
   m_hSocket = INVALID_SOCKET;
   
   WSADATA wsaData;
   int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

   if (iResult != 0)
      return false;

   struct addrinfo hints;

   ZeroMemory(&hints, sizeof(hints));
   
   hints.ai_family   = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;

   struct addrinfo *pAddrInfo;

   char portBuffer[100];

   _itoa_s(port, portBuffer, 10);

   iResult = getaddrinfo(pAddress, portBuffer, &hints, &pAddrInfo);
   
   if (iResult != 0) {
      printf("getaddrinfo failed with error: %d\n", iResult);
      WSACleanup();
      return false;
   }

   m_hSocket = socket(pAddrInfo->ai_family, pAddrInfo->ai_socktype, pAddrInfo->ai_protocol);
   
   if (m_hSocket == INVALID_SOCKET) {
      printf("socket failed with error: %ld\n", WSAGetLastError());
      WSACleanup();
      return false;
   }

   // Connect to server.
   iResult = connect(m_hSocket, pAddrInfo->ai_addr, (int) pAddrInfo->ai_addrlen);
   if (iResult == SOCKET_ERROR) {
      closesocket(m_hSocket);
      m_hSocket = INVALID_SOCKET;
      WSACleanup();
      return false;
   }

   freeaddrinfo(pAddrInfo);

   return true;
}

int ClientSocket::Send(void *pData, int length)
{
   if (!m_hSocket)
      return -1;

   return send(m_hSocket, (const char *) pData, length, 0);
}

int ClientSocket::Receive(void *pData, int length)
{
   if (!m_hSocket)
      return -1;

   return recv(m_hSocket, (char *) pData, length, 0);
}

void ClientSocket::Close()
{
   if (m_hSocket) {
      closesocket(m_hSocket);
      m_hSocket = NULL;
   }
   
   WSACleanup();
}

void Log(const wchar_t *pMsg, ...)
{
}

bool SendData(SOCKET hSocket, void *pData, int size)
{
   int iResult = send(hSocket, (const char *) pData, size, 0);

   if (iResult != SOCKET_ERROR) {
      return true;
   }
   
   Log(L"Error 0x%08X sending data", WSAGetLastError());
   
   return false;
}

bool ReceiveData(SOCKET hSocket, void *pData, int size)
{
   int bytesRemaining = size;

   char *pBuffer = (char *) pData;

   while (bytesRemaining > 0)
   {
      int iResult = recv(hSocket, pBuffer, bytesRemaining, 0);

      if (iResult <= 0)
         return false;

      pBuffer += iResult;
     
      bytesRemaining -= iResult;
   }

   return true;
}

#define VIDEO_FRAME_BUFFER_SIZE 640 * 360 * 4

BYTE videoFrame[VIDEO_FRAME_BUFFER_SIZE];

bool GetVideoFrame(SOCKET hSocket)
{
   ClientCommand cmd = {0x0BADF00D, 0x00000000};

   if (!SendData(hSocket, &cmd, sizeof(cmd)))
      return false;

   unsigned int header, byteCount;

   if (!ReceiveData(hSocket, &header, sizeof(header)))
      return false;

   if (header != 0x0BADF00D)
      return false;

   if (!ReceiveData(hSocket, &byteCount, sizeof(byteCount)))
      return false;

   if (byteCount > VIDEO_FRAME_BUFFER_SIZE)
      return false;

   if (!ReceiveData(hSocket, videoFrame, byteCount))
      return false;

   BYTE *pData = new BYTE[byteCount];

   memcpy(pData, videoFrame, byteCount);

   extern HWND g_hZoomWnd;

   ::PostMessage(g_hZoomWnd, WM_USER_NEW_REAR_FRAME, (WPARAM)pData, (LPARAM)byteCount);

   return true;
}

#define MAX_STRING_SIZE 4000
#define STRING_BUFFER_SIZE MAX_STRING_SIZE + 16

char stringBuffer[STRING_BUFFER_SIZE];

bool SendParams(SOCKET hSocket)
{
   ClientCommand cmd = { 0x0BADF00D, 0x00000003 };

   if (!SendData(hSocket, &cmd, sizeof(cmd)))
      return false;

   unsigned int header, byteCount;

   if (!ReceiveData(hSocket, &header, sizeof(header)))
      return false;

   if (header != 0x0BADF00D)
      return false;

   std::string str;

   g_mp.SaveToString(str);

   unsigned int stringLength = str.length();

   if (!SendData(hSocket, &stringLength, sizeof(stringLength)))
      return false;

   const char *pStr = str.c_str();

   if (!SendData(hSocket, (void *) pStr, stringLength))
      return false;

   return true;
}

bool GetParams(SOCKET hSocket)
{
   ClientCommand cmd = { 0x0BADF00D, 0x00000004 };

   if (!SendData(hSocket, &cmd, sizeof(cmd)))
      return false;

   unsigned int header, byteCount;

   if (!ReceiveData(hSocket, &header, sizeof(header)))
      return false;

   if (header != 0x0BADF00D)
      return false;

   if (!ReceiveData(hSocket, &byteCount, sizeof(byteCount)))
      return false;

   if (byteCount > MAX_STRING_SIZE)
      return false;

   memset(stringBuffer, 0, STRING_BUFFER_SIZE);

   if (!ReceiveData(hSocket, stringBuffer, byteCount))
      return false;

   char *pData = new char[byteCount + 16];

   strcpy_s(pData, byteCount + 1, stringBuffer);

   extern HWND g_hMainWnd;

   ::PostMessage(g_hMainWnd, WM_USER_NEW_PARAMS, (WPARAM)pData, (LPARAM)byteCount);

   return true;
}

bool GetStats(SOCKET hSocket)
{
   ClientCommand cmd = { 0x0BADF00D, 0x00000006 };

   if (!SendData(hSocket, &cmd, sizeof(cmd)))
      return false;

   unsigned int header, byteCount;

   if (!ReceiveData(hSocket, &header, sizeof(header)))
      return false;

   if (header != 0x0BADF00D)
      return false;

   if (!ReceiveData(hSocket, &byteCount, sizeof(byteCount)))
      return false;

   if (byteCount > MAX_STRING_SIZE)
      return false;

   memset(stringBuffer, 0, STRING_BUFFER_SIZE);

   if (!ReceiveData(hSocket, stringBuffer, byteCount))
      return false;

   char *pData = new char[byteCount + 16];

   strcpy_s(pData, byteCount + 1, stringBuffer);

   extern HWND g_hMainWnd;

   ::PostMessage(g_hMainWnd, WM_USER_NEW_STATS, (WPARAM)pData, (LPARAM)byteCount);

   return true;
}

bool LoadFile(SOCKET hSocket)
{
   g_bLoadFile = true;

   ClientCommand cmd = { 0x0BADF00D, 0x00000001 };

   if (!SendData(hSocket, &cmd, sizeof(cmd)))
      return false;

   unsigned int header, byteCount;

   if (!ReceiveData(hSocket, &header, sizeof(header)))
      return false;

   if (header != 0x0BADF00D)
      return false;

   _bstr_t bstrFileName = (const wchar_t *) g_fileName;

   std::string str = (const char *) bstrFileName;

   unsigned int stringLength = str.length();

   if (!SendData(hSocket, &stringLength, sizeof(stringLength)))
      return false;

   const char *pStr = str.c_str();

   if (!SendData(hSocket, (void *)pStr, stringLength))
      return false;

   return true;
}

bool SaveFile(SOCKET hSocket)
{
   g_bSaveFile = false;

   ClientCommand cmd = { 0x0BADF00D, 0x00000002 };

   if (!SendData(hSocket, &cmd, sizeof(cmd)))
      return false;

   unsigned int header, byteCount;

   if (!ReceiveData(hSocket, &header, sizeof(header)))
      return false;

   if (header != 0x0BADF00D)
      return false;

   _bstr_t bstrFileName = (const wchar_t *)g_fileName;

   std::string str = (const char *)bstrFileName;

   unsigned int stringLength = str.length();

   if (!SendData(hSocket, &stringLength, sizeof(stringLength)))
      return false;

   const char *pStr = str.c_str();

   if (!SendData(hSocket, (void *)pStr, stringLength))
      return false;

   return true;
}

bool GetFiles(SOCKET hSocket)
{
   ClientCommand cmd = { 0x0BADF00D, 0x00000005 };

   if (!SendData(hSocket, &cmd, sizeof(cmd)))
      return false;

   unsigned int header, byteCount;

   if (!ReceiveData(hSocket, &header, sizeof(header)))
      return false;

   if (header != 0x0BADF00D)
      return false;

   if (!ReceiveData(hSocket, &byteCount, sizeof(byteCount)))
      return false;

   if (byteCount > MAX_STRING_SIZE)
      return false;

   memset(stringBuffer, 0, STRING_BUFFER_SIZE);

   if (!ReceiveData(hSocket, stringBuffer, byteCount))
      return false;

   char *pData = new char[byteCount + 16];

   strcpy_s(pData, byteCount + 1, stringBuffer);

   extern HWND g_hMainWnd;

   ::PostMessage(g_hMainWnd, WM_USER_FILES, (WPARAM)pData, (LPARAM)byteCount);

   return true;
}

void SocketThreadFunc()
{
   int iResult;
   
   HANDLE hStopEvent = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, L"StopSocketThread");
   
   WSADATA wsaData;

   bool bRunning = true;
   
   while (bRunning) {
      SOCKET ConnectSocket = INVALID_SOCKET;
 
      iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
      
      if (iResult != NO_ERROR) {
         Log(L"WSAStartup failed: %d\n", iResult);
         break;
      }
   
      int connectionAttempt = 0;

      bool bReady = false;
   
      while (!bReady) {
         TIMEVAL Timeout;
         Timeout.tv_sec = 1;
         Timeout.tv_usec = 0;
         struct sockaddr_in address;  /* the libc network address data structure */
   
         if (ConnectSocket != INVALID_SOCKET) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
         }
   
         ConnectSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   
         extern CString g_serverAddress;
         extern int     g_serverPort;
   
         InetPton(AF_INET, g_serverAddress, &address.sin_addr.s_addr);
         address.sin_port = htons(g_serverPort);
         address.sin_family = AF_INET;
   
         //set the socket in non-blocking
         unsigned long iMode = 1;
         int iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
         if (iResult != NO_ERROR)
            Log(L"ioctlsocket failed with error: %ld\n", iResult);
   
         if (connect(ConnectSocket, (struct sockaddr *)&address, sizeof(address))) {
            iMode = 0;
            iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
            if (iResult != NO_ERROR)
               Log(L"ioctlsocket failed with error: %ld\n", iResult);
   
            fd_set Write, Err;

            FD_ZERO(&Write);
            FD_ZERO(&Err);
            FD_SET(ConnectSocket, &Write);
            FD_SET(ConnectSocket, &Err);
   
            select(0, NULL, &Write, &Err, &Timeout);
   
            int waitTime = 0;
   
            if (FD_ISSET(ConnectSocket, &Write)) {
               DWORD timeout = 1.0 * 1000;
   
               iResult = setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
   
               if (iResult == SOCKET_ERROR) 
                  Log(L"Error 0x%08X receive setsockopt", WSAGetLastError());
               else
               {
                  iResult = setsockopt(ConnectSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

                  if (iResult == SOCKET_ERROR)
                     Log(L"Error 0x%08X transmit setsockopt", WSAGetLastError());
                  else
                     bReady = true;
               }
            }
            else {
               Log(L"Error 0x%08X connecting", WSAGetLastError());
               waitTime = 1000;
            }
   
            if (::WaitForSingleObject(hStopEvent, waitTime) == WAIT_OBJECT_0) {
               bReady = false;
               bRunning = false;
               break;
            }
         }
      }
   
      bool bContinue = true;
   
   
      if (GetParams(ConnectSocket) && GetFiles(ConnectSocket))
      {
         do {

            bContinue = GetVideoFrame(ConnectSocket);

            if (bContinue) {
               if (g_bSendParams) {
                  bContinue = SendParams(ConnectSocket);
                  g_bSendParams = false;
               }
               if (g_bLoadFile) {
                  if (LoadFile(ConnectSocket))
                     GetParams(ConnectSocket);                  
                  g_bLoadFile = false;
               }
               if (g_bSaveFile) {
                  SaveFile(ConnectSocket);
                  g_bLoadFile = false;
               }
               if (g_bGetStats) {
                  GetStats(ConnectSocket);
                  g_bGetStats = false;
               }
            } 
              
 
            if (::WaitForSingleObject(hStopEvent, 0) == WAIT_OBJECT_0) {
               bContinue = false;
               bRunning = false;
            }
   
         } while (bContinue);
      }
   
      // shutdown the connection since no more data will be sent
      iResult = shutdown(ConnectSocket, SD_SEND);
      if (iResult == SOCKET_ERROR) {
         Log(L"shutdown failed: %d\n", WSAGetLastError());
         closesocket(ConnectSocket);
         WSACleanup();
         ::CloseHandle(hStopEvent);
         return;
      }
      closesocket(ConnectSocket);
      WSACleanup();
   
   } while (bRunning);
   
   ::CloseHandle(hStopEvent);
}