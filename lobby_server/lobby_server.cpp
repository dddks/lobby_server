#pragma comment (lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#define S_PORT 7203
#define B_SIZE 1024


/*struct SOCKETINFO
{
	OVERLAPPED overlapped;
	SOCKET sock;
	char buf[B_SIZE + 1];
	int recvbytes;
	int sendbytes;
	WSABUF wsabuf;
};
*/
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);

}

void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);

}

DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	int check;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[B_SIZE + 1];

	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);
	

	while (1)
	{
		check = recv(client_sock, buf, B_SIZE, 0);
		if (check == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (check == 0)
		{
			break;
		}

		buf[check] = '\0';
		printf("[TCP%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);

		check = send(client_sock, buf, check, 0);
		if (check == SOCKET_ERROR)
		{
			err_display("send()");
			break;
			
		
		}
	}

	closesocket(client_sock);
	printf("[TCP����]Ŭ���̾�Ʈ ����: IP�ּ�=%s, ��Ʈ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;


}


int main()
{
	int check;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
	{
		err_quit("socket()");
	}

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(S_PORT);
	check = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (check == SOCKET_ERROR)
	{
		err_quit("bind()");

	}

	check = listen(listen_sock, SOMAXCONN);
	if (check == SOCKET_ERROR)
	{
		err_quit("listen()");
	}

	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;

	HANDLE hThread;


	while (1)
	{
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}
		
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�= %s, ��Ʈ��ȣ : %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));


		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL)
		{
			closesocket(client_sock);

		}
		else
		{
			CloseHandle(hThread);
		}

	}

	closesocket(client_sock);

	WSACleanup();
	return 0;

}