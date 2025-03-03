#include <functional>
#include <sstream>

struct Command {
	std::string name;
	std::function<void(const std::vector<std::string>&, ClientManager*, size_t author)> todo;
};

class CommandHandler {
	ClientManager* clients;
	std::vector<Command> commands;
	std::function<void()> defaultCmd;

public:
	CommandHandler(ClientManager* clients);
	~CommandHandler() {}

	void add(std::string name,
		std::function<void(const std::vector<std::string>&, ClientManager*, size_t author)> todo);

	void setDefault(std::function<void()> defaultCmd);
	void handle(std::string cmdString, size_t author);
};

CommandHandler::CommandHandler(ClientManager* clients) {
	this->clients = clients;
}

void CommandHandler::add(std::string name,
	std::function<void(const std::vector<std::string>&, ClientManager*, size_t author)> todo) {
	commands.push_back({ name, todo });
}

void CommandHandler::setDefault(std::function<void()> defaultCmd) {
	this->defaultCmd = defaultCmd;
}

void CommandHandler::handle(std::string cmdString, size_t author) {
	std::istringstream iss(cmdString);
	std::string name; iss >> name;
	std::vector<std::string> args;

	bool commandFound = false;
	size_t index = 0;
	for (size_t i = 0; i < commands.size(); i++) {
		if (commands[i].name == name) {
			commandFound = true;
			index = i;
			break;
		}
	}

	if (!commandFound) {
		if (defaultCmd) defaultCmd();
		return;
	}

	std::string tmp;
	while (!iss.eof() && iss >> tmp) {
		args.push_back(tmp);
	}

	commands[index].todo(args, clients, author);
}