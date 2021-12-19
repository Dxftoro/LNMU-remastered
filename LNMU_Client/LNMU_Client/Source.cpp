#pragma comment(lib, "ws2_32.lib")
#include<WinSock2.h>

#include<iostream>
#include "DebugView.h"
#include "TheGreatHelper.h"

#pragma warning(disable: 4996)

SOCKET serverConnect;

void ClientHandler() {
	char msg[1024];
	while (true) {
		recv(serverConnect, msg, sizeof(msg), NULL);
		if (msg == NULL) {
			continue;
		}
		else {
			cout << msg << endl;
		}
	}
}

using namespace std;
int main() {
	DebugView act;
	TheGreatHelper help;

	WSADATA wsalib_d;
	WORD DLLVersion = MAKEWORD(1, 3);

	if (WSAStartup(DLLVersion, &wsalib_d) != 0) {
		cout << "Error: " << act.error[0] << endl;
		exit(0);
	}

	SOCKADDR_IN addr;
	int sizea = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(7379);
	addr.sin_family = AF_INET;

	serverConnect = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(serverConnect, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		cout << "Error: " << act.error[1];
		return 2;
	}
	cout << act.msgs[0] << endl;

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	char clmsg[1024];
	while (true) {
		cin.getline(clmsg, sizeof(clmsg));
		send(serverConnect, clmsg, sizeof(clmsg), NULL);
		memset(clmsg, 0, sizeof(clmsg));
	}

	system("pause");
	return 1;
}