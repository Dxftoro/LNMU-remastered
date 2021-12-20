#pragma comment(lib, "ws2_32.lib")
#include<WinSock2.h>

#include<iostream>
#include<string>
#include<WinBase.h>
#include<fstream>
#include "DebugView.h"

#pragma warning(disable: 4996)

SOCKET clients[45];
int CNT = 0;

void ClientHandler(int index) {
	char msg[1024];
	
	while (true) {
		if (clients[index] != NULL) recv(clients[index], msg, sizeof(msg), NULL);

		for (int i = 0; i < CNT; i++) {
			if (clients[i] != NULL) {
				if (i == index) continue;
				send(clients[i], msg, sizeof(msg), NULL);
			}
		}
	}
}

using namespace std;
int main() {
	DebugView act;
	ifstream fin("settings.txt");
	ifstream fin1("sign.rs");

	WSAData wsalib_d;
	WORD DLLVersion = MAKEWORD(1, 3);

	if (WSAStartup(DLLVersion, &wsalib_d) != 0) {
		cout << "Error: " << act.error[0] << endl;
		exit(0);
	}

	char ip[64];
	string sign;
	int port, cot = 0;
	fin >> ip >> port;
	getline(fin1, sign);

	SOCKADDR_IN addr;
	int sizea = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);
	cout << act.result[1] << endl;

	SOCKET clientCon;
	for (int i = 0; i < 45; i++) {
		clientCon = accept(sListen, (SOCKADDR*)&addr, &sizea);

		if (clientCon == 0) {
			cout << "Error: " << act.error[1] << endl;

		}
		else {
			cout << act.result[0] << " (connection id = " << i << ")" << endl;
			char msg[256];
			strcpy(msg, sign.c_str());
			send(clientCon, msg, sizeof(msg), NULL);
		}

		clients[i] = clientCon;
		CNT++;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)i, NULL, NULL);
	}

	system("pause");
	return 1;
}