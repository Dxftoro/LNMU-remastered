#pragma once
#include<string>
class DebugView	{
	public:
		std::string error[3] = { "Can't load WSA library!", "User Can't be connected!", "Can't find important files!" };
		std::string result[3] = { "Client connection succesful!", "Listening socket created, waiting for clients...", "Client disconnected!" };
		std::string msgs[1] = { "Welcome to the club buddy!" };
};