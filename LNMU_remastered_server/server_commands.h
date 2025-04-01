void cmdDef() {
	cout << "User tried to call an unknown command!" << endl;
}

void cmdTell(const std::vector<std::string>& args, ClientManager* clients, size_t author) {
	if (args.empty()) return;

	size_t target = clients->findByName(args[0]);
	
	if (target == SIZE_MAX) {
		std::string warnMsg = "User not found";
		Packet packet(serverChatName, warnMsg);
		clients->sendTo(author, (char*)&packet, sizeof(packet), NULL);
		return;
	}

	std::string msgText = "";
	for (size_t i = 1; i < args.size(); i++) {
		msgText += args[i] + " ";
	}
	msgText = "(to you) " + msgText;

	Packet packet(clients->getConnection(author).name, msgText);
	clients->sendTo(target, (char*)&packet, sizeof(packet), NULL);
}

void cmdLs(const std::vector<std::string>& args, ClientManager* clients, size_t author) {
	std::string msgText = "\n\nLIST OF CONNECTED CLIENTS (" +
		std::to_string(clients->clientCount()) + "/" +
		std::to_string(clients->size()) + "):\n";
	size_t clientCount = clients->size();

	for (size_t i = 0; i < clientCount; i++) {
		Connection client = clients->getConnection(i);

		if (client.socket != INVALID_SOCKET) {
			msgText += client.name + 
					"\tid: " + std::to_string(i) + 
					"\tsock_id: " + std::to_string(client.socket) + "\n";
		}
	}

	Packet packet(serverChatName, msgText);
	clients->sendTo(author, (char*)&packet, sizeof(packet), NULL);
}

void cmdHelp(const std::vector<std::string>& args, ClientManager* clients, size_t author) {
	std::string msgText = "\n\nList of avaible commands:\n";
	msgText += "/help -\tthis command;\n";
	msgText += "/tell [nickname] -\twrite a private message to the specified client;\n";
	msgText += "/ls -\tshow all clients connected to the server.\n";

	Packet packet(serverChatName, msgText);
	clients->sendTo(author, (char*)&packet, sizeof(packet), NULL);
}